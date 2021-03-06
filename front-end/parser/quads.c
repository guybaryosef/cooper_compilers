/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * quads.c - Implements the functions associated with
 * quads (IR generation), ie the functions declared 
 * at quads.h. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "quads.h"
#include "../front_end_header.h"
#include "../lexer/lheader.h"
#include "../lexer/lheader2.h"
#include "symbol_table.h"
#include "pheader_ast.h"
#include "../../back-end/assemb_gen.h"



/////////////////////////////////////////////////////////////////////////
///////////////////////////// Basic Blocks //////////////////////////////
/////////////////////////////////////////////////////////////////////////

/**
 * newBasicBlock - Creates and returns a new IR Basic Block.
 */
BasicBlock *newBasicBlock(char *name) {
    static int generic_bb_count = 0;

    BasicBlock *new_block = malloc(sizeof(BasicBlock));

    if (!name) {
        char *str = calloc(10, sizeof(char));
        sprintf(str, "BB_%d", generic_bb_count);
        generic_bb_count++;
        new_block->u_label = str; 
    }
    else
        new_block->u_label = name;

    new_block->next = NULL;
    new_block->quads_ll = NULL;
    new_block->printed = false;
    new_block->translated = false;
    return new_block;
}


/**
 * newBBnode - A constructor for a basic block linked list node (BB_ll_node).
 */
BB_ll_node *newBBnode(BasicBlock *bb) {
    BB_ll_node *new_node = malloc(sizeof(BB_ll_node));
    new_node->bb = bb;
    new_node->next = NULL;
    return new_node;
}


/////////////////////////////////////////////////////////////////////////
///////////////////////////////// Quads /////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/**
 * newQuadLLNode - Creates and returns a new linked list node of a quad.
 */
void newQuadLLNode(Quad *new_quad) {
    QuadLLNode *new_node = malloc(sizeof(QuadLLNode));
    if (!new_node)
        yyerror("Error allocating new quad node :(");

    new_node->quad = *new_quad;

    if (!cur_quad_ll) { /* new basic block */
        cur_basic_block->quads_ll = new_node; 
    }
    else {
        cur_quad_ll->next = new_node;
    }
    cur_quad_ll = new_node;
}




/**
 * generateQuads - Generates the Intermediate Representation (quads) of 
 * a function and stores them into a specified file (stdout be default). 
 */
void generateQuads(astnode *root) {
    if (root->nodetype != STABLE_FNC_DEFINITION)
        yyerror("Attempting to print quads of a non-function definition type!");
    else {
        cur_basic_block = newBasicBlock(root->stable_entry.ident);
        cur_quad_ll = cur_basic_block->quads_ll;

        if (bb_ll.first == NULL) {
            bb_ll.first = newBBnode(cur_basic_block);
            bb_ll.last = bb_ll.first;
        }
        else {
            bb_ll.last->next = newBBnode(cur_basic_block);
            bb_ll.last = bb_ll.last->next;
        }

        genQuads(root->stable_entry.fnc.function_body);
    }
}


/**
 * genQuads - Generates the QUADS of an astnode (and all its children).
 */
astnode *genQuads(astnode *node) {

    astnode *target;
    AstnodeLinkedListNode *cur;
    switch (node->nodetype) {
        case COMPOUND_STMT:
            cur = node->compound_stmt.astnode_ll->first;

            while (cur) {
                if (cur->node != NULL)
                    genQuads(cur->node);
                cur = cur->next;
            }
            return NULL;

        case ASS_TYPE:
            generateAssignmentIR(node);
            return NULL;

        case FNC_CALL:
            generateFunctionCallIR(node, NULL);
            return NULL;

        case CONDITIONAL_STMT:
            generateConditionalIR(node);
            return NULL;

        case WHILE_STMT:
            generateWhileLoopIR(node);
            return NULL;

        case FOR_STMT:
            generateForLoopIR(node);
            return NULL;

        case DO_WHILE_STMT:
            generateDoWhileLoopIR(node);
            return NULL;

        case BREAK_STMT:
            if (break_bb) {
                emitQuad(BR, NULL, newNode_bb(break_bb), NULL);
            }
            else {
                yyerror("Invalid break statement.");
            }
            return NULL;

        case CONTINUE_STMT:
            if (continue_bb)
                emitQuad(BR, NULL, newNode_bb(continue_bb), NULL);
            else
                yyerror("Invalid continue statement.");
            return NULL;

        case RETURN_STMT:
            target = genRvalue(node->return_stmt.expr, NULL);
            emitQuad(RETURN, NULL, target, NULL);
            return NULL;

        case UNOP_TYPE:
            if (node->unop.op == PLUSPLUS || node->unop.op == MINUSMINUS) {
                astnode * tmp = genPostixIncrIR(node, node->unop.op);
                emitQuad(MOVL, tmp, tmp, NULL);
                return NULL;
            }
            // else let logic fall through to the default case
        default:
            yywarn("This line has no useful effect");
            target = newGenericTemp();
            emitQuad(MOVL, target, genRvalue(node, NULL), NULL);
            return target;
    }
}    


/**
 * genPostixIncrIR - Generates IR for a postfix expression.
 */ 
