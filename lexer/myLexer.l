
/* Prologue: Options and definitions */

/* flex doesn't call yywrap after end of file */
%option noyywrap 

/* no default rule for input */
%option nodefault

/* no default line counter, we do it ourselves */
%option noyylineno

/* setting up comment start state */
%x COMMENT
%x LITERAL

/* repeating patterns */
HEX [0-9a-fA-F]
DEC [0-9]
OCT [0-7]

/* vars which can be accessed inside yylex() and main() */
%{ 
	#include <math.h>
	#include "./lexer_header.h"
	#include "./numeric_conversions.c"
	#include <stdlib.h>
	#define LINESIZE 1024

	int cur_line_num = 1;            /* current line number  */
	char cur_file_name[LINESIZE+1];  /* current file name    */
	char tmp[20];                    /* temp helper variable */

	/* helper pointers for exracting the line number and file name */
	char *helper_end, *helper_begin;    

	/* struct and variable for the token semantic values */
	typedef union {
		struct num {
			int val;
			int u;
			int l_ll;	/* 0:neither, 1:long, 2:long long */
			int int_doub_float;	/* 0:int, 1:double, 2:float */
		} num;
		struct str {
			int is_char; /* 0- character literal, 1-string literal */
			char *str;
			int str_size;
			char char_val[4];
		} str;
	} YYSTYPE;

	YYSTYPE yylval;
%} 

%% 
	/* Body: (regexp) : (embedded action) pairs */

	/* skip comments */
"//".*\n		{ ++cur_line_num; }
"/*"   		    { BEGIN COMMENT;  }
<COMMENT>\n	    { ++cur_line_num; }
<COMMENT>[^\n"*/]*	{ }
<COMMENT>"*/" 	{ BEGIN INITIAL;  }


	/* skip white spaces  */
[ \t\r]     {}
\n			{ ++cur_line_num; }


	/* tokens: keywords */
auto        { return AUTO;  }
break       { return BREAK; }
case        { return CASE;  }
char        { return CHAR;  }
const       { return CONST; }
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
[a-zA-Z_][a-zA-Z_0-9]*	{
							yylval.str.str = yytext;
							return IDENT; 
						}


	/* tokens: numeric constants */

	
-?0x|X[0-9a-fA-F]+p?[0-9a-fA-F]+(((l|L)?(u|U)?)|((u|U)?(l|L)?)) {
		if (strstr(yytext, "u") || strstr(yytext, "U"))
			yylval.num.u = 1;
		if (strstr(yytext, "l") || strstr(yytext, "L"))
			yylval.num.l = 1;
		
		return NUMBER;
	}
-?0x|X[0-9a-fA-F]+p?[0-9a-fA-F]+(((ll|LL)?(u|U)?)|((u|U)?(ll|LL)?)) {

}


-?0[0-7]+((((ll|LL)|(l|L))(u|U))|((u|U)((ll|LL)|(l|L))))? {

		return NUMBER;
	}

-?[0-9]+e?[0-9]+((((ll|LL)|(l|L))(u|U))|((u|U)((ll|LL)|(l|L))))? {

		return NUMBER;
	}


	/* tokens: character literals */
L?' 		{ 	
	BEGIN LITERAL; 
	yylval.str.is_char = 1;
}
[^\\]?'		{ 	
	BEGIN INITIAL;
	return CHARLIT;
}

<LITERAL>[\n\0\a\b\f\r\t\v]	{ 
	if (yylval.str.is_char) {
		if (yylval.str.char_val[0])
			continue;
		else {
			yylval.str.char_val[0] = '\\';
			switch(yytext[0]) {
				case '\n': yylval.str.char_val[1] = 'n'; break;
				case '\0': yylval.str.char_val[1] = '0'; break;
				case '\a': yylval.str.char_val[1] = 'a'; break;
				case '\b': yylval.str.char_val[1] = 'b'; break;
				case '\f': yylval.str.char_val[1] = 'f'; break;
				case '\r': yylval.str.char_val[1] = 'r'; break;
				case '\t': yylval.str.char_val[1] = 't'; break;
				case '\v': yylval.str.char_val[1] = 'v'; 
			}
		}
	}
	else {
		/* allocate more space for the string literal if neccasary */
		if (strlen(yylval.str.str) == yylval.str.str_size-5) {
			char *tmp;
			if (!(tmp = calloc(2*yylval.str.str_size, 1))) {
					fprintf(stderr, "Error allocating space for string"
						" literal: %s\n", strerror(errno));
					return -1;
			}
			strcpy(tmp, yylval.str.str);
			free(yylval.str.str);
			yylval.str.str = tmp;
		}

		switch(yytext[0]) {
			case '\n': 
				strcpy(yylval.str.str+strlen(yylval.str.str), "\n"); break;
			case '\0':
				strcpy(yylval.str.str+strlen(yylval.str.str), "\0"); break;
			case '\a':
				strcpy(yylval.str.str+strlen(yylval.str.str), "\a"); break;
			case '\b':
				strcpy(yylval.str.str+strlen(yylval.str.str), "\b"); break;
			case '\f':
				strcpy(yylval.str.str+strlen(yylval.str.str), "\f"); break;
			case '\r':
				strcpy(yylval.str.str+strlen(yylval.str.str), "\r"); break;
			case '\t':
				strcpy(yylval.str.str+strlen(yylval.str.str), "\t"); break;
			case '\v':
				strcpy(yylval.str.str+strlen(yylval.str.str), "\v");
		}

	}
}


