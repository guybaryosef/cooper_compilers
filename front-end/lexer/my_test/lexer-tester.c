/*
 * ECE466: Compilers
 * By: Guy Bar Yosef
 * 
 * A test program for the lexical analyzer.
 * This program will run the lexical analyzer in the parent directory 
 * on whatever input is fed into it through stdin.
 * 
 */

#include <string.h>




/* Defines the global variables here. All other header files
   will see these variables as extern vars.  */
#define EXTERN_VAR

#include "../../front_end_header.h"
#include "../lexer.c"

/* We are testing only the lexer, and do not want to implement the 
   locations feature for the parser (will lead to and error, because 
   it requires variables that the parser initializes).
   As such, we define YYLTYPE which is supposed to be defined in the 
   parser and which yyerror uses. Here we will not be using it,
   just implementing it to keep the compiler happy. 	*/
YYLTYPE yylloc = { 1,1,1,1};


int main(){

	/* Need to initialize error-counting variables */
    cur_line_num = 1;
	error_count = 0;
    yycolumn = 1;

	memset(&yylval, 0, sizeof(yylval));  /* reset the token semantic value */      

	int token_code;

	/* table format to compare output to hakner's lexer */
	while (token_code = yylex()) {
		if (token_code == NUMBER) {

			printf("%s\t%d\t%s\t", cur_file_name, cur_line_num, 
									stringFromTokens(token_code));

			/* print number type */
			if (yylval.num.types & NUMMASK_INTGR) {
				printf("%s\t%llu\t", "INTEGER", yylval.num.val);
				if (yylval.num.types & NUMMASK_UNSIGN) 
					printf("%s,", "UNSIGNED");
				
				if (yylval.num.types & NUMMASK_INT)
					printf("%s\n", "INT");
				else if (yylval.num.types & NUMMASK_LONG)
					printf("%s\n", "LONG");
				else 
					printf("%s\n", "LONGLONG");
			}
			else { 
				printf("%s\t", "REAL");

				/* format number output like hakner's */
				if ((int)yylval.num.d_val == yylval.num.d_val && yylval.num.d_val < 99)
						printf("%d\t", (int)yylval.num.d_val);
				else if (yylval.num.d_val >=0.01 && yylval.num.d_val <= 9.9)
					printf("%1.1LF\t", yylval.num.d_val);
				else
					printf("%g\t", (double)yylval.num.d_val);

				if (yylval.num.types & NUMMASK_FLOAT)
					printf("%s\n", "FLOAT");
				else if (yylval.num.types & NUMMASK_LDBLE)
					printf("%s\n", "LONGDOUBLE");
				else
					printf("%s\n", "DOUBLE");
			}
		}
		else if (token_code == CHARLIT) {
			printf("%s\t%d\t%s\t", cur_file_name, cur_line_num, 
										stringFromTokens(token_code));

			if (yylval.str.char_val > 31 && yylval.str.char_val < 127 &&
				yylval.str.char_val != 92 && yylval.str.char_val != 39 &&
				yylval.str.char_val != 34) {
				printf("%c", yylval.str.char_val);
			}
			else {
				switch(yylval.str.char_val) {
					case  92: printf("\\\\"); break;
					case   0: printf("\\0"); break;
					case  10: printf("\\n"); break;
					case   7: printf("\\a"); break;
					case   8: printf("\\b"); break;
					case   9: printf("\\t"); break;
					case  13: printf("\\r"); break;
					case  12: printf("\\f"); break;
					case  11: printf("\\v"); break;
					case  39: printf("\\'"); break;
					case  34: printf("\"");  break;	
					default: printf("\\%03o", yylval.str.char_val);
				}
			}
			printf("\n");
		}
		else if (token_code == STRING) {
			printf("%s\t%d\t%s\t", cur_file_name, cur_line_num, 
									stringFromTokens(token_code));
			for (int i = 0 ; i < yylval.str.str_size; ++i) {
			
				if (yylval.str.str[i] > 31 && yylval.str.str[i] < 127 &&
					yylval.str.str[i] != 92 && yylval.str.str[i] != 39 &&
					yylval.str.str[i] != 34) {
					printf("%c", yylval.str.str[i]);
				}
				else {
					switch(yylval.str.str[i]) {
						case  92: printf("\\\\");break;
						case   0: printf("\\0"); break;
						case  10: printf("\\n"); break;
						case   7: printf("\\a"); break;
						case   8: printf("\\b"); break;
						case   9: printf("\\t"); break;
						case  13: printf("\\r"); break;
						case  12: printf("\\f"); break;
						case  11: printf("\\v"); break;
						case  39: printf("\\'"); break;
						case  34: printf("\\\"");break;		
						default: 
							if (yylval.str.str[i] < 255)
								printf("\\%03o", (unsigned char)yylval.str.str[i]);
							else
								printf("\\%03o", (unsigned char)255);
					}
				}
			}
			printf("\n");
		}
		else if (token_code == IDENT) {
			printf("%s\t%d\t%s\t%s\n", cur_file_name, cur_line_num, 
							stringFromTokens(token_code), yylval.str.str);
		}
		else /* token is a keyword */ {
			if (token_code < 256)
				printf("%s\t%d\t%c\n", cur_file_name, cur_line_num, token_code);
			else 
				printf("%s\t%d\t%s\n", cur_file_name, cur_line_num, 
											stringFromTokens(token_code));

		}

		if (token_code == STRING)
			free(yylval.str.str);

		memset(&yylval, 0, sizeof(yylval));  /* reset the token semantic value */      
	}

	return 0; 
}