astnode *genPostixIncrIR(astnode *node, int node_type) {
    // set up a tmp variable which will be returned by the function


    // set up the increment value node
    struct YYnum num_val;
    num_val.val = 1;
    num_val.types = NUMMASK_INTGR | NUMMASK_INT;

    // set up the new assignment node
    astnode *new_node = newNode_assment('=');
    new_node->assignment.left = node->unop.expr;

    astnode *new_op;
    if (node_type == PLUSPLUS)
        new_op = newNode_binop('+');
    else
        new_op = newNode_binop('-');

    astnode *tmp = genRvalue(node->unop.expr, NULL);
    new_op->binop.left = node->unop.expr;
    new_op->binop.right = newNode_num(num_val);
    
    new_node->assignment.right = new_op;

    generateAssignmentIR(new_node);

    return tmp;
}


/**
 * generateForLoopIR - Generates IR for a for-loop.
 */
void generateForLoopIR(astnode *node) {

    // generate initial clause IR in existing bb 
    genQuads(node->for_stmt.initial_clause);

    // set up the basic blocks the for loop will consist of
    BasicBlock *condition_bb = newBasicBlock(NULL);
    BasicBlock *loop_bb = newBasicBlock(NULL);
    BasicBlock *increment_bb = newBasicBlock(NULL);
    
    BasicBlock *next_bb = newBasicBlock(NULL);

    // move basic block state to condition basic block
    cur_basic_block->next = condition_bb;
    cur_basic_block = condition_bb;
    cur_quad_ll = cur_basic_block->quads_ll;

    // set up the corresponding cursors for continue and break stmts
    BasicBlock *past_cont_bb = NULL;
    BasicBlock *past_break_bb = NULL;
    if (continue_bb) {
        past_cont_bb = continue_bb;
    }
    if (break_bb)
        past_break_bb = break_bb;

    continue_bb = increment_bb;
    break_bb = next_bb;
    
    generateConditionIR(node->for_stmt.check_expr, loop_bb, next_bb);

    // set up basic block setup for the loop body
    cur_basic_block = loop_bb;
    cur_quad_ll = cur_basic_block->quads_ll;

    // generate quads for the loop body
    genQuads(node->for_stmt.stmt);
    cur_basic_block->next = increment_bb;

    // set up bb setups for the increment expression
    cur_basic_block = increment_bb;
    cur_quad_ll = cur_basic_block->quads_ll;

    // generate quads for the increment expression
    genQuads(node->for_stmt.iteration_expr);
    emitQuad(BR, NULL, newNode_bb(condition_bb), NULL);

    // remove continue and break cursors after loop is done
    if (past_break_bb)
        break_bb = past_break_bb;
    else
        break_bb = NULL;
    
    if (past_cont_bb)
        continue_bb = past_cont_bb;
    else
        continue_bb = NULL;

    // set up next basic block after while loop 
    cur_basic_block = next_bb;
    cur_quad_ll = cur_basic_block->quads_ll;
}


/**
 * generateDoWhileLoopIR - Generates IR for a do-while loop.
 */
void generateDoWhileLoopIR(astnode *node) {

    BasicBlock *if_bb = newBasicBlock(NULL);
    
    BasicBlock *loop_bb = newBasicBlock(NULL);
    loop_bb->next = if_bb;

    BasicBlock *next_bb = newBasicBlock(NULL);

    // move bb state to loop body bb
    cur_basic_block->next = loop_bb;
    cur_basic_block = loop_bb;
    cur_quad_ll = cur_basic_block->quads_ll;

    // set up the corresponding cursors for continue and break stmts
    BasicBlock *past_cont_bb = NULL;
    BasicBlock *past_break_bb = NULL;
    if (continue_bb) {
        past_cont_bb = continue_bb;
    }
    if (break_bb)
        past_break_bb = break_bb;

    continue_bb = if_bb;
    break_bb = next_bb;

    // generate quads for the loop body
    genQuads(node->while_stmt.stmt);

    // set up basic block setup for the loop condition
    cur_basic_block = if_bb;
    cur_quad_ll = cur_basic_block->quads_ll;

    generateConditionIR(node->while_stmt.expr, loop_bb, next_bb);

    // remove continue and break cursors after loop is done
    if (past_break_bb)
        break_bb = past_break_bb;
    else
        break_bb = NULL;
    
    if (past_cont_bb)
        continue_bb = past_cont_bb;
    else
        continue_bb = NULL;

    // set up next basic block after while loop 
    cur_basic_block = next_bb;
    cur_quad_ll = cur_basic_block->quads_ll;
}


/**
 * generateWhileLoopIR - Generates IR for a while loop.
 */
void generateWhileLoopIR(astnode *node) {    
    BasicBlock *if_bb = newBasicBlock(NULL);
    
    BasicBlock *loop_bb = newBasicBlock(NULL);

    BasicBlock *next_bb = newBasicBlock(NULL);

    // move basic block state to while condition basic block
    cur_basic_block->next = if_bb;
    cur_basic_block = if_bb;
    cur_quad_ll = cur_basic_block->quads_ll;

    // set up the corresponding cursors for continue and break stmts
    BasicBlock *past_cont_bb = NULL;
    BasicBlock *past_break_bb = NULL;
    if (continue_bb) {
        past_cont_bb = continue_bb;
    }
    if (break_bb)
        past_break_bb = break_bb;

    continue_bb = if_bb;
    break_bb = next_bb;
    
    generateConditionIR(node->while_stmt.expr, loop_bb, next_bb);

    // set up basic block setup for the loop body
    cur_basic_block = loop_bb;
    cur_quad_ll = cur_basic_block->quads_ll;

    // generate quads for the loop body
    genQuads(node->while_stmt.stmt);
    emitQuad(BR, NULL, newNode_bb(continue_bb), NULL);

    // remove continue and break cursors after loop is done
    if (past_break_bb)
        break_bb = past_break_bb;
    else
        break_bb = NULL;
    
    if (past_cont_bb)
        continue_bb = past_cont_bb;
    else
        continue_bb = NULL;

    // set up next basic block after while loop 
    cur_basic_block = next_bb;
    cur_quad_ll = cur_basic_block->quads_ll;
}