<LITERAL>\\OCT{1,3}		{
	if (yylval.str.is_char) {
		if (yylval.str.char_val[0])
			continue;
		else {
			int i = 0;
			yylval.str.char_val[i] = '\\';
			for ( ; i < 4-strlen(yytext); ++i)
				yylval.str.char_val[i+1] = '0';
			strcpy(yylval.str.char_val+i+1, yytext+1);
		}
	}
	else {

	}
}

<LITERAL>\\xHEX+	{
	int val = hex2dec(yytext);
	
	return val;
}

<LITERAL>\\.   { 
	fprintf(stderr, "%s:%d: Error: unrecognized character %s\n", 
							cur_file_name, cur_line_num, yytext);
	return -1;
}

<LITERAL>.		{
	if (yylval.str.is_char) {
		if (yylval.str.char_val[0])
			continue;
		else
			yylval.str.char_val[0] = yytext[0];
	}
	else {
		/* allocate more space for the string literal if neccasary */
		if (strlen(yylval.str.str) == yylval.str.str_size-5) {
			char *tmp;
			if (!(tmp = calloc(2*yylval.str.str_size, 1))) {
					fprintf(stderr, "Error allocating space for string"
						" literal: %s\n", strerror(errno));
					return -1;
			}
			strcpy(tmp, yylval.str.str);
			free(yylval.str.str);
			yylval.str.str = tmp;
		}

		strcpy(yylval.str.str+strlen(yylval.str.str), yytext);
	}
}

	/* tokens: string literals */
L?\"		{ 	BEGIN LITERAL;
				yylval.str.is_char = 0;
				if (!(yylval.str.str = calloc(50*sizeof(char), 1))) {
					fprintf(stderr, "Error allocating space for string"
						" literal: %s\n", strerror(errno));
					return -1;
				}
				yylval.str.str_size = 0;
			}
[^\\]?\"	{ 	
				BEGIN INITIAL;
				return STRING;
			}



	/* tokens: operators */
"!"     { return '!'; }
"%"     { return '%'; }
"^"     { return '^'; }
"&"     { return '&'; }
"*"     { return '*'; }
"-"     { return '-'; }
"+"     { return '+'; }
"="     { return '='; }
"~"     { return '~'; }
"|"     { return '|'; }
"."     { return '.'; }
"<"     { return '<'; }
">"     { return '>'; }
"/"     { return '/'; }
"?"     { return '?'; }

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
"("     { return '('; }
")"     { return ')'; }
"["     { return '['; }
"]"     { return ']'; }
"{"     { return '{'; }
"}"     { return '}'; }
","     { return ','; }
";"     { return ';'; }
":"     { return ':'; }
"..."   { return ELLIPSIS; }


	/* Preprocessor output as well as keeping track 
	   of file name and line number for error checking. */
#\ [0-9]+\ \"[a-zA-Z0-9\/\._]+\".*\n {
		helper_begin = strstr(yytext, "\""); 
		helper_end = strstr(++helper_begin, "\"");
		strncpy(cur_file_name, helper_begin, (size_t)(helper_end-helper_begin));
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
			char value[3];
			strcpy(value, "65");
			char tmp1[10];
			strcpy(tmp1, "INTEGER");
			char num_type[10];
			strcpy(num_type, "DOUBLE");
			printf("%s\t%d\t%d\t%s\t%s\t%s\n", 
				cur_file_name, cur_line_num, token_code, tmp1, value, num_type);
		}
		else if (yylval.str.str)
			printf("%s\t%d\t%s\t%s\n", 
				cur_file_name, cur_line_num, 
				stringFromTokens(token_code), yylval.str.str);
		else if (token_code < 256)
			printf("%s\t%d\t%c\n", 
				cur_file_name, cur_line_num, token_code);
		else
			printf("%s\t%d\t%s\n", 
				cur_file_name, cur_line_num, stringFromTokens(token_code));

		memset(&yylval, 0, sizeof(yylval));  /* reset the token semantic value */      
	}

	return 0; 
}