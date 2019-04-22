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


#include "quads.h"
#include "../front_end_header.h"
#include "../lexer/lheader.h"
#include "../lexer/lheader2.h"
#include "symbol_table.h"
#include "pheader_ast.h"



/////////////////////////////////////////////////////////////////////////
///////////////////////////// Basic Blocks //////////////////////////////
/////////////////////////////////////////////////////////////////////////

/**
 * newBasicBlock - Creates and returns a new IR Basic Block.
 */
BasicBlock *newBasicBlock(char *name) {
    BasicBlock *new_block = calloc(1, sizeof(BasicBlock));
    new_block->u_label = name;
    
    
    return new_block;
}



/////////////////////////////////////////////////////////////////////////
///////////////////////////////// Quads /////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/**
 * newQuadLLNode - Creates and returns a new linked list node of a quad.
 */
QuadLLNode *newQuadLLNode(Quad *new_quad) {
    QuadLLNode *new_node = malloc(sizeof(QuadLLNode));
    if (!new_node)
        yyerror("Error allocating new quad node :(");

    new_node->quad = *new_quad;

    if (!cur_quad_ll) {
        cur_basic_block->quads_ll = new_node; 
    }
    else {
        cur_quad_ll->next = new_node;
    }
        cur_quad_ll = new_node;
}


/**
 * newGenericTemp - Generates a new generic node that is of temporary
 * type. There temporary types will be used when evaluating
 * subexpressions and the like. We will defer register allocation until
 * the BackEnd and so for now we will assume an infinite supply of 
 * virtual registers in which to place temporary values.
 */
astnode *newGenericTemp() {
    char *str = calloc(10, sizeof(char));
    sprintf(str, "%%T%d", generic_node_count);
    generic_node_count++;

    astnode *node = malloc(sizeof(astnode));

    node->nodetype = IDENT_TYPE;
    node->ident.str = str;
    return node;
}

/**
 * printQuads - Prints out the Intermediate Representation (quads) of
 * the expressions inside a compound statement. 
 */
void generateQuadsHelper(astnode *compound_stmt, FILE *output_file) {

    AstnodeLinkedListNode *cur = compound_stmt->compound_stmt.astnode_ll->first;
    while (cur) {
        if (cur->node != NULL)
            genQuads(cur->node);
        cur = cur->next;
    }
}


/**
 * genQuads - Generates the QUADS of an astnode (and all its children).
 */
astnode *genQuads(astnode *node) {
    astnode *target  = newGenericTemp();
    switch (node->nodetype) {
        case ASS_TYPE:
            generateAssignmentIR(node);
            return NULL;
        case FNC_CALL:
            generateFunctionCall(node);
            return NULL;
        default:
            emitQuad(MOVQ, target, genRvalue(node, NULL), NULL);
            return target;
    }
}    


/**
 * generateQuads - Generates the Intermediate Representation (quads) of 
 * a function and stores them into a specified file (stdout be default). 
 */
void generateQuads(astnode *root, FILE *output_file) {
    FILE *output = (output_file) ? output_file : stdout;

    if (root->nodetype != STABLE_FNC_DEFINITION)
        yyerror("Attempting to print quads of a non-function definition type!");
    else
        generateQuadsHelper(root->stable_entry.fnc.function_body, output);
}

/**
 * emitQuad - generates a new quad with the specified opcode and the generic
 * nodes des (destination), source 1 (src 1), and source 2 (src2).
 */
Quad *emitQuad(enum QuadOpcode op, astnode *des, astnode *src1, astnode *src2) {
    Quad *new_quad = malloc(sizeof(Quad));
    new_quad->opcode = op;
    new_quad->result = des;
    new_quad->src1 = src1;
    new_quad->src2 = src2;
    return new_quad;
}


/**
 * printQuad - Prints out to stdout a QUAD intermediate representation.
 */
void printQuad(Quad *quad) {
    if (quad->result != NULL) {
        char *val = node2str(quad->result);
        char *tmp = malloc(sizeof(char)*(strlen(val) + 2));
        strcpy(tmp, val);
        tmp[strlen(val)] = '=';
        printf("%-7s", tmp);
    }
    else
        printf("       ");
    
    printf("%-8s", op2str(quad->opcode));
    
    if (quad->src1 != NULL) 
        printf("%s", node2str(quad->src1));

    if (quad->src1 && quad->src2)
        printf(",");

    if (quad->src2 != NULL)
        printf("%s", node2str(quad->src2));
    
    printf("\n");
}

