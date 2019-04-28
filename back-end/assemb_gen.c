/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * assemb_gen.c - Implements the functions associated with
 * target code generation, ie the functions declared 
 * at quads.h. 
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#include "../front-end/front_end_header.h"
#include "assemb_gen.h"
#include "../front-end/parser/quads.h"
#include "../front-end/lexer/lheader2.h"
#include "./back_end_header.h"


/**
 * generateAssemb32 - Generates 32 bit x86 assembly code.
 */
void generateAssemb32(char *output_file_name) {
    
    // set up assembly file
    FILE *output;
    if (output_file_name) {
        output = fopen(output_file_name, "w+");
        fprintf(output, "        .file   \"%s\"\n", output_file_name);
    }
    else {
        output = stdout;
        fprintf(output, "        .file   \"stdout\"\n");
    }

    /* because we do not have any initialized declarations, there won't be
    a .data section, we can skip right ahead to .text section */
    fprintf(output, "       .text\n");

    // set up the global variables that will go in the .comm (bss) section
    generateGlobalVarAssemb(output);

    // translate the functions defined in the file from IR to assembly
    generateFunctionsAssemb(output);
}


/**
 * generateGlobalVarAssemb - Generates the 32 bit x86 assembly
 * for global variables that appear in the front-end's
 * symbol table.
 */
void generateGlobalVarAssemb(FILE *output) {
    /* iterate through the global scope of the symbol table, adding 
    its variables to the output file in the .comm (bss) section. 
    Note that we will not initialize extern variables. */

    ScopeStackLayer *global_scope = scope_stack.global_scope;

    astnode *cur_node;
    /* iterate over variable and function namespace of global scope */
    for (int i = 0; i < global_scope->tables[GENERAL_NAMESPACE]->size; ++i) {
        
        /* symbol table is implemented as a hash table, so as we go through
        the hashtable, check if this element is filled or not */
        if ((cur_node = global_scope->tables[GENERAL_NAMESPACE]->data[i])) {

            /* in this namespace reside both variables and functions, we want
            only the variables */
            if (cur_node->nodetype == STABLE_VAR) {

                // get the size of the variable
                astnode *size_node = evaluateSizeOf(cur_node);

                // get the alignment of the variable type
                int align_size = getAlignment(cur_node->stable_entry.node);

                // print the bss-ed variables to the assembly file
                fprintf(output, "       .comm   %s,%llu,%d\n",
                                        cur_node->stable_entry.ident, 
                                        size_node->num.val,
                                        align_size);

            }
        }
    }
}


/**
 * generateFunctionAssemb - Generates the 32 bit x86 assembly
 * for functions - this is done through a simple instruction
 * selector followed by a register allocator.
 * The IR is located in the bb_ll global struct, where each
 * node in the linked list is another defined function in the
 * source program.
 */
void generateFunctionsAssemb(FILE *output) {
    
    // go through every function 
    BB_ll_node *cur_node = bb_ll.first;

    while (cur_node) {

        // declare the function variable 
        fprintf(output, "       .globl  %s\n", cur_node->bb->u_label);
        fprintf(output, "       .type   %s, @function\n", cur_node->bb->u_label);
        fprintf(output, "%s:\n", cur_node->bb->u_label);

        // set up the stack and base pointers
        fprintf(output, "       pushl   %%ebp\n");
        fprintf(output, "       movl    %%esp, %%ebp\n");

        /* get the total size of the local variables + the size of the 
        largest amount of function arguments that belong to a function 
        that will be called in this current function. */
        //int local_scope_size = getScopeSize(cur_node->)

        bbIR2Assemb(cur_node->bb, output);
        fprintf(output, "       leave\n");
        fprintf(output, "       .size   %s, .-%s\n", cur_node->bb->u_label, cur_node->bb->u_label);
        cur_node = cur_node->next;
    }
}


/**
 * funcIR2Assemb - Generates assembly for the basic block and
 * all the blocks that flow from it.
 */
