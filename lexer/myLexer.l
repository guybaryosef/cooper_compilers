
/* Prologue: Options and definitions */

/* tells lexer to read only 1 input file */
%option noyywrap  

/* vars which can be accessed inside yylex() and main() */
%{ 
      #include "./my_test/tokens-manual.h"
      #include <stdlib.h>
      #define LINESIZE 1024

      int cur_line_num = 1;            /* current line number  */
      char cur_file_name[LINESIZE+1];  /* current file name    */
      char tmp[20];                    /* temp helper variable */

      /* helper pointers for exracting the line number and file name */
      char *helper_end, *helper_begin;    

      typedef union {
            char chars[4];
            char *string_literal;
            int integer;
      } YYSTYPE;
      YYSTYPE yylval;   /* contains the token semantic value */
%} 
  
%% 
      /* Body: regexp - embedded action pairs */

      /* skip comments */
//.*\n            {     ++cure_line_num; }
/\*[.\n]*\*/      {
                        helper_begin = yytext;
                        while (helper_begin = strstr(helper_begin+1, '\n'))
                              ++cur_line_num;
                  }

      /* skip white spaces (not including newlines) */
[ \t\r]     {}

      /* tokens: numeric constants */
(0x)[0-9a-fA-F]+p?[0-9a-fA-F]+

0[0-7]+

[0-9]+e?[0-9]+((ll|LL)|(u|U)|(l|L)


      /* tokens: characters (including but not differentiating, wide chars) */
L?'[^ \n([^\]')]+[^\]'  { 
                  if ((helper_begin = strstr(yytext, '\'))) {
                        strncpy(yylval.chars, helper_begin, 2);
                        yylval.chars[2] = 0;
                  }
                  else {
                        yylval.chars[0] = yytext[1];
                        yylval.chars[1] = 0;
                  }
                  return CHARLIT;
            }

      /* tokens: string literals */
L?\"[a-zA-Z_][a-zA-Z0-9_]+\"  { 
                  yylval.string_literal = (yytext+1);
                  return IDENT;
            }

      /* tokens: operators */
!     { return '!'; }
%     { return '%'; }
^     { return '^'; }
&     { return '&'; }
*     { return '*'; }
-     { return '-'; }
+     { return '+'; }
=     { return '='; }
~     { return '~'; }
|     { return '|'; }
.     { return '.'; }
<     { return '<'; }
>     { return '>'; }
/     { return '/'; }
?     { return '?'; }

+=    {return PLUSEQ; }
-=    { return MINUSEQ; }
*=    { return TIMESEQ; }
/=    { return DIVEQ; }
%=    { return MODEQ; }
<<=   { return SHLEQ; }
>>=   { return SHREQ; }
&=    { return ANDEQ; }
^=    { return XOREQ; }
|=    { return OREQ;  }

->    { return INDSEL; }
++    { return PLUSPLUS;   }
--    { return MINUSMINUS; }
<<    { return SHL;   }
>>    { return SHR;   }
<=    { return LTEQ;  }
>=    { return GTEQ;  }
==    { return EQEQ;  }
!=    { return NOTEQ; }
&&    { return LOGAND }
||    { return LOGOR  }

      /* tokens: separator characters */
(     { return '('; }
)     { return ')'; }
[     { return '['; }
]     { return ']'; }
{     { return '{'; }
}     { return '}'; }
,     { return ','; }
;     { return ';'; }
:     { return ':'; }
...   { return ELLIPSIS; }

      /* tokens: keywords */
auto        { return AUTO;  }
break       { return BREAK; }
case        { return CASE;  }
char        { return CHAR;  }
const       { return CONST; }
continue    { return CONTINUE; }
default     { return DEFAULT;  }
do          { return DO;       }
double      { return DOUBLe;   }
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

      /* Preprocessor output as well as keeping track 
         of file name and line number for error checking. */
\n          { ++cur_line_num; }
#\ [0-9]+\ \"[a-zA-Z0-9\/\._]+\".*\n {
                  helper_begin = strstr(yytext, "\""); 
                  helper_end = strstr(++helper_begin, "\"");
                  strncpy(cur_file_name, helper_begin, (size_t)(helper_end-helper_begin));
                  helper_begin = strstr(yytext, " ");
                  helper_end = strstr(++helper_begin, " ");
                  strncpy(tmp, helper_begin, (size_t)(helper_end-helper_begin));
                  cur_line_num = atoi(tmp);
            }
       {}    /* skip any other preprocessor output */


      /* look for something */
[A-Z]+ { printf("%s capital letter\n", yytext); } 


      /* error handling */
.     { fprintf(stderr, "%s:%d: Error: unrecognized character %s\n", 
                                    cur_file_name, cur_line_num, yytext);
        return -1; }

%% 
/* Epilouge: Code to be run using lexer */

int main(){ 

      int token_code;
      /* table format to compare output to hakner lexer */
      while (token_code = yylex()) {
            if (yylval && token_code == NUMBER)
                  printf("%s\t%d\t%d%d\t%s\t%d\n", 
                              cur_file_name, cur_line_num, token_code,
                              token_semantic_value, value, num_type);
            else if (yylval)
                  printf("%s\t%d\t%d%d\t%s\n", 
                              cur_file_name, cur_line_num, token_code,
                              token_semantic_value, value);
            else
                  printf("%s\t%d\t%d\n", 
                              cur_file_name, cur_line_num, token_code);
      
            yylval = 0;  /* reset the token semantic value */      
      }
  
      return 0; 
}