/**
 * op2str - A helper function to output the generated QUADs opcode. 
 */
char *op2str(enum QuadOpcode op) {
    switch (op) {
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
        case COMPLB:return "COMPLB";
        case COMPLW:return "COMPLW";
        case COMPLL:return "COMPLL";
        case COMPLQ:return "COMPLQ";
        case ORB:   return "ORB";
        case ORW:   return "ORW";
        case ORL:   return "ORL";
        case ORQ:   return "ORQ";
        case LT:    return "LT:";
        case GT:    return "GT:";
        case SHL_OP:return "SHL_OP";
        case SHR_OP:return "SHR_OP";
        case LTEQ_OP: return "LTEQ_OP";
        case GTEQ_OP: return "GTEQ_OP";
        case DIVB:  return "DIVB";
        case DIVW:  return "DIVW";
        case DIVL:  return "DIVL";
        case DIVQ:  return "DIVQ";
        case EQEQ_OP: return "EQEQ_OP";
        case NOTEQ_OP:return "NOTEQ_OP";
        case LOGO:  return "LOGO";
        case LOGN:  return "LOGN";
        case COMMA: return "COMMA";
        case DEREF: return "DEREF";
        case PLPL:  return "PLPL";
        case MINMIN:return "MINMIN";
        case NEG:   return "NEG";
        case POS:   return "POS";
        case LOG_NEG_EXPR:  return "LOG_NEG_EXPR";
        case ARGBEGIN:      return "ARGBEGIN";
        case STORE: return "STORE";
        case LOAD:  return "LOAD";
        case LEA:   return "LEA";
        case ARG:   return "ARG";
        case CALL:  return "CALL";
    }
}

/**
 * node2str - A helper function used to output the value of a node in QUADS.
 */
