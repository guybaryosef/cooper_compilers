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
struct astnode_unop {
    int op;
    struct astnode *right;
};

/* struct for a token string literal */
#define STRLIT_TYPE 5
struct astnode_strlit {
    char *str;
    int str_size;
};

/* struct for a token character literal */
#define CHRLIT_TYPE 6
struct astnode_chrlit {
    char c_val;
};


/* struct for an Abstract Syntax Tree */
struct astnode {
    int nodetype;   /* will contain the token name from the lexer       */
    union {         /* anonymous union for the different token values   */
        struct astnode_num num;
        struct astnode_ident ident;
        struct astnode_chrlit chrlit;
        struct astnode_strlit strlit;
        struct astnode_unop unop;
        struct astnode_binop binop;
    };
} astnode;


/*
 * This class of functions create new Abstract Tree Nodes.
 */
/* Handles integers and floats */
struct astnode *newNode_num(int token_name, struct YYnum num);

/* Handles IDENTs, CHRLITs, and STRLITs */ 
struct astnode *newNode_str(int token_name, struct YYstr str);

/* Handles Operations */
struct astnode *newNode_unop(int token_name);
struct astnode *newNode_binop(int token_name);


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