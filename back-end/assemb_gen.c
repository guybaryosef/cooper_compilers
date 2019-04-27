/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * assemb_gen.c - Implements the functions associated with
 * target code generation, ie the functions declared 
 * at quads.h. 
 */


#include <stdio.h>

#include "assemb_gen.h"

/**
 * generateAssemb32 - Generates 32 bit x86 assembly code.
 */
void generateAssemb32(FILE *output_file) {
    FILE *output = output_file ? output_file : stdout;

    generateGlobalVarAssemb();
}


/**
 * generateGlobalVarAssemb - Generates the 32 bit x86 assembly
 * for global variables that appear in the front-end's
 * symbol table.
 */
void generateGlobalVarAssemb() {

}