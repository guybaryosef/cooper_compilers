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
#include <string.h>


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

    // set up the temporary files that will make up the assembly file
    FILE *strlit_output = fopen("stringlit_assemb_guycc.s", "w+");
    FILE *body_output = fopen("body_assemb_guycc.s", "w+");

    /* because we do not have any initialized declarations, there won't be
    a .data section, we can skip right ahead to .text section */
    fprintf(body_output, "        .text\n");

    // set up the global variables that will go in the .comm (bss) section
    generateGlobalVarAssemb(body_output);

    // translate the functions defined in the file from IR to assembly
    generateFunctionsAssemb(body_output, strlit_output);

    fseek(strlit_output, 0, SEEK_SET);
    fseek(body_output, 0, SEEK_SET);
    
    char c;
    // dump strlit_output to the output file
    while ((c = fgetc(strlit_output)) != EOF) 
        fputc(c, output);

    // dump body_output to the output file
    while ((c = fgetc(body_output)) != EOF) 
        fputc(c, output);

    // delete the two temporary files
    remove("stringlit_assemb_guycc.s");
    remove("body_assemb_guycc.s");
}


/**
 * generateGlobalVarAssemb - Generates the 32 bit x86 assembly
 * for global variables that appear in the front-end's
 * symbol table.
 */
void generateGlobalVarAssemb(FILE *body_output) {
    /* iterate through the global scope of the symbol table, adding 
    its variables to the body_output file in the .comm (bss) section. 
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
                fprintf(body_output, "        .comm   %s,%llu,%d\n",
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
void generateFunctionsAssemb(FILE *body_output, FILE *strlit_output) {
    // go through every function 
    BB_ll_node *cur_node = bb_ll.first;
    while (cur_node) {

        // declare the function variable 
        fprintf(body_output, "        .globl  %s\n", cur_node->bb->u_label);
        fprintf(body_output, "        .type   %s, @function\n", cur_node->bb->u_label);
        fprintf(body_output, "%s:\n", cur_node->bb->u_label);
        fprintf(body_output, "        pushl   %%ebp\n");
        fprintf(body_output, "        movl    %%esp, %%ebp\n");

        /* get the total size of the local variables */
        int fnc_scope_size = evaluateLocalVars(cur_node->bb->u_label);
        fprintf(body_output, "        subl    $%d, %%esp\n", fnc_scope_size);

        Quad *last_quad = bbIR2Assemb(cur_node->bb, body_output, strlit_output, false, true);

        if (!last_quad || (last_quad->opcode != RETURN)) {
            fprintf(body_output, "        movl    $0, %%eax\n");
            fprintf(body_output, "        leave\n");
            fprintf(body_output, "        ret\n");
        }
        fprintf(body_output, "        .size   %s, .-%s\n", cur_node->bb->u_label, cur_node->bb->u_label);
        cur_node = cur_node->next;
    }


}


/**
 * evaluateLocalVars - Evaluates how much space all the local variables
 * of a function will take. This is done by iterating through the 
 * function's symbol table and summing the sizeof each local variable.
 */
long int evaluateLocalVars(char *fnc_name) {
    long int total_sum = 0;

    astnode *fnc_node = searchStackScope(GENERAL_NAMESPACE, fnc_name);
    if (!fnc_node) {
        fprintf(stderr, "Internal compiler error: finding function in global scope.\n");
        return -1;
    }

    SymbolTable **tables = fnc_node->stable_entry.fnc.function_body->compound_stmt.scope_layer->tables;
    astnode *tmp;
    for (int i = 0 ; i < 3; ++i) {
        for (int j=0; j<tables[i]->size; ++j) {
            if (tables[i]->data[j]) {
                // evaluate the size of the current local variable
                tmp = evaluateSizeOf(tables[i]->data[j]);

                // update the total local storage size
                total_sum += tmp->num.val;

                // set the offset for the current local variable
                tables[i]->data[j]->stable_entry.var.offset_within_stack_frame = -total_sum;
            }
        }
    }

    return total_sum;
}


/**
 * funcIR2Assemb - Generates assembly for the basic block and
 * all the blocks that flow from it.
 */