/**
 * generateConditionalStmt - Generates the IR required for 
 * conditional statements.
 */
void generateConditionalIR(astnode *node) {

    // check for logical and and ors- we need to break them apart */
    if (node->conditional_stmt.expr->nodetype == LOG_TYPE) {
        astnode *cond_node = node->conditional_stmt.expr;
        if (cond_node->binop.op == LOGAND) {

            astnode *reorder1 = newNode_conditionalStmt(cond_node->binop.right, 
                                                    node->conditional_stmt.if_node, 
                                                    node->conditional_stmt.else_node);

            astnode *reorder2 = newNode_conditionalStmt(cond_node->binop.left, 
                                                    reorder1, 
                                                    node->conditional_stmt.else_node);
            generateConditionalIR(reorder2);
        }
        else if (cond_node->binop.op == LOGOR) {
            astnode *reorder1 = newNode_conditionalStmt(cond_node->binop.right, 
                                                    node->conditional_stmt.if_node, 
                                                    node->conditional_stmt.else_node);

            astnode *reorder2 = newNode_conditionalStmt(cond_node->binop.left, 
                                                    node->conditional_stmt.if_node, 
                                                    reorder1);
            generateConditionalIR(reorder2);
        }
    }
    else {
        
        BasicBlock *bb_then= newBasicBlock(NULL);
        BasicBlock *bb_else= newBasicBlock(NULL);

        /* check if there exists an else stmt, if not then consolidate the 
        continuing basic block to be the already created else block. */
        BasicBlock *bb_next;
        if (node->conditional_stmt.else_node)
            bb_next = newBasicBlock(NULL);
        else
            bb_next = bb_else;

        // create quads for conditional
        generateConditionIR(node->conditional_stmt.expr, bb_then, bb_else);

        // create quads for 'then' case
        cur_basic_block = bb_then;
        cur_quad_ll = cur_basic_block->quads_ll;

        genQuads(node->conditional_stmt.if_node);
        emitQuad(BR, NULL, newNode_bb(bb_next), NULL);

        // create quads for 'else' case
        if (node->conditional_stmt.else_node) {
            cur_basic_block = bb_else;
            cur_quad_ll = cur_basic_block->quads_ll;
        
            genQuads(node->conditional_stmt.else_node);
            emitQuad(BR, NULL, newNode_bb(bb_next), NULL);
        }

        cur_basic_block = bb_next;
        cur_quad_ll = cur_basic_block->quads_ll;
    }    
}


/**
 * generateConditionIR - Generates quads for a conditional expression.
 */ 
void generateConditionIR(astnode *node, BasicBlock *bb_then, BasicBlock *bb_else) {
    
    // set up an evaluation to 0
    struct YYnum num_val;
    num_val.types = NUMMASK_INTGR | NUMMASK_INT;
    num_val.val = 0;

    // evaluate conditional 'if' stmt
    if (node->nodetype == COMPARE_TYPE) {       /* compare */
        astnode *left_side, *right_side;
        
        if (node->binop.left->nodetype != TEMP_REG_TYPE)
            left_side = genRvalue(node->binop.left, NULL);
        else
            left_side = node->binop.left;

        if (node->binop.right->nodetype != TEMP_REG_TYPE)
            right_side = genRvalue(node->binop.right, NULL);
        else
            right_side = node->binop.right;

        emitQuad(CMP, NULL, left_side, right_side);

        switch(node->binop.op) {
            case '<': emitQuad(BRLT, NULL, newNode_bb(bb_then), newNode_bb(bb_else));   break;
            case '>': emitQuad(BRGT, NULL, newNode_bb(bb_then), newNode_bb(bb_else));   break;
            case LTEQ: emitQuad(BRLE, NULL, newNode_bb(bb_then), newNode_bb(bb_else));  break;
            case GTEQ: emitQuad(BRGE, NULL, newNode_bb(bb_then), newNode_bb(bb_else));  break;
            case EQEQ: emitQuad(BREQ, NULL, newNode_bb(bb_then), newNode_bb(bb_else));  break;
            case NOTEQ:emitQuad(BRNEQ, NULL, newNode_bb(bb_then), newNode_bb(bb_else)); break;
            default:  yyerror("Invalid comparator operator");
        }
    }
    else if (node->nodetype == STABLE_VAR) {    /* compare to 0 */
        emitQuad(CMP, NULL, node, newNode_num(num_val));
        emitQuad(BRNEQ, NULL, newNode_bb(bb_then), newNode_bb(bb_else));
    }
    else {                                  /* compare to 0 */
        astnode *tmp_target = genRvalue(node, NULL);

        emitQuad(CMP, NULL, tmp_target, newNode_num(num_val));
        emitQuad(BRNEQ, NULL, newNode_bb(bb_then), newNode_bb(bb_else));
    }
}


