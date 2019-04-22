/*
 * Front-End Main Header File
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * front_end_header.h - A header file for the front-end
 * of the compiler (lexer and parser). 
 * 
 * This file includes most of the global variables for 
 * the front-end, as well as some of the macros and 
 * structs that it will use. 
 */


#ifndef FRONT_END
#define FRONT_END


/* Defines a str and num structs that will appear in YYSTYPE (yylval) */
struct YYnum {
    unsigned long long val;
    long double d_val;
    int types;  /* masks defined in header */
};
struct YYstr {
    char *str;
    int str_size;
    char char_val;
};

enum PrintLevel { Minimal_Level = 1, Mid_Level, Verbose_Level };

#define LINESIZE 1024
#define MAX_STRLIT_SIZE 2048


#ifndef EXTERN_VAR
#define EXTERN_VAR extern
#endif

EXTERN_VAR int cur_line_num;		            /* current line number	            */
EXTERN_VAR int error_count;                     /* Counts errors - stops after 10   */
EXTERN_VAR char cur_file_name[LINESIZE+1];		/* current file name                */
EXTERN_VAR char tmp[20];						/* temp helper variable             */
EXTERN_VAR char strlit_buffer[MAX_STRLIT_SIZE]; /* buffer for string literals       */
EXTERN_VAR char *helper_end, *helper_begin; 
EXTERN_VAR int yycolumn;                        /* implements locations for the lexer */
EXTERN_VAR enum PrintLevel print_level;         /* the level on which to print      */
EXTERN_VAR int generic_node_count;              /* a counter of the value of next IR generic node */
EXTERN_VAR struct QuadLLNode *cur_quad_ll;      /* the current quad linked list to append to      */
EXTERN_VAR struct BasicBlock *cur_basic_block;  /* the current basic block to append to           */



#include "./parser/symbol_table.h"
EXTERN_VAR ScopeStack scope_stack;              /* scope linked list   */

#include "./lexer/lheader.h"
EXTERN_VAR YYSTYPE yylval;                      /* The infamous yylval */

/*
 * initializeFrontEnd - A function for initializing
 * some of the global variables of the front-end (defined
 * above). Could be thought of the as the constructor for
 * the front-end.
 */
void initializeFrontEnd();


#endif