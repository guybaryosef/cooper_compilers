/*
 * Compiler Main Header File
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * back_end_header.c - Implements the functions declared in 
 * the header file 'back_end_header.h'.
 */


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "back_end_header.h"
#include "../front-end/front_end_header.h"
#include "../front-end/parser/pheader_ast.h"


/* initialize the global registerCollection struct */
void initializeRegisterCollection() {
    reg_collector.size = 6;
    reg_collector.names = malloc(sizeof(char *)*reg_collector.size);
    reg_collector.available = malloc(sizeof(_Bool)*reg_collector.size);

    reg_collector.names[0] = "%eax";
    reg_collector.names[1] = "%ebx";
    reg_collector.names[2] = "%ecx";
    reg_collector.names[3] = "%edx";
    reg_collector.names[4] = "%edi";
    reg_collector.names[5] = "%esi";
    // reg_collector.names[6] = "es";  /* only a 16 bit register */

    for (int i = 0; i < reg_collector.size; ++i)
        reg_collector.available[i] = true;
}

/**
 * getRegister - Gets a free register from the collection, changing
 * its status in the registerCollection global struct.
 */
astnode *getRegister(astnode *node) {
    struct YYstr str_val;
     
    for (int i = 0; i < reg_collector.size; ++i) {
        if (reg_collector.available[i] == true) {
            reg_collector.available[i] = false;
            
            if (node && node->nodetype == IDENT_TYPE) {
                node->ident.str = reg_collector.names[i];
                return node;
            }
            else {
                return newNode_str(IDENT, str_val);
            }
        }
    }
}


/**
 * freeReigster - Frees up a register from the register Collection
 * struct that was previously taken.
 */
void freeRegister(astnode *node) {
    if (!node || node->nodetype != IDENT_TYPE)
        return;

    char *name = node->ident.str;
    if (!strcmp(name, "eax"))
        reg_collector.available[0] = true;
    else if (!strcmp(name, "ebx"))
        reg_collector.available[1] = true;
    else if (!strcmp(name, "ecx"))
        reg_collector.available[2] = true;
    else if (!strcmp(name, "edx"))
        reg_collector.available[3] = true;        
    else if (!strcmp(name, "edi"))
        reg_collector.available[4] = true;
    else if (!strcmp(name, "esi"))
        reg_collector.available[5] = true;
    // else if (!strcmp(name, "es"))
    //     reg_collector.available[6] = true;   
}