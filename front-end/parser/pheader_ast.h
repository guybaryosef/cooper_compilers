/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * pheader_ast.h - A header file for the parser
 * that contains the abstract syntax tree struct
 * and associated functions:
 *  -   Create a new node.
 *  -   Print out the AST in the hakner-designated format.
 *  -   Free an AST.
 */


#ifndef PARSER_AST
#define PARSER_AST

#include <stdio.h>


////////////////////////////////////////////////////////
/// Structs used in Abstract Syntax Tree Definitions ///
////////////////////////////////////////////////////////

#define IDENT_TYPE 1  /* struct for a token Identifer */
struct astnode_ident {  
    char *str;
};

#define NUM_TYPE 2    /* struct for a token numer */
struct astnode_num {
    int types;
    unsigned long long val;
    double d_val;
}; 

/* Comparison operators (< > <= >=) and logical 
   operators (&& ||) are both binary operations.
   The distinction is made only for 
   differentiating the AST-printing format. */
#define BINOP_TYPE 3    /* a binary operator    */
#define COMPARE_TYPE 4  /* comparison operators */
#define LOG_TYPE 5      /* logiacal operators   */
struct astnode_binop {
    int op;
    struct astnode *left, *right;
};

/* The address-of operator (&), the dereference 
   operator (*), and the SIZEOF function/expression
   are all binary operations. The distinction is 
   made only for differentiating the AST-printing format. */
#define UNOP_TYPE 6   /* a unary operator       */
#define ADDR_TYPE 7   /* address operator(&)    */
#define DEREF_TYPE 8  /* dereferece operator(*) */
#define SIZEOF_TYPE 9 /* sizeof operator        */
struct astnode_unop {
    int op;
    struct astnode *expr;
};

#define STRLIT_TYPE 10  /* a string literal */
struct astnode_strlit {
    char *str;
    int str_size;
};

#define CHRLIT_TYPE 11  /* a character literal */
struct astnode_chrlit {
    char c_val;
};

#define ARG_TYPE 12     /* a function argument */
struct astnode_arg {
    int num;        /* the argument count of the function */
    struct astnode *expr;
};

/* Struct for argument list. This struct won't get printed
   or used in the actual AST. Rather it is used as a helper 
   for the structs 'astnode_fnc' and 'astnode_arg'. */
#define ARGLIST_TYPE 13
struct astnode_arglist {
    int size;               /* numbers of arguments in the list */
    struct astnode **list;  /* the list of arguments            */
};

#define FNC_CALL 14 /* a function call */
struct astnode_fnc {
    struct astnode *ident;       /* ident (name) of the funtion         */
    struct astnode **arguments;  /* the argument list of the function   */
    int arg_count;               /* number of arguments in the function */
};

#define SLCT_TYPE 15        /* a selection expression (direct & indrect) */
struct astnode_slct {
    struct astnode *left, *right;   /* left-expression, right-identifier */
};

#define TERNARY_TYPE 16     /* a ternary operator */
struct astnode_ternary {
    struct astnode *if_expr, *then_expr, *else_expr;
};

/* The assignment operator struct has identical
   members as the binary operator stuct, however
   it isn't a binary op and so is made into a 
   seperate struct. */
#define ASS_TYPE 17 /* the assignment operator */
struct astnode_assignment {
    int op;
    struct astnode *left, *right;
};

#define PTR_TYPE 18  /* a pointer */
struct astnode_ptr {
    struct astnode *pointee;
};

#define ARRAY_TYPE 19   /* an array */
struct astnode_arr {
    int size;
    struct astnode *ptr;
};

/* There are 10 different scalar types. They can be
   either signed or unsigned, and so will all share
   the same struct in the AST, differentiated with
   an enum. */
enum ScalarTypes {Void, Char, Short, Int, Long, LongLong, 
            Bool, Float, Double, LongDouble, FloatComplex,
            DoubleComplex, LongDoubleComplex, FloatImag, 
            DoubleImag, LongDoubleImag};
