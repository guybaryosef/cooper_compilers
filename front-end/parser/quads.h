/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * quads.h - Declares the functions and defines the structs
 * associated with quads (IR generation). 
 */
#include <stdio.h>


#ifndef QUADS
#define QUADS

struct astnode;


/* sizeof operator constants */
#define DATATYPE_CHAR_SIZE 1
#define DATATYPE_SHORT_SIZE 2
#define DATATYPE_INTEGER_SIZE 4
#define DATATYPE_LONG 8
#define DATATYPE_LONGLONG 8
#define DATATYPE_POINTER 8
#define DATATYPE_FLOAT 4
#define DATATYPE_DOUBLE 8
#define DATATYPE_LONGDOUBLE 16

/**
 * evaluateSizeOf - Evalutates the SizeOf operators into a constant.
 * Because we are not implementing a variable length array any time
 * soon, this will always evaluate into a constant during compile-time.
 */
struct astnode *evaluateSizeOf(struct astnode *node);

/**
 * newGenericTemp - Generates a new generic node that is of temporary
 * type. There temporary types will be used when evaluating
 * subexpressions and the like. We will defer register allocation until
 * the BackEnd and so for now we will assume an infinite supply of 
 * virtual registers in which to place temporary values.
 */
struct astnode *newGenericTemp();


/////////////////////////////////////////////////////////////////////////
///////////////////////////////// Quads /////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/**
 * Contains all opcodes possible in our IR.
 * Rule for categorizing types (sizes) in quads is final letter in opcode:
 *  - b - bytes - 8 bits
 *  - w - words - 16 bites
 *  - l - long  - 32 bites
 *  - q - quads - 64 bites 
 */
enum QuadOpcode {   MOVB = 1, MOVW, MOVL, MOVQ, ADDB, ADDW, ADDL, ADDQ,
                    MODB, MODW, MODL, MODQ,     XORB, XORW, XORL, XORQ,
                    ANDB, ANDW, ANDL, ANDQ,     MULB, MULW, MULL, MULQ,
                    SUBB, SUBW, SUBL, SUBQ,     COMPLB, COMPLW, COMPLL, COMPLQ,
                    ORB, ORW, ORL, ORQ,         LT, GT, SHL_OP, SHR_OP, LTEQ_OP, GTEQ_OP, 
                    DIVB, DIVW, DIVL, DIVQ,      EQEQ_OP, NOTEQ_OP, 
                    LOGO, LOGN, COMMA,       DEREF, PLPL, MINMIN,
                    NEG, POS, LOG_NEG_EXPR,     ADDR_OF, STORE, LOAD, LEA
                };  


/* Each quad consists of these four pieces, note that not all 3 generic
   nodes are always filled. */
typedef struct Quad {
    enum QuadOpcode opcode;
    struct astnode *result, *src1, *src2;
} Quad;


/**
 * emitQuad - generates a new quad with the specified opcode and the generic
 * nodes des (destination), source 1 (src 1), and source 2 (src2).
 */
Quad *emitQuad(enum QuadOpcode op, struct astnode *des, struct astnode *src1, struct astnode *src2);


/* A linked list wrapper of a quad */
typedef struct QuadLLNode {
    Quad quad;                  /* quad that the node contains  */
    struct QuadLLNode *next;    /* next node in the linked list */
} QuadLLNode;


/**
 * newQuadLLNode - Creates and returns a new linked list node of a quad.
 */
QuadLLNode *newQuadLLNode(Quad *new_quad);






/////////////////////////////////////////////////////////////////////////
///////////////////////////// Basic Blocks //////////////////////////////
/////////////////////////////////////////////////////////////////////////

/* each basic block is made out of a linked list of quads and a unique label */
typedef struct BasicBlock {
    char *u_label;          /* a unique label */
    QuadLLNode *quads_ll;   /* linked list of quads */
} BasicBlock;



/**
 * generateQuads - Generates the Intermediate Representation (quads) of 
 * a function and stores them into a specified file (stdout be default). 
 */
void generateQuads(struct astnode *root, FILE *output_file);

  
/**
 * printQuads - Prints out the Intermediate Representation (quads) of
 * the expressions inside a compound statement. 
 */
void genQuad(struct astnode *compound_stmt, FILE *output_file);


/**
 * generateAssignmentIR - Generates the IR of an assignment operation.
 */
void generateAssignmentIR(struct astnode *node);


enum LvalueMode { DIRECT = 1, INDIRECT };
/**
 * genLvalue - Generates the L values of an expression.
 */
struct astnode *genLvalue(struct astnode *node, enum LvalueMode *mode);


/**
 * genRvalue - Generates the r-values of an expression.
 */
struct astnode *genRvalue(struct astnode *node, struct astnode *target);


/**
 * op2str - A helper function to output the generated QUADs opcode. 
 */
char *op2str(enum QuadOpcode op);

/**
 * node2str - A helper function used to output the value of a node in QUADS.
 */
char *node2str(struct astnode *node);


/**
 * confirmSamePointer - Checks that the 2 AST nodes inputted in are pointing
 * to the same type. If so, returns size of the type in an integer AST node.
 * Else returns a NULL AST pointer.
 */
struct astnode *confirmSamePointer(struct astnode *node1,struct astnode *node2);


#endif