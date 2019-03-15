/*
 * Front-End Main Header's Implementation File
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * front_end_header.h - Implements the functions
 * defined in front_end_header.h. 
 */


#include "front_end_header.h"

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

    /* scope stack initialization */
    createNewScope(File);
}