#define SCALAR_TYPE 20  /* struct for a scalar type */
struct astnode_scalar_type {
    _Bool sign;         /* 0- unsigned, 1-signed */
    enum ScalarTypes type;
};

#define FNC_TYPE 21  /* a function type */
struct astnode_fnc_type {
    struct astnode **args_types;  /* the types of the arguments         */
    int arg_count;                /* number of arguments in the function*/
    struct astnode *return_type;  /* the return type of the function    */
};

#define STRUCT_TYPE 22 /* struct for a struct type */
struct astnode_struct {
    struct SymbolTable *stable;
};

////////////////////////////////////////////////////////
//////////////// Symbol Table Structs //////////////////
////////////////////////////////////////////////////////
/* There are 10 different uses for identifiers in the C language, most
 * of which require different attributes to be kept by the compiler.
 * These identifiers will be placed into a symbol table, and they
 * will be constructed as AST nodes. 
 * 
 * They will all be prefixed with stable to symbolize that they will
 * be the entries of a symbol table (defined in file 'symbol_table.h'). 
 * 
 * The 10 identifier purposes are: variable name, function name, 
 * typedef name, enum constant, struct tag, union tag, enum tag, label, 
 * struct member, and union member.
 * 
 * The decision to use AST nodes as symbol table entries instead of
 * a different, more specific struct is give flexibility for types
 * such as 'pointer to a const int'. This pointer needs to point
 * to a scalar int with a const type qualifier, and so in this case
 * it will point to a symbol table entry. By having this polymorphic
 * strucutre to the symbol table entries & AST nodes we are much 
 * more flexible with how we implement these declarations.  
 */
enum SymbolTableStorageClass { Auto = 1, Register, Extern, Static};
enum possibleTypeQualifiers { Const, Volatile, Restrict};
enum SymbolTableTypeQualifiers { None = 0, C, V, R, CV, CR, VR, CVR};

enum STEntry_Type { NO_TYPE = 0, VARIABLE_TYPE, FUNCTION_TYPE, 
                    SU_TAG_TYPE, ENUM_TAG, STATEMENT_LABEL, 
                    ENUM_CONST_TYPE, TYPEDEF_NAME, SU_MEMBER_TYPE};

#define STABLE_VAR 23  /* s_table entry for a variable */
struct stable_var {
    enum SymbolTableStorageClass storage_class;
    enum SymbolTableTypeQualifiers type_qualifier;
    int offset_within_stack_frame;
};

#define STABLE_FNC 24  /* s_table entry for a function */
struct stable_fnc {
    enum SymbolTableStorageClass storage_class;
    _Bool is_inline;
    _Bool is_defined;
    struct astnode *return_type;
    struct astnode **args_types;
};

#define STABLE_SU_TAG 25  /* s_table entry for a struct/union tag */
struct stable_sutag {
    _Bool is_defined;
    struct SymbolTable *s_u_table;
};

#define STABLE_ENUM_TAG 26  /* s_table entry for an enum tag */
struct stable_enumtag {
    _Bool is_defined;
};

#define STABLE_STMT_LABEL 27  /* s_table entry for a statement label */
struct stable_stmtlabel {
    int IR_assembly_label;
};

#define STABLE_ENUM_CONST 28  /* s_table entry for an enum constant */
struct stable_enumconst {
    struct SymbolTableEntry *tag;
    int val;
};

#define STABLE_TYPEDEF 29  /* s_table entry for a typedef name */
struct stable_typedef {
    struct astnode *equivalent_type;
};

#define STABLE_SU_MEMB 30  /* s_table entry for a struct/union member */
struct stable_sumemb {
    struct astnode *type;
    int offset_within_s_u;
    int bit_field_width;
    int bit_offset;
};


struct astnode_stable_entry {
    char *file_name;        /* file name where the ident first appeared  */
    int line_num;           /* line number of file where ident appeared  */
    enum STEntry_Type type; /* the type of ident that the entry holds     */
    struct astnode *node;   /* the AST node that represents this entry    */
    char *ident;            /* ident name */