Quad *bbIR2Assemb(BasicBlock *bb, FILE *body_output, FILE *strlit_output, _Bool in_conditional_arm, _Bool is_fnc) {

    // checks whether the block has already been seen and translated
    if (!bb || bb->translated)
        return NULL;

    if (!is_fnc)
        fprintf(body_output, "%s:\n", bb->u_label);

    // a instructor selector, with a window size of 1 quad
    QuadLLNode *last_node = NULL;
    QuadLLNode *cur_node = bb->quads_ll;

    // run through quads
    while (cur_node) {
        instructorSelector(cur_node->quad, body_output, strlit_output);

        last_node = cur_node;
        cur_node = cur_node->next;
    }
    Quad *last_quad = &last_node->quad;

    /* turn on flag that tells recursive logic that this 
    block has already been seen and translated. */
    bb->translated = true;

    // continue running down the basic block linked-list/ chain!
    if (last_quad && (
        last_quad->opcode == BRLE || last_quad->opcode == BRGE ||
        last_quad->opcode == BRLT || last_quad->opcode == BRGT ||
        last_quad->opcode == BRNEQ || last_quad->opcode == BREQ) ) {

        Quad *new_last_quad1 = bbIR2Assemb(last_quad->src1->bb_type.bb, body_output, strlit_output, true, false);
        if (new_last_quad1 && new_last_quad1->opcode == BR && 
        new_last_quad1->src1->bb_type.bb == last_quad->src2->bb_type.bb) {
            if (in_conditional_arm)
                return new_last_quad1;
            else {
                return bbIR2Assemb(last_quad->src2->bb_type.bb, body_output, strlit_output, false, false);
            }
        }
        else {
            Quad *new_last_quad2 = bbIR2Assemb(last_quad->src2->bb_type.bb, body_output, strlit_output, true, false);
            if (new_last_quad2 && new_last_quad2->opcode == BR) {
                if (in_conditional_arm) {
                    return new_last_quad2;
                }
                else {
                    return bbIR2Assemb(new_last_quad2->src1->bb_type.bb,body_output, strlit_output, false, false);
                }
            }
        }
    }
    else if (in_conditional_arm && bb->next) {
        return bbIR2Assemb(bb->next, body_output, strlit_output, true, false);
    }
    else if(!in_conditional_arm && bb->next) {
        return bbIR2Assemb(bb->next, body_output, strlit_output, false, false);
    }
    else if (in_conditional_arm && last_quad && last_quad->opcode == BR) {
        return last_quad;
    }
    else if (!in_conditional_arm && last_quad && last_quad->opcode == BR) {
        return bbIR2Assemb(last_quad->src1->bb_type.bb, body_output, strlit_output, false, false);
    }
    return last_quad;
}


/**
 * instructorSelector - Looks at a quad and generates one or
 * more assembly instructions for it.
 */