/**
 * generateFunctionCall - Generates the QUADS necessary for a 
 * function call. We will attempt to keep this architecture 
 * independent, thereby pushing off many of the architecture
 * specific parts of the function call assembly code to the
 * backend.
 */
void generateFunctionCallIR(astnode *node, astnode *target) {
    if (node->nodetype != FNC_CALL)
        yyerror("Cannot create a funciton call for a non-function call type");

    struct YYnum num_val;
    num_val.val = node->fnc.arg_count;
    num_val.types = NUMMASK_INTGR | NUMMASK_INT;

    emitQuad(ARGBEGIN, NULL, newNode_num(num_val), NULL);

    for (int i = node->fnc.arg_count-1 ; i > -1 ; --i) {
        num_val.val = i+1;
        astnode *new_tmp = genRvalue(node->fnc.arguments[i]->arg.expr, NULL);
        emitQuad(ARG, NULL, newNode_num(num_val), new_tmp);
    }

    if (target)
        emitQuad(CALL, target, node->fnc.ident, NULL);
    else
        emitQuad(CALL, NULL, node->fnc.ident, NULL);
}


/**
 * generateAssignmentIR - Generates the IR of an assignment operation.
 */
void generateAssignmentIR(astnode *node) {
    
    enum LvalueMode l_mode;
    astnode *des = genLvalue(node->assignment.left, &l_mode);

    if (des == NULL)
        yyerror("Invalid assignment of an l-value.");
    else if (l_mode == DIRECT) {
        astnode *r_val = genRvalue(node->assignment.right, des);
    }
    else {      /* indirect l-value mode */
        astnode *r_val = genRvalue(node->assignment.right, NULL);
        emitQuad(STORE, NULL, r_val, des);
    }
    
}


/**
 * genLvalue - Generates the L values of an expression.
 */
astnode *genLvalue(astnode *node, enum LvalueMode *mode) {

    switch (node->nodetype) {
        case STABLE_VAR:
            *mode = DIRECT;
            return node;
        case NUM_TYPE:
            return NULL;
        case DEREF_TYPE:
            *mode = INDIRECT;
            return genRvalue(node->unop.expr, NULL);
    }
}


/**
 * genRvalue - Generates the r-values of an expression.
 * 
 * We check for whether the target is defined in each if
 * statement instead of once at the beginning because for
 * efficient quad generation, we want each tmp register=variable
 * to get initialized as soon as possible to when it will 
 * get used as a source.
 */
