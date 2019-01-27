
/* Prologue: Options and definitions */

/* flex doesn't call yywrap after end of file */
%option noyywrap 

/* no default rule for input */
%option nodefault

/* no default line counter, we do it ourselves */
%option noyylineno

/* setting up start states */
%x COMMENT
%x CHR_LIT
%x STR_LIT

/* repeating patterns */
HEX		[0-9a-fA-F]
DEC 	[0-9]
OCT 	[0-7]
NTYPE 	((l|L|ll|LL)?[uU]?)|([uU]?(l|L|ll|LL)?)

/* vars which can be accessed inside yylex() and main() */
%{ 
	#include "./lexer_header.h"

	int cur_line_num = 1;            /* current line number  */
	char cur_file_name[LINESIZE+1];  /* current file name    */
	char tmp[20];                    /* temp helper variable */
	char strlit_buffer[4096];		 /* buffer for string literals */
	/* helper pointers for exracting the line number and file name */
	char *helper_end, *helper_begin;    

	/* struct and variable for the token semantic values */
	typedef union {
		struct num num; /* defined in header */
		struct str {
			char *str;
			int str_size;
			char char_val;
		} str;
	} YYSTYPE;

	YYSTYPE yylval;
%} 

%% 
	/* Body: (regexp) : (embedded action) pairs */

	/* skip comments */
"//".*\n			{ ++cur_line_num; }
"/*"   		    	{ BEGIN COMMENT;  }
<COMMENT>\n	    	{ ++cur_line_num; }
<COMMENT>[^\n"*/]*	{}
<COMMENT>"*/" 		{ BEGIN INITIAL;  }


	/* skip white spaces  */
[ \t\r]     {}
\n			{ ++cur_line_num; }


	/* tokens: keywords */
auto        { return AUTO;     }
break       { return BREAK;    }
case        { return CASE;     }
char        { return CHAR;     }
const       { return CONST;    }
continue    { return CONTINUE; }
default     { return DEFAULT;  }
do          { return DO;       }
double      { return DOUBLE;   }
else        { return ELSE;     }
enum        { return ENUM;     }
extern      { return EXTERN;   }
float       { return FLOAT;    }
for         { return FOR;      }
goto        { return GOTO;     }
if          { return IF;       }
inline      { return INLINE;   }
int         { return INT;      }
long        { return LONG;     }
register    { return REGISTER; }
restrict	{ return RESTRICT; }
return      { return RETURN;   }
short       { return SHORT;    }
signed      { return SIGNED;   }
sizeof      { return SIZEOF;   }
static      { return STATIC;   }
struct      { return STRUCT;   }
switch      { return SWITCH;   }
typedef     { return TYPEDEF;  }
union       { return UNION;    }
unsigned    { return UNSIGNED; }
void        { return VOID;     }
volatile    { return VOLATILE; }
while       { return WHILE;    }
_Bool       { return _BOOL;    }
_Complex    { return _COMPLEX; }
_Imaginary  { return _IMAGINARY; }


	/* identifiers */
[a-zA-Z_][a-zA-Z_0-9]*	{ yylval.str.str = yytext; return IDENT; }


	/* tokens: numeric constants */
0[xX]{HEX}+{NTYPE}? {
		yylval.num.types |= NUMMASK_INTGR;	
		yylval.num.val = strtol(yytext, NULL, 16);
	
		checkNumberTypes(&yylval.num, yytext);
		return NUMBER;
	}

0{OCT}*{NTYPE}? {		
		yylval.num.types = NUMMASK_INTGR;	
		yylval.num.val = strtol(yytext, NULL, 8);

		checkNumberTypes(&yylval.num, yytext);
		return NUMBER;
	}

[1-9]{DEC}*{NTYPE}? {		
		yylval.num.types = NUMMASK_INTGR;
		yylval.num.val = strtol(yytext, NULL, 10);

		checkNumberTypes(&yylval.num, yytext);
		return NUMBER;
	}


	/* tokens: character & string literals */
L?' 		{ 	
		BEGIN CHR_LIT;

		/* zero out the string literal buffer */
		memset(strlit_buffer, 0, sizeof(strlit_buffer));
	}
L?\"		{ 	
		BEGIN STR_LIT;

		/* zero out the string literal buffer */
		memset(strlit_buffer, 0, sizeof(strlit_buffer));
	}
<CHR_LIT>'	{ 	
		yylval.str.char_val = strlit_buffer[0]; 
		BEGIN INITIAL;
		return CHARLIT;
	}
<STR_LIT>\"	{ 	
		BEGIN INITIAL;

		/* allocate space for string literal and copy it into yylval */
		if (!(yylval.str.str = malloc(yylval.str.str_size+1))) {
			fprintf(stderr, "Error allocating space for string "
									"literal: %s\n", strerror(errno));
			return -1;
		}
		memcpy(yylval.str.str, strlit_buffer, yylval.str.str_size);
		return STRING;
	}