void bbIR2Assemb(BasicBlock *bb, FILE *output) {

    // checks whether the block has already been seen and translated
    if (bb->translated)
        return;

    // a instructor selector, with a window size of 1 quad
    QuadLLNode *prev_node = NULL;
    QuadLLNode *cur_node = bb->quads_ll;

    // run through quads
    while (cur_node) {
        instructorSelector(cur_node->quad, output);

        prev_node = cur_node;
        cur_node = cur_node->next;
    }

    /* turn on flag that tells recursive logic that this 
    block has already been seen and translated. */
    bb->translated = true;

    // continue running down the basic block linked-list/ chain!
    if (prev_node->quad.src1 && prev_node->quad.src1->nodetype == BASIC_BLOCK_TYPE) {
        bbIR2Assemb(prev_node->quad.src1->bb_type.bb, output);
    }
    if (prev_node->quad.src2 && prev_node->quad.src2->nodetype == BASIC_BLOCK_TYPE) {
        bbIR2Assemb(prev_node->quad.src2->bb_type.bb, output);
    }
    if (bb->next)
        bbIR2Assemb(bb->next, output);
}


/**
 * instructorSelector - Looks at a quad and generates one or
 * more assembly instructions for it.
 */
void instructorSelector(Quad quad, FILE *output) {


    if (quad.opcode == MOVL) {
        if (quad.src1->nodetype == STABLE_VAR && quad.result->nodetype == STABLE_VAR) {
            astnode *new_reg = getRegister(NULL);
            fprintf(output, "       movl   %s, %s\n", node2assemb(quad.src1), node2assemb(new_reg));  
            fprintf(output, "       movl   %s, %s\n", node2assemb(new_reg), node2assemb(quad.result));
            freeRegister(new_reg);
        }
        else {
            getRegister(quad.result);
            fprintf(output, "       movl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));
        }
    }
    else if (quad.opcode == ADDL) {
        getRegister(quad.result);
        fprintf(output, "       addl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));
        fprintf(output, "       addl    %s, %s\n", node2assemb(quad.src2), node2assemb(quad.result));   
    }
    else if (quad.opcode == SUBL) {
        getRegister(quad.result);
        fprintf(output, "       subl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));
        fprintf(output, "       subl    %s, %s\n", node2assemb(quad.src2), node2assemb(quad.result));   
    }
    else if (quad.opcode == XORL) {
        getRegister(quad.result);
        fprintf(output, "       xorl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.src2));
        fprintf(output, "       movl    %s, %s\n", node2assemb(quad.src2), node2assemb(quad.result));   
    }
    else if (quad.opcode == ANDL) {
        getRegister(quad.result);
        fprintf(output, "       andl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.src2));
        fprintf(output, "       movl    %s, %s\n", node2assemb(quad.src2), node2assemb(quad.result));   
    }
    else if (quad.opcode == MULL) {
        getRegister(quad.result);
        fprintf(output, "       imull   %s, %s\n", node2assemb(quad.src1), node2assemb(quad.src2));
        fprintf(output, "       movl    %s, %s\n", node2assemb(quad.src2), node2assemb(quad.result));   
    }
    else if (quad.opcode == COMPLL) {
        getRegister(quad.result);
        fprintf(output, "       notl    %s\n", node2assemb(quad.src1));
        fprintf(output, "       movl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));   
    }
    else if (quad.opcode == ORL) {

    }
    else if (quad.opcode == DIVL) {

    }
    else if (quad.opcode == MODL) {

    }
    else if (quad.opcode == LT) {

    }
    else if (quad.opcode == GT) {

    }
    else if (quad.opcode == LTEQ_OP) {

    }
    else if (quad.opcode == GTEQ_OP) {

    }
    else if (quad.opcode == EQEQ_OP) {

    }
    else if (quad.opcode == NOTEQ_OP) {

    }

    else if (quad.opcode == LOGO) {

    }
    else if (quad.opcode == LOGN) {

    }
    else if (quad.opcode == COMMA) {

    }
    else if (quad.opcode == DEREF) {

    }
    else if (quad.opcode == PLPL) {

    }
    else if (quad.opcode == MINMIN) {

    }
    else if (quad.opcode == NEG) {

    }
    else if (quad.opcode == POS) {

    }
    else if (quad.opcode == LOG_NEG_EXPR) {

    }
    else if (quad.opcode == STORE) {

    }
    else if (quad.opcode == LOAD) {

    }
    else if (quad.opcode == LEA) {

    }
    else if (quad.opcode == ARGBEGIN) {

    }
    else if (quad.opcode == ARG) {

    }
    else if (quad.opcode == CALL) {

    }
    else if (quad.opcode == CMP) {

    }
    else if (quad.opcode == BR) {

    }
    else if (quad.opcode == BRNEQ) {

    }
    else if (quad.opcode == BREQ) {

    }
    else if (quad.opcode == BRLT) {

    }
    else if (quad.opcode == BRLE) {

    }
    else if (quad.opcode == BRGT) {

    }
    else if (quad.opcode == BRGE) {

    }
    else if (quad.opcode == CC_LT) {

    }
    else if (quad.opcode == CC_GT) {

    }
    else if (quad.opcode == CC_EQ) {

    }
    else if (quad.opcode == CC_NEQ) {

    }
    else if (quad.opcode == CC_GE) {

    }
    else if (quad.opcode == CC_LE) {

    }

    // stuff not worth implementing
    else if (quad.opcode == SHL_OP) {}
    else if (quad.opcode == SHR_OP) {}

    // at every quad, attempt to free the temporary registers
    freeRegister(quad.src1);
    freeRegister(quad.src2);
}


/**
 * node2assemb - Prints out the assembly meaning of an ast node.
 */
char *node2assemb(astnode *node) {
    char *str_val = malloc(sizeof(char)*256);

    if (node->nodetype == NUM_TYPE) {
        if (node->num.types & NUMMASK_INTGR) {
            if (node->num.types & NUMMASK_INT) {
                if (node->num.types & NUMMASK_UNSIGN)
                    sprintf(str_val, "$%d", (int) node->num.val);
                else 
                    sprintf(str_val, "$%u", (unsigned int) node->num.val);            
            }
            else if (node->num.types & NUMMASK_LONG) {
                if (node->num.types & NUMMASK_UNSIGN)
                    sprintf(str_val, "$%lu", (long) node->num.val);
                else 
                    sprintf(str_val, "$%ld", (unsigned long) node->num.val);            
            }
            else { /* long long by default */
                if (node->num.types & NUMMASK_UNSIGN)
                    sprintf(str_val, "$%llu", node->num.val);
                else 
                    sprintf(str_val, "$%lld", (long long ) node->num.val);
            }
        }
        else if (node->num.types & NUMMASK_FLOAT)
            sprintf(str_val, "$%f", (float) node->num.d_val);
        else if (node->num.types & NUMMASK_DOUBLE)
            sprintf(str_val, "$%f", (double) node->num.d_val);
        else
            sprintf(str_val, "$%f", node->num.d_val);
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
 * get the alignment size of a variable AST node.
 */
int getAlignment(astnode *node) {

    if (!node) 
        return -1;

    if (node->nodetype == NUM_TYPE) {
        if (node->num.types & NUMMASK_INT)
            return DATATYPE_INTEGER_ALIGN;
        else if (node->num.types & NUMMASK_LONG)
            return DATATYPE_LONG_ALIGN;
        else if (node->num.types & NUMMASK_LL)
            return DATATYPE_LONGDOUBLE_ALIGN;
        else if (node->num.types & NUMMASK_FLOAT)
            return DATATYPE_FLOAT_ALIGN;
        else if (node->num.types & NUMMASK_DOUBLE)
            return DATATYPE_DOUBLE_ALIGN;
        else if (node->num.types & NUMMASK_LDBLE)
            return DATATYPE_LONGDOUBLE_ALIGN;
        else    /* some error, so just default to regular int align */
            return DATATYPE_INTEGER_ALIGN;
    }   
    else if (node->nodetype == PTR_TYPE || node->nodetype == ARRAY_TYPE)
        return DATATYPE_POINTER_ALIGN;
    else if (node->nodetype == SCALAR_TYPE) {
        switch(node->scalar_type.type) {
            case Int: return DATATYPE_INTEGER_ALIGN;
            case Void: 
                yyerror("Cannot have a void non-pointer variable!"); 
                return -1;
            case Char: return DATATYPE_CHAR_ALIGN;
            case Short: return DATATYPE_SHORT_ALIGN;
            case Long: return DATATYPE_LONG_ALIGN;
            case LongLong: return DATATYPE_LONGDOUBLE_ALIGN;
            case Bool: return DATATYPE_CHAR_ALIGN;
            case Float: return DATATYPE_FLOAT_ALIGN;
            case Double: return DATATYPE_DOUBLE_ALIGN;
            case LongDouble: return DATATYPE_LONGDOUBLE_ALIGN;
            case FloatComplex: return DATATYPE_FLOAT_ALIGN;
            case DoubleComplex: return DATATYPE_DOUBLE_ALIGN;
            case LongDoubleComplex: return DATATYPE_LONGDOUBLE_ALIGN;
            case FloatImag: return DATATYPE_FLOAT_ALIGN;
            case DoubleImag: return DATATYPE_DOUBLE_ALIGN;
            case LongDoubleImag: return DATATYPE_LONGDOUBLE_ALIGN;
        }
    }
    return -1;
}