astnode *genRvalue(astnode *node, astnode *target) {

    if (node->nodetype == TEMP_REG_TYPE) {
        if (!target) target = newGenericTemp();
        emitQuad(MOVL, target, node, NULL);
        return target;
    }
    if (node->nodetype == STABLE_VAR && node->stable_entry.node->nodetype == ARRAY_TYPE) {
        if (!target) target = newGenericTemp();
        emitQuad(LEA, target, node, NULL);
        return target;
    }
    else if (node->nodetype == STABLE_VAR) {
        if (!target) target = newGenericTemp();

        if (target->nodetype == STABLE_VAR) {
            astnode *tmp_tag = newGenericTemp();
            emitQuad(MOVL, tmp_tag, node, NULL);
            emitQuad(MOVL, target, tmp_tag, NULL);
        }
        else {
            if (node->stable_entry.node->scalar_type.type == Char)
                emitQuad(MOVB, target, node, NULL);
            else
                emitQuad(MOVL, target, node, NULL);
        }

        return target;
    }
    else if (node->nodetype == NUM_TYPE) {
        if (!target) target = newGenericTemp();
        emitQuad(MOVL, target, node, NULL);
        return target;
    }
    else if (node->nodetype == CHRLIT_TYPE) {
        if (!target) target = newGenericTemp();
        emitQuad(MOVB, target, node, NULL);
        return target;
    }
    else if (node->nodetype == IDENT_TYPE) {
        if (!target) target = newGenericTemp();
        emitQuad(MOVL, target, node, NULL);
        return target;
    }
    else if (node->nodetype == STRLIT_TYPE) {
        if (!target) target = newGenericTemp();
        emitQuad(MOVL, target, node, NULL);
        return target;
    }
    else if (node->nodetype == BINOP_TYPE) {
        // for now ignore type values
        astnode *left = genRvalue(node->binop.left, NULL);
        astnode *right = genRvalue(node->binop.right, NULL);
        
        enum QuadOpcode op;
        switch (node->binop.op)  {
            case '%':    op = MODL;     break;
            case '^':    op = XORL;     break;
            case '&':    op = ANDL;     break;
            case '*':    op = MULL;     break;
            case '-':    op = SUBL;     break;
            case '+':    op = ADDL;     break;
            case '|':    op = ORQ;      break;
            case '/':    op = DIVL;     break;
            case SHL:    op = SHL;      break;
            case SHR:    op = SHR;      break;
            case ',':    op = COMMA;    break;
        }
        
        // if doing addition of anything regarding pointers, need to do poitner arithmetic
        if (node->binop.op == '+' || node->binop.op == '-') {
            
            astnode *num_val;
            if ( (node->binop.left->nodetype == STABLE_VAR && (
                node->binop.left->stable_entry.node->nodetype == PTR_TYPE || 
                node->binop.left->stable_entry.node->nodetype == ARRAY_TYPE) ) && 
                (node->binop.right->nodetype == NUM_TYPE || 
                node->binop.right->nodetype == STABLE_VAR && 
                node->binop.right->stable_entry.node->nodetype == SCALAR_TYPE) ) 
            {
                if (node->binop.left->stable_entry.node->nodetype == PTR_TYPE)
                    num_val = evaluateSizeOf(node->binop.left->stable_entry.node->ptr.pointee);
                else
                    num_val = evaluateSizeOf(node->binop.left->stable_entry.node->arr.ptr->ptr.pointee);


                astnode *tmp = newGenericTemp();
                emitQuad(MULL, tmp, right, num_val);
                right = tmp;
            }
            else if ( (node->binop.right->nodetype == STABLE_VAR && (
                    node->binop.right->stable_entry.node->nodetype == PTR_TYPE || 
                    node->binop.right->stable_entry.node->nodetype == ARRAY_TYPE) ) && 
                    (node->binop.left->nodetype == NUM_TYPE || 
                    node->binop.left->nodetype == STABLE_VAR && 
                    node->binop.left->stable_entry.node->nodetype == SCALAR_TYPE)) 
            {
                if (node->binop.right->stable_entry.node->nodetype == PTR_TYPE)
                    num_val = evaluateSizeOf(node->binop.right->stable_entry.node->ptr.pointee);
                else
                    num_val = evaluateSizeOf(node->binop.right->stable_entry.node->arr.ptr->ptr.pointee);


                astnode *tmp = newGenericTemp();
                emitQuad(MULL, tmp, left, num_val);
                left = tmp;
            }
            /* if both are pointers, need to confirm they point to the same type and
            also need to make sure that the result is the differnece in their indicies */
            else if ( (node->binop.right->nodetype == STABLE_VAR && node->binop.left->nodetype == STABLE_VAR) &&                
                        (node->binop.right->stable_entry.node->nodetype == PTR_TYPE || 
                      node->binop.right->stable_entry.node->nodetype == ARRAY_TYPE) &&
                     (node->binop.left->stable_entry.node->nodetype == PTR_TYPE || 
                      node->binop.left->stable_entry.node->nodetype == ARRAY_TYPE)) 
            {
                astnode * size;
                if (size = confirmSamePointer(node->binop.right->stable_entry.node, 
                                                node->binop.left->stable_entry.node)) {
                    /* emit the binop op. then divide by the size of the type */
                    astnode *tmp_targ = newGenericTemp();
                    emitQuad(op, tmp_targ, left, right);

                    if (!target) target = newGenericTemp();
                    emitQuad(DIVL, target, tmp_targ, size);
                    return target;
                }              
                else
                    yyerror("Unable to do pointer arithmetic on differing types!");
            }
        }

        if (!target) target = newGenericTemp();
        emitQuad(op, target, left, right);
        
        return target;
    }
    else if (node->nodetype == LOG_TYPE) {
        // for now ignore type values
        astnode *left = genRvalue(node->binop.left, NULL);
        astnode *right = genRvalue(node->binop.right, NULL);
        
        enum QuadOpcode op;
        switch (node->binop.op) {
            case LOGAND: op = LOGN; break;
            case LOGOR:  op = LOGO; break;
        }

        if (!target) target = newGenericTemp();
        emitQuad(op, target, left, right);
        
        return target;
    }
    else if (node->nodetype == UNOP_TYPE) {
        // for now ignore type values

        // a bit more logic if it is a postfix increment
        if (node->unop.op == PLUSPLUS || node->unop.op == MINUSMINUS) {
            return genPostixIncrIR(node, node->unop.op);
        }
        else {
            // convert sizeof operator to an constant value
            astnode *expr = genRvalue(node->unop.expr, NULL);
            enum QuadOpcode op;
            switch (node->unop.op) { 
                case '~':       op = COMPLL;        break;
                case '-':       op = NEG;           break;
                case '+':       op = MOVL;           break;
                case '!':       op = LOG_NEG_EXPR;  break;
            }
        
            if (!target) target = newGenericTemp();
            emitQuad(op, target, expr, NULL);
            return target;
        }
    }
    else if (node->nodetype == SIZEOF_TYPE) {
        astnode *expr = evaluateSizeOf(node->unop.expr);

        if (!target) target = newGenericTemp();
        emitQuad(MOVL, target, expr, NULL);
        return target;
    }
    else if (node->nodetype == DEREF_TYPE) {
        astnode *left = genRvalue(node->unop.expr, NULL);

        if (!target) target = newGenericTemp();
        emitQuad(LOAD, target, left, NULL);
        return target;
    }
    else if (node->nodetype == ADDR_TYPE) {

        if (node->unop.expr->nodetype == DEREF_TYPE) {
            if (!target) target = newGenericTemp();
            genRvalue(node->unop.expr->unop.expr, target); 
        }
        else {
            if (node->unop.expr->nodetype == STABLE_VAR) {
                if (!target) target = newGenericTemp();
                    emitQuad(LEA, target, node->unop.expr, NULL);
            }
            else {
                astnode *expr = genRvalue(node->unop.expr, NULL);
                if (!target) target = newGenericTemp();
                    emitQuad(LEA, target, expr, NULL);
            }
        }
        return target;
    }
    else if (node->nodetype == COMPARE_TYPE) {
        emitQuad(CMP, NULL, node->binop.left, node->binop.right);
        switch(node->binop.op) {
            case '<': emitQuad(CC_LT, target, NULL, NULL);   break;
            case '>': emitQuad(CC_GT, target, NULL, NULL);   break;
            case LTEQ: emitQuad(CC_LE, target, NULL, NULL);  break;
            case GTEQ: emitQuad(CC_GE, target, NULL, NULL);  break;
            case EQEQ: emitQuad(CC_EQ, target, NULL, NULL);  break;
            case NOTEQ:emitQuad(CC_NEQ, target, NULL, NULL); break;
            default:  yyerror("Invalid comparator operator");
        }
        if (!target) target = newGenericTemp();
        return target;
    }
    else if (node->nodetype == FNC_CALL) {
        if (!target) target = newGenericTemp();
        generateFunctionCallIR(node, target);
        return target;
    }
    return NULL;
}