<CHR_LIT,STR_LIT>\\[\\0nabtrfv'\"?]	{
		switch(yytext[1]) {
			case '\\':strlit_buffer[yylval.str.str_size] = 92; break;
			case '0': strlit_buffer[yylval.str.str_size] = 0;  break;
			case 'n': strlit_buffer[yylval.str.str_size] = 10; break;
			case 'a': strlit_buffer[yylval.str.str_size] = 7;  break;
			case 'b': strlit_buffer[yylval.str.str_size] = 8;  break;
			case 't': strlit_buffer[yylval.str.str_size] = 9;  break;
			case 'r': strlit_buffer[yylval.str.str_size] = 13; break;
			case 'f': strlit_buffer[yylval.str.str_size] = 12; break;
			case 'v': strlit_buffer[yylval.str.str_size] = 11; break;
			case '\'':strlit_buffer[yylval.str.str_size] = 39; break;
			case '"': strlit_buffer[yylval.str.str_size] = 34; break;
			case '?': strlit_buffer[yylval.str.str_size] = 63;
			default: break;
		} 
		++yylval.str.str_size;
	}

<CHR_LIT,STR_LIT>\\{OCT}{1,3}	{
		long int tmp = strtol(yytext+1, NULL, 8);
		if (tmp > 255)
			tmp = 255;
		strlit_buffer[yylval.str.str_size] = tmp;
		++yylval.str.str_size;
	}

<CHR_LIT,STR_LIT>\\x{HEX}+		{
		yytext[0] = '0';
		long int tmp = strtol(yytext, NULL, 16);
		if (tmp > 255)
			tmp = 255;
		strlit_buffer[yylval.str.str_size] = tmp;
		++yylval.str.str_size;
	}

<CHR_LIT,STR_LIT>\\.   { 
		fprintf(stderr, "%s:%d: Error: unrecognized character %s\n", 
								cur_file_name, cur_line_num, yytext);
		return -1;
	}

<CHR_LIT,STR_LIT>[^'\"]	 {
		strlit_buffer[yylval.str.str_size] = yytext[0];
		++yylval.str.str_size;
	}


	/* tokens: operators */
"!"     |
"%"     |
"^"     |
"&"     |
"*"     |
"-"     |
"+"     |
"="     |
"~"     |
"|"     |
"."     |
"<"     |
">"     |
"/"     |
"?"     { return yytext[0]; }

"/="    { return DIVEQ;   }
"%="    { return MODEQ;   }
"<<="   { return SHLEQ;   }
">>="   { return SHREQ;   }
"&="    { return ANDEQ;   }
"^="    { return XOREQ;   }
"|="    { return OREQ;    }
"->"    { return INDSEL;  }
"<<"    { return SHL;     }
">>"    { return SHR;     }
"<="    { return LTEQ;    }
">="    { return GTEQ;    }
"=="    { return EQEQ;    }
"!="    { return NOTEQ;   }
"&&"    { return LOGAND;  }
"||"    { return LOGOR;   }
"+="    { return PLUSEQ;  }
"-="    { return MINUSEQ; }
"*="    { return TIMESEQ; }
"++"    { return PLUSPLUS;   }
"--"    { return MINUSMINUS; }


	/* tokens: separator characters */
"("     |
")"     |
"["     |
"]"     |
"{"     |
"}"     |
","     |
";"     |
":"     { return yytext[0]; }
"..."   { return ELLIPSIS; }


	/* Preprocessor output as well as keeping track 
	   of file name and line number for error checking. */
#\ [0-9]+\ \"[a-zA-Z0-9\/\._]+\".*\n {
		/* get current file name */
		helper_begin = strstr(yytext, "\""); 
		helper_end = strstr(++helper_begin, "\"");
		strncpy(cur_file_name, helper_begin, (size_t)(helper_end-helper_begin));
		cur_file_name[(size_t)(helper_end-helper_begin)] = 0;

		/* get current line */
		helper_begin = strstr(yytext, " ");
		helper_end = strstr(++helper_begin, " ");
		strncpy(tmp, helper_begin, (size_t)(helper_end-helper_begin));
		cur_line_num = atoi(tmp);
	}
#.*\n 	{}    /* skip any other preprocessor output */

	/* error handling and reporting */
.   { 
		fprintf(stderr, "%s:%d: Error: unrecognized character %s\n", 
								cur_file_name, cur_line_num, yytext);
		return -1;
	}

%%
	/* Epilouge: Code to be run using lexer */

int main(){ 
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
				printf("%s\t%llu\t", "REAL", yylval.num.val);

				if (yylval.num.types & NUMMASK_FLOAT)
					printf("%s\n", "FLOAT");
				else
					printf("%s\n", "DOUBLE");
			}
		}
		else if (token_code == CHARLIT) {
			printf("%s\t%d\t%s\t", 
				cur_file_name, cur_line_num, 
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
			printf("%s\t%d\t%s\t%s\n", 
				cur_file_name, cur_line_num, 
				stringFromTokens(token_code), yylval.str.str);
		}
		else /* token is a keyword */ {
			if (token_code < 256)
				printf("%s\t%d\t%c\n", 
					cur_file_name, cur_line_num, token_code);
			else 
				printf("%s\t%d\t%s\n", cur_file_name, 
					cur_line_num, stringFromTokens(token_code));

		}

		memset(&yylval, 0, sizeof(yylval));  /* reset the token semantic value */      
	}

	return 0; 
}