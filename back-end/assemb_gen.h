/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * assemb_gen.h - Declares the functions and defines the structs
 * associated with target code generation. 
 */

#include <stdio.h>

#ifndef TARGET_CODE_GEN
#define TARGET_CODE_GEN


/**
 * generateAssemb32 - Generates 32 bit x86 assembly code.
 */
void generateAssemb32(FILE *output_file);


/**
 * generateGlobalVarAssemb - Generates the 32 bit x86 assembly
 * for global variables that appear in the front-end's
 * symbol table.
 */
void generateGlobalVarAssemb();


/**
 * generateFunctionAssemb - Generates the 32 bit x86 assembly
 * for functions - this is done through a simple instruction
 * selector followed by a register allocator.
 * The IR is located in the bb_ll global struct, where each
 * node in the linked list is another defined function in the
 * source program.
 */
void generateFunctionAssemb();


#endif