char *node2str(astnode *node) {
    char *str_val = malloc(sizeof(char)*256);

    if (node->nodetype == NUM_TYPE) {
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
        sprintf(str_val, "%c", node->chrlit.c_val);
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

    return str_val;
}


/**
 * generateFunctionCall - Generates the QUADS necessary for a 
 * function call. We will attempt to keep this architecture 
 * independent, thereby pushing off many of the architecture
 * specific parts of the function call assembly code to the
 * backend.
 */
void generateFunctionCall(astnode *node) {
    if (node->nodetype != FNC_CALL)
        yyerror("Cannot create a funciton call for a non-function call type");

    struct YYnum num_val;
    num_val.val = node->fnc.arg_count;
    num_val.types = NUMMASK_INTGR | NUMMASK_INT;

    emitQuad(ARGBEGIN, NULL, newNode_num(num_val), NULL);


    for (int i = 0 ; i < node->fnc.arg_count ; ++i) {
        num_val.val = i+1;
        astnode *new_tmp = genRvalue(node->fnc.arguments[i]->arg.expr, NULL);
        emitQuad(ARG, NULL, newNode_num(num_val), new_tmp);
    }

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
 */
astnode *genRvalue(astnode *node, astnode *target) {
    if (!target) target = newGenericTemp();

    if (node->nodetype == STABLE_VAR && node->stable_entry.node->nodetype == ARRAY_TYPE) {
        astnode *tmp = newGenericTemp();
        emitQuad(LEA, target, node, NULL);
        return target;
    }
    else if (node->nodetype == STABLE_VAR) {
        emitQuad(MOVQ, target, node, NULL);
        return target;
    }
    else if (node->nodetype == NUM_TYPE) {
        emitQuad(MOVQ, target, node, NULL);
        return target;
    }
    else if (node->nodetype == CHRLIT_TYPE) {
        emitQuad(MOVQ, target, node, NULL);
        return target;
    }
    else if (node->nodetype == IDENT_TYPE) {
        emitQuad(MOVQ, target, node, NULL);
        return target;
    }
    else if (node->nodetype == STRLIT_TYPE) {
        emitQuad(MOVQ, target, node, NULL);
        return target;
    }
    else if (node->nodetype == BINOP_TYPE) {
        // for now ignore type values
        astnode *left = genRvalue(node->binop.left, NULL);
        astnode *right = genRvalue(node->binop.right, NULL);

        enum QuadOpcode op;
        switch (node->binop.op)  {
            case '%':    op = MODQ;     break;
            case '^':    op = XORQ;     break;
            case '&':    op = ANDQ;     break;
            case '*':    op = MULQ;     break;
            case '-':    op = SUBQ;     break;
            case '+':    op = ADDQ;     break;
            case '|':    op = ORQ;      break;
            case '<':    op = LT;       break;
            case '>':    op = GT;       break;
            case '/':    op = DIVQ;     break;
            case SHL:    op = SHL;      break;
            case SHR:    op = SHR;      break;
            case LTEQ:   op = LTEQ;     break;
            case GTEQ:   op = GTEQ;     break;
            case EQEQ:   op = EQEQ;     break;
            case NOTEQ:  op = NOTEQ;    break;
            case LOGOR:  op = LOGOR;    break;
            case LOGAND: op = LOGAND;   break;
            case ',':    op = COMMA;    break;
        }
        
        // if doing addition of anything regarding pointers, need to do poitner arithmetic
        if (node->binop.op == '+' || node->binop.op == '-') {
            struct YYnum tmp_val;
            tmp_val.val = 8;
            tmp_val.types = NUMMASK_INTGR;
            tmp_val.types |= NUMMASK_INT;
            astnode *num_val = newNode_num(tmp_val);
            if (  (node->binop.left->nodetype == STABLE_VAR && (
                    node->binop.left->stable_entry.node->nodetype == PTR_TYPE || 
                    node->binop.left->stable_entry.node->nodetype == ARRAY_TYPE)) && 
                    (node->binop.right->nodetype == NUM_TYPE) ) 
            {
                astnode *tmp = newGenericTemp();
                emitQuad(MULQ, tmp, right, num_val);
                free(right);
                right = tmp;
            }
            else if ( (node->binop.right->nodetype == STABLE_VAR && (
                    node->binop.right->stable_entry.node->nodetype == PTR_TYPE || 
                    node->binop.right->stable_entry.node->nodetype == ARRAY_TYPE) ) && 
                    node->binop.left->nodetype == NUM_TYPE) 
            {
                astnode *tmp = newGenericTemp();
                emitQuad(MULQ, tmp, left, num_val);
                free(left);
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

                    emitQuad(DIVQ, target, tmp_targ, size);
                    return target;
                }              
                else
                    yyerror("Unable to do pointer arithmetic on differing types!");
            }
        }
        emitQuad(op, target, left, right);
        
        return target;
    }
    else if (node->nodetype == UNOP_TYPE) {
        // for now ignore type values

        // convert sizeof operator to an constant value
        astnode *expr = genRvalue(node->unop.expr, NULL);
        enum QuadOpcode op;
        switch (node->unop.op) { 
            case '~':       op = COMPLQ;        break;
            case '-':       op = NEG;           break;
            case '+':       op = POS;           break;
            case '!':       op = LOG_NEG_EXPR;  break;
            case PLUSPLUS:  op = PLUSPLUS;      break;
            case MINUSMINUS:op = MINUSMINUS;    break;
        }
        
        emitQuad(op, target, expr, NULL);
        return target;
    }
    else if (node->nodetype == SIZEOF_TYPE) {
        astnode *expr = evaluateSizeOf(node->unop.expr);
        emitQuad(MOVQ, target, expr, NULL);
        return target;
    }
    else if (node->nodetype == DEREF_TYPE) {
        astnode *left = genRvalue(node->unop.expr, NULL);
        emitQuad(LOAD, target, left, NULL);
        return target;
    }
    else if (node->nodetype == ADDR_TYPE) {
        // convert sizeof operator to an constant value
        astnode *expr = genRvalue(node->unop.expr, NULL);
        emitQuad(LEA, target, expr, NULL);
        return target;
    }
    return NULL;
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
            num_val.val = 8;
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