/**
 * emitQuad - generates a new quad with the specified opcode and the generic
 * nodes des (destination), source 1 (src 1), and source 2 (src2).
 * 
 * Note that although the function returns a pointer to the QUAD, we will 
 * almost never actually store the returned pointer.
 */
Quad *emitQuad(enum QuadOpcode op, astnode *des, astnode *src1, astnode *src2) {
    Quad *new_quad = malloc(sizeof(Quad));
    new_quad->opcode = op;
    new_quad->result = des;
    new_quad->src1 = src1;
    new_quad->src2 = src2;

    newQuadLLNode(new_quad);
    return new_quad;
}


/**
 * printBB_ll - Prints the IR of the file, ie the IR for each function in the 
 * file, ie the IR for each basic block in the basic-block-linked-list.
 */
void printBB_ll(BB_ll *ll) {
    BB_ll_node *cur = ll->first;

    while (cur) {
        printBB(cur->bb, false);
        cur = cur->next;
    }
}


/**
 * printBB - Prints out to stdout the basic block.
 */
Quad *printBB(BasicBlock *bb, _Bool in_conditional_arm) {
    if (!bb || bb->printed)
        return NULL;
    
    fprintf(output_file, "%s:\n", bb->u_label);
    QuadLLNode *cur_node = bb->quads_ll;

    Quad *last_quad = NULL;
    while(cur_node) {
        last_quad = &cur_node->quad;

        printQuad(*last_quad);
        cur_node = cur_node->next;
    }

    bb->printed = true;
    
    if (last_quad && (
        last_quad->opcode == BRLE || last_quad->opcode == BRGE ||
        last_quad->opcode == BRLT || last_quad->opcode == BRGT ||
        last_quad->opcode == BRNEQ || last_quad->opcode == BREQ) )
    {      
        Quad *new_last_quad1 = printBB(last_quad->src1->bb_type.bb, true);
        if (new_last_quad1 && new_last_quad1->opcode == BR && 
        new_last_quad1->src1->bb_type.bb == last_quad->src2->bb_type.bb) 
        {
                printBB(last_quad->src2->bb_type.bb, false);
                return NULL;
        }
        else {
            Quad *new_last_quad2 = printBB(last_quad->src2->bb_type.bb, true);
            if (new_last_quad2 && new_last_quad2->opcode == BR) 
            {
                if (new_last_quad1 && new_last_quad1->opcode == BR && 
                        new_last_quad1->src1->bb_type.bb == new_last_quad2->src1->bb_type.bb) 
                {
                    return printBB(new_last_quad1->src1->bb_type.bb, in_conditional_arm);
                }
                else if (in_conditional_arm) {
                    return new_last_quad2;
                }
                else {
                    printBB(new_last_quad2->src1->bb_type.bb, false);
                    return NULL;
                }
            }
        }
    }
    else if (bb->next) {

        return printBB(bb->next, in_conditional_arm);
    }
    else if (in_conditional_arm && last_quad && last_quad->opcode == BR) {
        return last_quad;
    }
    else if (!in_conditional_arm && last_quad && last_quad->opcode == BR) {
        printBB(last_quad->src1->bb_type.bb, false);
    }

    return NULL;
}


/**
 * printQuad - Prints out to stdout a QUAD intermediate representation.
 */
void printQuad(Quad quad) {
    fprintf(output_file, "        ");
    if (quad.result != NULL) {
        char *val = node2str(quad.result);
        char *tmp = malloc(sizeof(char)*(strlen(val) + 2));
        strcpy(tmp, val);
        tmp[strlen(val)] = '=';
        fprintf(output_file, "%-7s", tmp);
    }
    else
        printf("       ");
    
    fprintf(output_file, "%-8s", op2str(quad.opcode));
    
    if (quad.src1 != NULL) 
        fprintf(output_file, "%s", node2str(quad.src1));

    if (quad.src1 && quad.src2)
        fprintf(output_file, ",");

    if (quad.src2 != NULL)
        fprintf(output_file, "%s", node2str(quad.src2));
    
    fprintf(output_file, "\n");
}

/**
 * op2str - A helper function to output the generated QUADs opcode. 
 */
