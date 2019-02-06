/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 *
 * lexer.l - The flex file that will be
 * used to build the lexical analyzer.
 */

/* Prologue: Options and definitions */

/* flex doesn't call yywrap after end of file */
%option noyywrap 

/* no default line counter, we do it ourselves */
%option noyylineno

/* setting up start states */
%x COMMENT
%x CHR_LIT
%x STR_LIT

/* repeating patterns */
HEX		[0-9a-fA-F]
DEC		[0-9]
OCT		[0-7]
NTYPE	((l|L|ll|LL)?[uU]?)|([uU]?(l|L|ll|LL)?)	

/* vars which can be accessed inside yylex() and main() */
%{ 
	#include "lheader2.h"
	#include "lheader.h"

	int cur_line_num = 1;					/* current line number  */
	char cur_file_name[LINESIZE+1];			/* current file name    */
	char tmp[20];							/* temp helper variable */
	char strlit_buffer[MAX_STRLIT_SIZE];	/* buffer for string literals */

	/* helper pointers for exracting the line numbers and file names */
	char *helper_end, *helper_begin;    

	YYSTYPE yylval;
%} 

%% 
	/* Body: (regexp) : (embedded action) pairs */

	/* skip comments */
"//".*\n			{ ++cur_line_num; }
"/*"				{ BEGIN COMMENT;  }
<COMMENT>\n			{ ++cur_line_num; }
<COMMENT>[^\n"*/]*	{}
<COMMENT>"*/"		{ BEGIN INITIAL;  }


	/* skip white spaces  */
[ \t\r]		{}
\n			{ ++cur_line_num; }


	/* tokens: keywords */
auto		{ return AUTO;     }
break		{ return BREAK;    }
case		{ return CASE;     }
char		{ return CHAR;     }
const		{ return CONST;    }
continue	{ return CONTINUE; }
default		{ return DEFAULT;  }
do			{ return DO;       }
double		{ return DOUBLE;   }
else		{ return ELSE;     }
enum		{ return ENUM;     }
extern		{ return EXTERN;   }
float		{ return FLOAT;    }
for			{ return FOR;      }
goto		{ return GOTO;     }
if			{ return IF;       }
inline		{ return INLINE;   }
int			{ return INT;      }
long		{ return LONG;     }
register	{ return REGISTER; }
restrict	{ return RESTRICT; }
return		{ return RETURN;   }
short		{ return SHORT;    }
signed		{ return SIGNED;   }
sizeof		{ return SIZEOF;   }
static		{ return STATIC;   }
struct		{ return STRUCT;   }
switch		{ return SWITCH;   }
typedef		{ return TYPEDEF;  }
union		{ return UNION;    }
unsigned	{ return UNSIGNED; }
void		{ return VOID;     }
volatile	{ return VOLATILE; }
while		{ return WHILE;    }
_Bool		{ return _BOOL;    }
_Complex	{ return _COMPLEX; }
_Imaginary	{ return _IMAGINARY; }


	/* identifiers */
[a-zA-Z_][a-zA-Z_0-9]*	{ yylval.str.str = yytext; return IDENT; }


	/* tokens: integer constants */
0[xX]{HEX}+{NTYPE}? {
		yylval.num.types |= NUMMASK_INTGR;	
		yylval.num.val = strtol(yytext, NULL, 16);
	
		checkNumberTypes(&yylval, yytext);
		return NUMBER;
	}

0{OCT}*{NTYPE}? {		
		yylval.num.types = NUMMASK_INTGR;	
		yylval.num.val = strtol(yytext, NULL, 8);

		checkNumberTypes(&yylval, yytext);
		return NUMBER;
	}

[1-9]{DEC}*{NTYPE}? {		
		yylval.num.types = NUMMASK_INTGR;
		yylval.num.val = strtol(yytext, NULL, 10);

		checkNumberTypes(&yylval, yytext);
		return NUMBER;
	}

	/* tokens: float constants */
{DEC}+[eE][+-]?{DEC}+[fFlL]? {
	char f_tmp = yytext[strlen(yytext)-1];
	if (f_tmp == 'f' || f_tmp == 'F')
		yylval.num.types = NUMMASK_FLOAT;
	else if (f_tmp == 'l' || f_tmp == 'L')
		yylval.num.types = NUMMASK_LDBLE;
	else
		yylval.num.types = NUMMASK_DOUBLE;

	yylval.num.d_val = strtod(yytext, NULL);
	return NUMBER;
}


(({DEC}+\.)|({DEC}+\.{DEC}+)|(\.{DEC}+))([eE][+-]?{DEC}+)?[fFlL]? {
	char f_tmp = yytext[strlen(yytext)-1];
	if (f_tmp == 'f' || f_tmp == 'F')
		yylval.num.types = NUMMASK_FLOAT;
	else if (f_tmp == 'l' || f_tmp == 'L')
		yylval.num.types = NUMMASK_LDBLE;
	else
		yylval.num.types = NUMMASK_DOUBLE;

	yylval.num.d_val = strtod(yytext, NULL);
	return NUMBER;
}


0[xX]{HEX}+[pP][+-]?{DEC}+[fFlL]? {
	char f_tmp = yytext[strlen(yytext)-1];
	if (f_tmp == 'f' || f_tmp == 'F')
		yylval.num.types = NUMMASK_FLOAT;
	else if (f_tmp == 'l' || f_tmp == 'L')
		yylval.num.types = NUMMASK_LDBLE;
	else
		yylval.num.types = NUMMASK_DOUBLE;


	yylval.num.d_val = strtod(yytext, NULL);
	return NUMBER;
}

0[xX]({HEX}+\.)|({HEX}+\.{HEX}+)|(\.{HEX}+)[pP][+-]?{DEC}+[fFlL]? {
	char f_tmp = yytext[strlen(yytext)-1];
	if (f_tmp == 'f' || f_tmp == 'F')
		yylval.num.types = NUMMASK_FLOAT;
	else if (f_tmp == 'l' || f_tmp == 'L')
		yylval.num.types = NUMMASK_LDBLE;
	else
		yylval.num.types = NUMMASK_DOUBLE;


	yylval.num.d_val = strtod(yytext, NULL);
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
		if (yylval.str.str_size < MAX_STRLIT_SIZE) {
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
	}

<CHR_LIT,STR_LIT>\\{OCT}{1,3}	{
		if (yylval.str.str_size < MAX_STRLIT_SIZE) {
			long int tmp = strtol(yytext+1, NULL, 8);
			if (tmp > 255)
				tmp = 255;
			strlit_buffer[yylval.str.str_size] = tmp;
			++yylval.str.str_size;
		}
	}

<CHR_LIT,STR_LIT>\\x{HEX}+		{
		if (yylval.str.str_size < MAX_STRLIT_SIZE) {
			yytext[0] = '0';
			long int tmp = strtol(yytext, NULL, 16);
			if (tmp > 255)
				tmp = 255;
			strlit_buffer[yylval.str.str_size] = tmp;
			++yylval.str.str_size;
		}
	}

<CHR_LIT,STR_LIT>\\.   { 
		if (yylval.str.str_size < MAX_STRLIT_SIZE) {
			fprintf(stderr, "%s:%d: Error: unrecognized character %s\n", 
										cur_file_name, cur_line_num, yytext);
			return -1;
		}
	}

<CHR_LIT,STR_LIT>[^'\"]	 {
		if (yylval.str.str_size < MAX_STRLIT_SIZE) {
			strlit_buffer[yylval.str.str_size] = yytext[0];
			++yylval.str.str_size;
		}
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
