/*
 * Compiler Main Header File
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * compiler-header.h - A header file for the overall
 * compiler.
 */

#include <stdbool.h>


#include "../front-end/front_end_header.h"


#ifndef BACKEND_HEADER
#define BACKEND_HEADER

struct astnode;


/******* REGISTER ALLOCATION ********/
/* The key behind this register allocation strategy is that each
temporary regsiter created in the quads will be given a value once
and will be used once. Therefore in the instructor selector we will
free each register quad source. */ 

/* a struct holding the names of all regsiters, how many there
are, and if they are available or not. */
typedef struct registerCollection {
    int size;
    char **names;
    _Bool *available;
} registerCollection;


/* initialize the global registerCollection struct */
void initializeRegisterCollection();


/**
 * getRegister - Gets a free register from the collection, changing
 * its status in the registerCollection global struct.
 */
struct astnode *getRegister(struct astnode *node);


/**
 * freeReigster - Frees up a register from the register Collection
 * struct that was previously taken.
 */
void freeRegister(struct astnode *name);


EXTERN_VAR registerCollection reg_collector;    /* global register collection */



#endif