char *op2str(enum QuadOpcode op) {
    switch (op) {
        case LT:    return "LT";
        case BR:    return "BR";
        case GT:    return "GT";
        case ORB:   return "ORB";
        case ORW:   return "ORW";
        case ORL:   return "ORL";
        case ORQ:   return "ORQ";
        case NEG:   return "NEG";
        case LEA:   return "LEA";
        case ARG:   return "ARG";
        case CMP:   return "CMP";
        case BREQ:  return "BREQ";
        case BRLT:  return "BRLT";
        case BRLE:  return "BRLE";
        case BRGT:  return "BRGT";
        case BRGE:  return "BRGE";
        case MOVB:  return "MOVB";
        case MOVW:  return "MOVW";
        case MOVL:  return "MOVL";
        case MOVQ:  return "MOVQ";
        case ADDB:  return "ADDB";
        case ADDW:  return "ADDW";
        case ADDL:  return "ADDL";
        case ADDQ:  return "ADDQ";
        case MODB:  return "MODB";
        case MODW:  return "MODW";
        case MODL:  return "MODL";
        case MODQ:  return "MODQ";
        case XORB:  return "XORB";
        case XORW:  return "XORW";
        case XORL:  return "XORL";
        case XORQ:  return "XORQ";
        case ANDB:  return "ANDB";
        case ANDW:  return "ANDW";
        case ANDL:  return "ANDL";
        case ANDQ:  return "ANDQ";
        case MULB:  return "MULB";
        case MULW:  return "MULW";
        case MULL:  return "MULL";
        case MULQ:  return "MULQ";
        case SUBB:  return "SUBB";
        case SUBW:  return "SUBW";
        case SUBL:  return "SUBL";
        case SUBQ:  return "SUBQ";
        case DIVB:  return "DIVB";
        case DIVW:  return "DIVW";
        case DIVL:  return "DIVL";
        case DIVQ:  return "DIVQ";
        case PLPL:  return "PLPL";
        case CALL:  return "CALL";
        case LOAD:  return "LOAD";
        case LOGO:  return "LOGO";
        case LOGN:  return "LOGN";
        case BRNEQ: return "BRNEQ";
        case STORE: return "STORE";
        case COMMA: return "COMMA";
        case CC_LT: return "CC_LT";
        case CC_GT: return "CC_GT";
        case CC_EQ: return "CC_EQ";
        case CC_GE: return "CC_GE";
        case CC_LE: return "CC_LE";
        case RETURN:        return "RETURN";
        case CC_NEQ:        return "CC_NEQ";
        case MINMIN:        return "MINMIN";
        case COMPLB:        return "COMPLB";
        case COMPLW:        return "COMPLW";
        case COMPLL:        return "COMPLL";
        case COMPLQ:        return "COMPLQ";
        case SHL_OP:        return "SHL_OP";
        case SHR_OP:        return "SHR_OP";
        case ARGBEGIN:      return "ARGBEGIN";
        case LOG_NEG_EXPR:  return "LOG_NEG_EXPR";
    }
}

/**
 * node2str - A helper function used to output the value of a node in QUADS.
 */
char *node2str(astnode *node) {
    char *str_val = malloc(sizeof(char)*256);

    if (node->nodetype == TEMP_REG_TYPE)
        sprintf(str_val, "%s", node->ident.str);
    else if (node->nodetype == IDENT_TYPE) {
        sprintf(str_val, "%s", node->ident.str);
    }
    else if (node->nodetype == STABLE_FNC_DECLARATOR ||
        node->nodetype == STABLE_FNC_DEFINITION ) {
        sprintf(str_val, "%s", node->stable_entry.ident);
    }
    else if (node->nodetype == NUM_TYPE) {
        if (node->num.types & NUMMASK_INTGR) {
            if (node->num.types & NUMMASK_INT) {
                if (node->num.types & NUMMASK_UNSIGN)
                    sprintf(str_val, "%d", (int) node->num.val);
                else 
                    sprintf(str_val, "%u", (unsigned int) node->num.val);            
            }
            else if (node->num.types & NUMMASK_LONG) {
                if (node->num.types & NUMMASK_UNSIGN)
                    sprintf(str_val, "%lu", (long) node->num.val);
                else 
                    sprintf(str_val, "%ld", (unsigned long) node->num.val);            
            }
            else { /* long long by default */
                if (node->num.types & NUMMASK_UNSIGN)
                    sprintf(str_val, "%llu", node->num.val);
                else 
                    sprintf(str_val, "%lld", (long long ) node->num.val);
            }
        }
        else if (node->num.types & NUMMASK_FLOAT)
            sprintf(str_val, "%f", (float) node->num.d_val);
        else if (node->num.types & NUMMASK_DOUBLE)
            sprintf(str_val, "%f", (double) node->num.d_val);
        else
            sprintf(str_val, "%f", node->num.d_val);
    }
    else if (node->nodetype == CHRLIT_TYPE) {
        sprintf(str_val, "%d", node->chrlit.c_val);
    }
    else if (node->nodetype == IDENT_TYPE) {
        sprintf(str_val, "%s", node->ident.str);
    }
    else if (node->nodetype == STRLIT_TYPE) {
        snprintf(str_val, node->strlit.str_size + 1, "%s", node->strlit.str);
    }
    else if (node->nodetype == STABLE_VAR || node->nodetype == STABLE_IDENT_TYPE) {
        sprintf(str_val, "%s", node->stable_entry.ident);
    }
    else if (node->nodetype == BASIC_BLOCK_TYPE) {
        sprintf(str_val, "%s", node->bb_type.bb->u_label);
    }

    return str_val;
}


