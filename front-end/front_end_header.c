/*
 * Front-End Main Header's Implementation File
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * front_end_header.h - Implements the functions
 * defined in front_end_header.h. 
 */


#include "front_end_header.h"
#include "./parser/quads.h"

#include <stdlib.h>


/*
 * initializeFrontEnd - A function for initializing
 * some of the global variables of the front-end (defined
 * above). Could be thought of the as the constructor for
 * the front-end.
 */
void initializeFrontEnd() {
    /* error checking variables */
    cur_line_num = 1;
	error_count = 0;
    yycolumn = 1;

    /* IR generic code initialization */
    cur_basic_block = NULL;
    initial_bb = NULL;
    cur_quad_ll = NULL;
    generic_node_count = 1; 
    generic_bb_count = 1;
    continue_bb = NULL;
    break_bb = NULL;
    
    /* scope stack initialization */
    createNewScope(File);
}
