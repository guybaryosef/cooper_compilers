/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * pheader_ast.h - A header file for the parser
 * that contains the abstract syntax tree structure
 * and associated functions:
 *  -   Create a new node.
 *  -   Print out the AST in the hakner-designated format.
 *  -   Free an AST.
 */


#ifndef PARSER_AST
#define PARSER_AST


/* struct for a token Identifer */
#define IDENT_TYPE 1
struct astnode_ident {
    char *str;
};

/* struct for a token numer */
#define NUM_TYPE 2
struct astnode_num {
    int types;
    unsigned long long val;
    double d_val;
}; 

/* struct for a token binary operator */
#define BINOP_TYPE 3
struct astnode_binop {
    int op;
    struct astnode *left, *right;
};

/* struct for a token uniary operator */
#define UNOP_TYPE 4
#define ADDR_TYPE 5     /* address of op (&) is a unary op with different AST-printing format */
#define DEREF_TYPE 6    /* dereferece operator (*) is a unary op with different AST-printing format */
struct astnode_unop {
    int op;
    struct astnode *expr;
};

/* struct for a token string literal */
#define STRLIT_TYPE 7
struct astnode_strlit {
    char *str;
    int str_size;
};

/* struct for a token character literal */
#define CHRLIT_TYPE 8
struct astnode_chrlit {
    char c_val;
};

/* struct for a function argument */
#define ARG_TYPE 9
struct astnode_arg {
    int num;        /* the argument number of the function */
    struct astnode *expr;
};

/* Struct for argument list. This struct won't get printed, rather
   instead it is a helper for the structs astnode_fnc and astnode_arg. */
#define ARGLIST_TYPE 10
struct astnode_arglist {
    int size;               /* numbers of arguments in the list */
    struct astnode **list;  /* the list of arguments            */
};

/* struct for a function call */
#define FNC_TYPE 11
struct astnode_fnc {
    struct astnode *ident;      /* ident (name) of the funtion          */
    struct astnode **arguments; /* the argument list of the function    */
    int arg_count;              /* number of arguments in the function  */
};

/* struct for a selection expression (direct & indrect) */
#define SLCT_TYPE 12
struct astnode_slct {
    struct astnode *left, *right;   /* left-expression, right-identifier */
};


/* struct for an Abstract Syntax Tree */
struct astnode {
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
    };
} astnode;


/*
 * This class of functions create new Abstract Tree Nodes.
 */
/* Handles integers and floats */
struct astnode *newNode_num(struct YYnum num);

/* Handles IDENTs, CHRLITs, and STRLITs */ 
struct astnode *newNode_str(int token_name, struct YYstr str);

/* Handles Operations */
struct astnode *newNode_unop(int token_name);
struct astnode *newNode_binop(int token_name);

/* Handles functions and function arguments */
struct astnode *newNode_fnc();
struct astnode *newNode_arglist();
void expand_arglist(struct astnode *);  /* makes the argument list greater by 1 */
struct astnode *newNode_arg(int num);

/* Handles component selection */
struct astnode *newNode_slct(); /* Input: 0-direct, 1-indirect */



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