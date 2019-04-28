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

struct astnode;
struct BasicBlock;
struct Quad;

/* pick the assembly type to convert to - should be compiler parameter. */
//#define TARGET_CODE_64
#define TARGET_CODE_32

/* sizeof operator constants */
#ifdef TARGET_CODE_64
#define DATATYPE_CHAR_SIZE 1
#define DATATYPE_SHORT_SIZE 2
#define DATATYPE_INTEGER_SIZE 4
#define DATATYPE_LONG 8
#define DATATYPE_LONGLONG 8
#define DATATYPE_POINTER 8
#define DATATYPE_FLOAT 4
#define DATATYPE_DOUBLE 8
#define DATATYPE_LONGDOUBLE 16

#define DATATYPE_CHAR_ALIGN 1
#define DATATYPE_SHORT_ALIGN 2
#define DATATYPE_INTEGER_ALIGN 4
#define DATATYPE_LONG_ALIGN 8
#define DATATYPE_LONGLONG_ALIGN 8
#define DATATYPE_POINTER_ALIGN 8
#define DATATYPE_FLOAT_ALIGN 4
#define DATATYPE_DOUBLE_ALIGN 8
#define DATATYPE_LONGDOUBLE_ALIGN 8
#endif

#ifdef TARGET_CODE_32
#define DATATYPE_CHAR_SIZE 1
#define DATATYPE_SHORT_SIZE 2
#define DATATYPE_INTEGER_SIZE 4
#define DATATYPE_LONG 4 
#define DATATYPE_LONGLONG 8
#define DATATYPE_POINTER 4
#define DATATYPE_FLOAT 4
#define DATATYPE_DOUBLE 8
#define DATATYPE_LONGDOUBLE 12

#define DATATYPE_CHAR_ALIGN 1
#define DATATYPE_SHORT_ALIGN 2
#define DATATYPE_INTEGER_ALIGN 4
#define DATATYPE_LONG_ALIGN 4 
#define DATATYPE_LONGLONG_ALIGN 4
#define DATATYPE_POINTER_ALIGN 4
#define DATATYPE_FLOAT_ALIGN 4
#define DATATYPE_DOUBLE_ALIGN 4
#define DATATYPE_LONGDOUBLE_ALIGN 4
#endif


/**
 * generateAssemb32 - Generates 32 bit x86 assembly code.
 */
void generateAssemb32(char *output_file_name);


/**
 * generateGlobalVarAssemb - Generates the 32 bit x86 assembly
 * for global variables that appear in the front-end's
 * symbol table.
 */
void generateGlobalVarAssemb(FILE *output);


/**
 * generateFunctionAssemb - Generates the 32 bit x86 assembly
 * for functions - this is done through a simple instruction
 * selector followed by a register allocator.
 * The IR is located in the bb_ll global struct, where each
 * node in the linked list is another defined function in the
 * source program.
 */
void generateFunctionsAssemb(FILE *output);


/**
 * funcIR2Assemb - Generates assembly for the basic block and
 * all the blocks that flow from it.
 */
void bbIR2Assemb(struct BasicBlock *bb, FILE *output);


/**
 * instructorSelector - Looks at a quad and generates one or
 * more assembly instructions for it.
 */
void instructorSelector(struct Quad quad, FILE *output);


/**
 * node2assemb - Prints out the assembly meaning of an ast node.
 */
char *node2assemb(struct astnode *node);


/**
 * get the alignment size of a variable AST node.
 */
int getAlignment(struct astnode *node);


#endif