    union { /* anonymous union */
        struct stable_var var;
        struct stable_fnc fnc;
        struct stable_sutag sutag;
        struct stable_enumtag enumtag;
        struct stable_stmtlabel stmtlabel;
        struct stable_enumconst enumconst;
        struct stable_typedef typedef_name;
        struct stable_sumemb sumemb;
    };
};








//////////////////////////////////////////////////////////
///////////// Abstract Syntax Tree Definition ////////////
//////////////////////////////////////////////////////////
typedef struct astnode {
    int nodetype;   /* the node type as based on the macros above       */
    union {         /* anonymous union for the different token values   */
        struct astnode_num num;
        struct astnode_ident ident;
        struct astnode_chrlit chrlit;
        struct astnode_strlit strlit;
        struct astnode_unop unop;
        struct astnode_binop binop;
        struct astnode_arg arg;
        struct astnode_fnc fnc;
        struct astnode_arglist arglist;
        struct astnode_slct slct;
        struct astnode_ternary ternary;
        struct astnode_assignment assignment;
        struct astnode_ptr ptr;
        struct astnode_arr arr;
        struct astnode_scalar_type scalar_type;
        struct astnode_fnc_type fnc_type;
        struct astnode_struct strct;
        struct astnode_stable_entry stable_entry;
    };
} astnode;


/* A simple struct used in the parser that acts as an array of 
   AST nodes. It is used when we have a declarator list. */
typedef struct astnode_list {
    astnode **list;
    int len;
} astnode_list;

/* astnode_list's constructor */
astnode_list *newASTnodeList(int len, astnode **cur_list);


//////////////////////////////////////////////////////////
///////////// Abstract Syntax Tree Functions /////////////
//////////////////////////////////////////////////////////

/* forward declartion - defined in front_end_header.h */
struct YYstr; struct YYnum;

//////// Constructors for the different AST nodes /////////
astnode *newNode_num(struct YYnum num);                  /* Integers and floats      */
astnode *newNode_str(int token_name, struct YYstr str);  /* IDENTs, CHRLITs, & STRLITS */
astnode *newNode_unop(int token_name);                   /* unary operation  */
astnode *newNode_binop(int token_name);                  /* binary operation */

/* Handles functions and function arguments */
astnode *newNode_fnc();
astnode *newNode_arglist();
void expand_arglist(struct astnode *);  /* makes the argument list greater by 1 */
astnode *newNode_arg(int num);

astnode *newNode_slct();         /* Component selection */
astnode *newNode_ternary();      /* ternary operator         */
astnode *newNode_assment();      /* assignment expressions   */
astnode *newNode_ptr();          /* pointer type */
astnode *newNode_arr(int size);  /* array type   */
astnode *newNode_scalarType(enum ScalarTypes, _Bool is_signed); /* scalar type  */
astnode *newNode_fncType(int arg_len);  /* function type*/
astnode *newNode_strctType();    /* struct type  */

/* forward declaration, will be defined in symbol_table.h */
struct TmpSymbolTableEntry; 

/* Handles new symbol table entries */
astnode *newNode_sTableEntry(struct TmpSymbolTableEntry *tmp_entry);


/*
 * token2op - Takes the token name of an operator and 
 * returns a string of how that operator looks in the C
 * language.
 * 
 * Ex: token name 'DIVEQ' will return the string "/=".
 */
char *token2op(int token_name);


/*
 * printAST - Given the root node of an Abstract Syntax Tree,
 * this function prints out the tree in a format matching 
 * Hakner's in the assignment sheet.
 * 
 * This is in essense a preorder traversal.
 */
void printAST(struct astnode *root, FILE *output_file);


/*
 * preorderTraversal - A helper function for printAST. This
 * function implements preorder traversal for the AST printing.
 */
void preorderTraversal(struct astnode *cur, FILE *output, int depth);


/* 
 * freeTree - Frees the dynamically allocated memory 
 * that was used by the Abstract Syntax Tree.
 *
 * This function does not return anything.
 */
void freeTree(struct astnode *root);


#endif