void instructorSelector(Quad quad, FILE *body_output, FILE *strlit_output) {
    static int func_arg_count = 0;

    /* check if one of the sources is a string literal, if so add it to the string literal
    output that will get concatinated with the whole file, later */
    if (quad.src1 && quad.src1->nodetype == STRLIT_TYPE) {
        quad.src1->strlit.memlbl = getStrlitName();
        fprintf(strlit_output, "%s:\n", quad.src1->strlit.memlbl);
        fprintf(strlit_output, "        .string \"%s\"\n", quad.src1->strlit.str);

        // add the quad getting the string literal from the other-place-in-memory
        astnode *tmp = getRegister(NULL);
        fprintf(body_output, "        leal    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));
        quad.src1->nodetype = REG_TYPE;
        quad.src1->reg_type.name = tmp->reg_type.name;
    }
    if (quad.src2 && quad.src2->nodetype == STRLIT_TYPE) {
        quad.src2->strlit.memlbl = getStrlitName();
        fprintf(strlit_output, "%s:\n", quad.src2->strlit.memlbl);
        fprintf(strlit_output, "        .string \"%s\"\n", quad.src1->strlit.str);


        // add the quad getting the string literal from the other-place-in-memory
        astnode *tmp = getRegister(NULL);
        fprintf(body_output, "      leal    %s, %s\n", node2assemb(quad.src2), node2assemb(tmp));
        quad.src2->nodetype = REG_TYPE;
        quad.src2->reg_type.name = tmp->reg_type.name;
    }


    if (quad.result)
        getRegister(quad.result);
    
    if (quad.opcode == MOVL) {
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));
    }
    else if (quad.opcode == ADDL) {
        astnode *tmp = getRegister(NULL);
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));
        fprintf(body_output, "        addl    %s, %s\n", node2assemb(quad.src2), node2assemb(tmp));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(tmp), node2assemb(quad.result));
        freeRegister(tmp);   
    }
    else if (quad.opcode == SUBL) {
        astnode *tmp = getRegister(NULL);
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src2), node2assemb(tmp));        
        fprintf(body_output, "        subl    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(tmp), node2assemb(quad.result));   
        freeRegister(tmp);
    }
    else if (quad.opcode == XORL) {
        astnode *tmp = getRegister(NULL);
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));        
        fprintf(body_output, "        xorl    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(tmp), node2assemb(quad.result));   
        freeRegister(tmp);
    }
    else if (quad.opcode == ANDL) {
        astnode *tmp = getRegister(NULL);
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));        
        fprintf(body_output, "        andl    %s, %s\n", node2assemb(quad.src2), node2assemb(tmp));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(tmp), node2assemb(quad.result));   
        freeRegister(tmp);
    }
    else if (quad.opcode == MULL) {
        astnode *tmp = getRegister(NULL);
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));        
        fprintf(body_output, "        imull   %s, %s\n", node2assemb(quad.src2), node2assemb(tmp));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(tmp), node2assemb(quad.result));   
        freeRegister(tmp);
    }
    else if (quad.opcode == COMPLL) {
        fprintf(body_output, "        notl    %s\n", node2assemb(quad.src1));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));   
    }
    else if (quad.opcode == ORL) {
        astnode *tmp = getRegister(NULL);
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));                
        fprintf(body_output, "        orl     %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(tmp), node2assemb(quad.result));   
    }
    else if (quad.opcode == RETURN) {
        if (quad.src1) {
            fprintf(body_output, "        movl    %s, %%eax\n", node2assemb(quad.src1));
            fprintf(body_output, "        leave\n");
            fprintf(body_output, "        ret\n");
        }
    }
    else if (quad.opcode == DIVL) {
        // move src1 into eax
        fprintf(body_output, "        pushl   %%eax\n");
        fprintf(body_output, "        movl %s, %%eax\n", node2assemb(quad.src1));

        // make sure edx is all 0s
        fprintf(body_output, "        pushl %%edx\n");
        fprintf(body_output, "        xor %%edx, %%edx\n");

        // do the godforsaken divide operation already
        fprintf(body_output, "        idiv    %s\n", node2assemb(quad.src2));
        fprintf(body_output, "        movl    %%eax, %s\n", node2assemb(quad.result));
    
        // return whatever was pushed back into eax and edx
        fprintf(body_output, "        movl    %%esp, %%edx\n");
        fprintf(body_output, "        addl $4, %%esp\n");
        fprintf(body_output, "        movl    %%esp, %%eax\n");
        fprintf(body_output, "        addl $4, %%esp\n");
    }
    else if (quad.opcode == MODL) {
        // move src1 into eax
        fprintf(body_output, "        pushl   %%eax\n");
        fprintf(body_output, "        movl %s, %%eax\n", node2assemb(quad.src1));

        // make sure edx is all 0s
        fprintf(body_output, "        pushl %%edx\n");
        fprintf(body_output, "        xor %%edx, %%edx\n");

        // do the godforsaken divide operation already
        fprintf(body_output, "        idiv    %s\n", node2assemb(quad.src2));
        fprintf(body_output, "        movl    %%edx, %s\n", node2assemb(quad.result));
    
        // return whatever was pushed back into eax and edx
        fprintf(body_output, "        movl    %%esp, %%edx\n");
        fprintf(body_output, "        addl    $4, %%esp\n");
        fprintf(body_output, "        movl    %%esp, %%eax\n");
        fprintf(body_output, "        addl    $4, %%esp\n");
    }
    else if (quad.opcode == NEG) {
        fprintf(body_output, "        negl    %s\n", node2assemb(quad.src1));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));
    }
    else if (quad.opcode == LOG_NEG_EXPR) {
        fprintf(body_output, "        testl   %s, %s\n", node2assemb(quad.src1), node2assemb(quad.src1));
        
        if (!strcmp(quad.src1->ident.str, "%eax")) {
            fprintf(body_output, "        sete    %%al\n");
            fprintf(body_output, "        movzbl  %%al, %s\n", node2assemb(quad.src1));
        }
        else if (!strcmp(quad.src1->ident.str, "%ebx")) {
            fprintf(body_output, "        sete    %%bl\n");
            fprintf(body_output, "        movzbl  %%bl, %s\n", node2assemb(quad.src1));
        }
        else if (!strcmp(quad.src1->ident.str, "%ecx")) {
            fprintf(body_output, "        sete    %%cl\n");
            fprintf(body_output, "        movzbl  %%cl, %s\n", node2assemb(quad.src1));
        }
        else if (!strcmp(quad.src1->ident.str, "%edx")) {
            fprintf(body_output, "        sete    %%dl\n");
            fprintf(body_output, "        movzbl  %%dl, %s\n", node2assemb(quad.src1));
        }
        else if (!strcmp(quad.src1->ident.str, "%edi")) {
            fprintf(body_output, "        sete    %%dil\n");
            fprintf(body_output, "        movzbl  %%dil, %s\n", node2assemb(quad.src1));
        }
        else if (!strcmp(quad.src1->ident.str, "%esi")) {
            fprintf(body_output, "        sete    %%sil\n");
            fprintf(body_output, "        movzbl  %%sil, %s\n", node2assemb(quad.src1));
        }

        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));
    }
    else if (quad.opcode == STORE) {
        fprintf(body_output, "        movl    %s, (%s)\n", node2assemb(quad.src1), node2assemb(quad.src2));
    }
    else if (quad.opcode == LOAD) {
        fprintf(body_output, "        movl    (%s), %s\n", node2assemb(quad.src1), node2assemb(quad.src1));
        fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));
    }
    else if (quad.opcode == LEA) {
        if (quad.result->nodetype == STABLE_VAR) {
            astnode *tmp = getRegister(NULL);
            fprintf(body_output, "        leal    %s, %s\n", node2assemb(quad.src1), node2assemb(tmp));
            fprintf(body_output, "        movl    %s, %s\n", node2assemb(tmp), node2assemb(quad.result));
        }
        else
            fprintf(body_output, "        leal    %s, %s\n", node2assemb(quad.src1), node2assemb(quad.result));
    }
    else if (quad.opcode == ARG) {
        fprintf(body_output, "        pushl   %s\n", node2assemb(quad.src2));
        func_arg_count += 1;
    }
    else if (quad.opcode == CALL) {
        fprintf(body_output, "        call    %s\n", node2assemb(quad.src1));

        // shift the stack pointer back to place before the function arguments
        fprintf(body_output, "        addl    $%d, %%esp\n", func_arg_count*4);
        func_arg_count = 0;

        if (quad.result) {
            fprintf(body_output, "        movl    %%eax, %s\n", node2assemb(quad.result));
        }
    }
    else if (quad.opcode == CMP) {
        astnode *new_reg1, *new_reg2;
        if (quad.src1->nodetype == REG_TYPE) {
            new_reg1 = quad.src1;
        }
        else {
            astnode *new_reg1 = getRegister(NULL);
            fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src1), node2assemb(new_reg1));
            freeRegister(quad.src1);
        }

        if (quad.src2->nodetype == REG_TYPE) {
            new_reg2 = quad.src2;
        }
        else {
            astnode *new_reg2 = getRegister(NULL);
            fprintf(body_output, "        movl    %s, %s\n", node2assemb(quad.src2), node2assemb(new_reg2));
            freeRegister(quad.src2);
        }

        fprintf(body_output, "        cmpl    %s, %s\n", node2assemb(new_reg2), node2assemb(new_reg1));
        freeRegister(new_reg1);
        freeRegister(new_reg2);
    }
    else if (quad.opcode == BR) {
        fprintf(body_output, "        jmp     %s\n", node2assemb(quad.src1));
    }
    else if (quad.opcode == BRNEQ) {
        fprintf(body_output, "        je      %s\n", node2assemb(quad.src2));
    }
    else if (quad.opcode == BREQ) {
        fprintf(body_output, "        jne     %s\n", node2assemb(quad.src2));
    }
    else if (quad.opcode == BRLT) {
        fprintf(body_output, "        jge     %s\n", node2assemb(quad.src2));
    }
    else if (quad.opcode == BRLE) {
        fprintf(body_output, "        jg     %s\n", node2assemb(quad.src2));
    }
    else if (quad.opcode == BRGT) {
        fprintf(body_output, "        jle     %s\n", node2assemb(quad.src2));
    }
    else if (quad.opcode == BRGE) {
        fprintf(body_output, "        jl     %s\n", node2assemb(quad.src2));
    }
    else if (quad.opcode == CC_LT || quad.opcode == CC_GT ||
            quad.opcode == CC_EQ || quad.opcode == CC_NEQ ||
            quad.opcode == CC_GE || quad.opcode == CC_LE) {
 
        char set_op[10];
        switch(quad.opcode) {
            case CC_LT: strcpy(set_op, "setl"); break;
            case CC_GT: strcpy(set_op, "setg"); break;
            case CC_EQ: strcpy(set_op, "sete"); break;
            case CC_NEQ: strcpy(set_op, "setne"); break;
            case CC_GE: strcpy(set_op, "setge"); break;
            case CC_LE: strcpy(set_op, "setle"); break;
        }

        astnode *new_reg1 = getRegister(NULL);
        if (!strcmp(new_reg1->ident.str, "%eax")) {
            fprintf(body_output, "        %s      %%al\n", set_op);
            fprintf(body_output, "        movzbl  %%al, %s\n", node2assemb(quad.result));
        }
        else if (!strcmp(new_reg1->ident.str, "%ebx")) {
            fprintf(body_output, "        %s      %%bl\n", set_op);
            fprintf(body_output, "        movzbl  %%bl, %s\n", node2assemb(quad.result));
        }
        else if (!strcmp(new_reg1->ident.str, "%ecx")) {
            fprintf(body_output, "        %s      %%cl\n", set_op);
            fprintf(body_output, "        movzbl  %%cl, %s\n", node2assemb(quad.result));
        }
        else if (!strcmp(new_reg1->ident.str, "%edx")) {
            fprintf(body_output, "        %s      %%dl\n", set_op);
            fprintf(body_output, "        movzbl  %%dl, %s\n", node2assemb(quad.result));
        }
        else if (!strcmp(new_reg1->ident.str, "%edi")) {
            fprintf(body_output, "        %s      %%dil\n", set_op);
            fprintf(body_output, "        movzbl  %%dil, %s\n", node2assemb(quad.result));
        }
        else if (!strcmp(new_reg1->ident.str, "%esi")) {
            fprintf(body_output, "        %s      %%sil\n", set_op);
            fprintf(body_output, "        movzbl  %%sil, %s\n", node2assemb(quad.result));
        }
        freeRegister(new_reg1);
    }

    // stuff not worth implementing
    else if (quad.opcode == ARGBEGIN) {}
    else if (quad.opcode == PLPL)   {}
    else if (quad.opcode == MINMIN) {}
    else if (quad.opcode == LOGO)   {}
    else if (quad.opcode == LOGN)   {}
    else if (quad.opcode == COMMA)  {}
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

    if (node->nodetype == REG_TYPE) {
        sprintf(str_val, "%s", node->reg_type.name);
    }
    else if (node->nodetype == BASIC_BLOCK_TYPE) {
        sprintf(str_val, "%s", node->bb_type.bb->u_label);
    }
    else if (node->nodetype == STRLIT_TYPE) {
        sprintf(str_val, "%s", node->strlit.memlbl);
    }
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
    else if (node->nodetype == STABLE_VAR) {
        if (node->stable_entry.var.storage_class == Extern) {
            sprintf(str_val, "%s", node->stable_entry.ident);
        }
        else {  /* it is a local variables */
            sprintf(str_val, "%d(%%ebp)", node->stable_entry.var.offset_within_stack_frame);
        }
    }
    else if (node->nodetype == STABLE_IDENT_TYPE) {
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


/**
 * getStrlitName - Returns the label of the area in memory in which the string
 * literal will reside.
 */
char *getStrlitName() {
    static int val = 0;
    char *str_val = malloc(sizeof(char)*8);
    sprintf(str_val, ".LC%d", val);
    ++val;

    return str_val; 
}