/**
 * evaluateSizeOf - Evalutates the SizeOf operators into a constant.
 * Because we are not implementing a variable length array any time
 * soon, this will always evaluate into a constant during compile-time.
 */
astnode *evaluateSizeOf(astnode *node) {
    struct YYnum num_val;
        num_val.d_val = 0;
        num_val.types = NUMMASK_INTGR;
        num_val.types |= NUMMASK_INT;

    astnode *resulting_size;

    if (node->nodetype == STABLE_VAR)
        node = node->stable_entry.node;

    switch(node->nodetype) {
        case PTR_TYPE:
            num_val.val = DATATYPE_POINTER;
            return newNode_num(num_val);
        case NUM_TYPE:

            if (node->num.types & NUMMASK_DOUBLE) {
                num_val.val = DATATYPE_DOUBLE;
                return newNode_num(num_val);
            }
            else if (node->num.types & NUMMASK_FLOAT) {
                num_val.val = DATATYPE_FLOAT;
                return newNode_num(num_val);
            }
            else if (node->num.types & NUMMASK_INT) {
                num_val.val = DATATYPE_INTEGER_SIZE;
                return newNode_num(num_val);
            }
            else if (node->num.types & NUMMASK_LDBLE) {
                num_val.val = DATATYPE_LONGDOUBLE;
                return newNode_num(num_val);
            }
            else if (node->num.types & NUMMASK_LL) {
                num_val.val = DATATYPE_LONGLONG;
                return newNode_num(num_val);
            }
            else if (node->num.types & NUMMASK_LONG) {
                num_val.val = DATATYPE_LONG;
                return newNode_num(num_val);
            }
            else
                return NULL;
        case SCALAR_TYPE:

            if (node->scalar_type.type == Void) {
                num_val.val = DATATYPE_POINTER;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == Char) {
                num_val.val = DATATYPE_CHAR_SIZE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == Short) {
                num_val.val = DATATYPE_SHORT_SIZE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == Int) {
                num_val.val = DATATYPE_INTEGER_SIZE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == Long) {
                num_val.val = DATATYPE_LONG;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == LongLong) {
                num_val.val = DATATYPE_LONGLONG;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == Bool) {
                num_val.val = DATATYPE_CHAR_SIZE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == Float) {
                num_val.val = DATATYPE_FLOAT;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == Double) {
                num_val.val = DATATYPE_DOUBLE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == LongDouble) {
                num_val.val = DATATYPE_LONGDOUBLE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == FloatComplex) {
                num_val.val = DATATYPE_FLOAT;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == DoubleComplex) {
                num_val.val = DATATYPE_DOUBLE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == LongDoubleComplex) {
                num_val.val = DATATYPE_LONGDOUBLE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == FloatImag) {
                num_val.val = DATATYPE_FLOAT;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == DoubleImag) {
                num_val.val = DATATYPE_DOUBLE;
                return newNode_num(num_val);
            }
            else if (node->scalar_type.type == LongDoubleImag) {
                num_val.val = DATATYPE_LONGDOUBLE;
                return newNode_num(num_val);
            }
            else
                return NULL;
        case ARRAY_TYPE:
            resulting_size = evaluateSizeOf(node->arr.ptr->ptr.pointee);
            num_val.val = resulting_size->num.val * node->arr.size;
            return newNode_num(num_val);
    }            
}


/**
 * confirmSamePointer - Checks that the 2 AST nodes inputted in are pointing
 * to the same type. If so, returns size of the type in an integer AST node.
 * Else returns a NULL AST pointer.
 */
astnode *confirmSamePointer(astnode *node1, astnode *node2) {
    if (node1->nodetype != node2->nodetype)
        return NULL;
    else {
        astnode *val1, *val2;
        switch (node1->nodetype) {
            case SCALAR_TYPE:
            case NUM_TYPE:
                val1 = evaluateSizeOf(node1);
                val2 = evaluateSizeOf(node2);
                if (val1->num.val == val2->num.val)
                    return val1;
                else 
                    return NULL;
            case ARRAY_TYPE:
                return confirmSamePointer(node1->arr.ptr->ptr.pointee, node2->arr.ptr->ptr.pointee);
            case PTR_TYPE:
                return confirmSamePointer(node1->ptr.pointee, node2->ptr.pointee);
        }
    }
}


/**
 * newGenericTemp - Generates a new generic node that is of temporary
 * type. There temporary types will be used when evaluating
 * subexpressions and the like. We will defer register allocation until
 * the BackEnd and so for now we will assume an infinite supply of 
 * virtual registers in which to place temporary values.
 */
astnode *newGenericTemp() {
    static int generic_node_count = 0;

    char *str = calloc(10, sizeof(char));
    sprintf(str, "%%T%d", generic_node_count);
    generic_node_count++;

    astnode *node = malloc(sizeof(astnode));

    node->nodetype = TEMP_REG_TYPE;
    node->ident.str = str;
    return node;
}