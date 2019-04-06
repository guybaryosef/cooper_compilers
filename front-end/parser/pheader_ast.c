/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * pheader_ast.c - This file contains the implementations
 * of the functions defined in pheader_ast.h.
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "../front_end_header.h"
#include "../lexer/lheader.h"
#include "../lexer/lheader2.h"
#include "symbol_table.h"
#include "pheader_ast.h"


////////////////////////////////////////////////////////
////// Constructors for Abstract Syntax Tree Nodes /////
////////////////////////////////////////////////////////


/*
 *  newNode_num - Creates a new AST node of type num.
 */
astnode *newNode_num(struct YYnum num) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = NUM_TYPE;

    node->num.types = num.types;
    node->num.val = num.val;
    node->num.d_val = num.d_val;

    return node;
}


/*
 * newNode_str - Creates anew AST node of type identifier,
 * string literal, or character literal.
 * 
 * Because they all are contained in a struct str, we can
 * handle all of these differnet tokens together.
 */
astnode *newNode_str(int token_name, struct YYstr str) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    switch(token_name) {
        case IDENT:
            node->nodetype = IDENT_TYPE;
            node->ident.str = str.str;
            break;       
        case CHARLIT:
            node->nodetype = CHRLIT_TYPE;
            node->chrlit.c_val = str.char_val;
            break;
        case STRING:
            node->nodetype = STRLIT_TYPE;
            node->strlit.str = str.str;
            node->strlit.str_size = str.str_size;
            break;
    }
    return node;
}


/*
 * newNode_unop - Creates a new AST node for type unop 
 * (unary operation).
 */
astnode *newNode_unop(int token_name) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    switch(token_name) {
        case SIZEOF:
            node->nodetype = SIZEOF_TYPE;
            break;
        case '*':
            node->nodetype = DEREF_TYPE;
            break;
        case '&': 
            node->nodetype = ADDR_TYPE;
            break;
        default:
            node->nodetype = UNOP_TYPE;
    }
    node->unop.op = token_name;
    node->unop.expr = NULL;
    return node;
}


/*
 * newNode_binop - Creates a new AST node for type binop 
 * (binary operation).
 */
astnode *newNode_binop(int token_name) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    switch(token_name) {
        case '>':
        case '<':
        case LTEQ:
        case GTEQ:
        case EQEQ:
        case NOTEQ:
            node->nodetype = COMPARE_TYPE;
            break;
        case LOGAND:
        case LOGOR:
            node->nodetype = LOG_TYPE;
            break;
        default:
            node->nodetype = BINOP_TYPE;
    }

    node->binop.op = token_name;
    node->binop.left = NULL;
    node->binop.right = NULL;
    return node;
}


/*
 * newNode_func - Creates a new AST node
 * of type function.
 * 
 * An arg_count of -1 indicates that the amount
 * of arguments is unknown.
 */
astnode *newNode_fnc() {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = FNC_CALL;
    node->fnc.arg_count = 0;
    node->fnc.arguments = NULL;
    node->fnc.ident = NULL;
    return node;
}


/*
 * newNode_arglist - creates a new AST node
 * for the argument list of a function.
 */
astnode *newNode_arglist() {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    node->nodetype = ARGLIST_TYPE;
    node->arglist.size = 0;
    node->arglist.list = NULL;
    return node;
}

/*
 * expand_arglist - Expands the list of arguments
 * by 1.
 */
void expand_arglist(astnode *node) {
    astnode **new_arg_list = malloc((node->arglist.size+1)*sizeof(astnode *));

    memcpy(new_arg_list, node->arglist.list, sizeof(astnode *)*node->arglist.size);

    free(node->arglist.list);
    node->arglist.list = new_arg_list;
}

/*
 * newNode_arg - creates a new AST node
 * of type function argument.
 */
astnode *newNode_arg(int num) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = ARG_TYPE;
    node->arg.num = num;
    node->arg.expr = NULL;
    return node;
}

/*
 * newNode_slct - creates a new AST node
 * of type component selection.
 * 
 * Type:
 *  - 0: Direct component selection.
 */
astnode *newNode_slct() {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = SLCT_TYPE;
    node->slct.left = NULL;
    node->slct.right = NULL;
    return node;
}


/*
 * newNode_ternary - Creates an AST node for
 * the ternary operator (expr ? res1 : res 2).
 */
astnode *newNode_ternary() {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = TERNARY_TYPE;
    node->ternary.if_expr = NULL;
    node->ternary.then_expr = NULL;
    node->ternary.else_expr = NULL;
    return node;
}


/*
 * newNode_assment - Creates an AST node for the
 * assignment expression operator (=).
 */
astnode *newNode_assment(int op) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = ASS_TYPE;
    node->assignment.op = op;
    node->assignment.left = NULL;
    node->assignment.right = NULL;
    return node;    
}


/*
 * newNode_ptr - Creates an AST node for a pointer.
 */
astnode *newNode_ptr(enum SymbolTableTypeQualifiers qual) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = PTR_TYPE;
    node->ptr.pointee = NULL;
    node->ptr.type_qualifier = qual;
    return node;
}


/*
 * newNode_arr - Creates an AST node for an array.
 * 
 * An input of -1 indicates that it is an incomplete array type.
 */
astnode *newNode_arr(int size) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = ARRAY_TYPE;
    node->arr.size = size;
    node->arr.ptr = newNode_ptr(None);
    return node;
}

/*
 * newNodeType - Creates a new AST node for a scalar type.
 */
astnode *newNode_scalarType(enum ScalarTypes type, _Bool is_signed) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = SCALAR_TYPE;
    node->scalar_type.sign = is_signed;
    node->scalar_type.type = type;
    return node;
}


/*
 * newNode_fncType - Creates a new AST node for a
 * function type.
 */
astnode *newNode_fncType(int arg_len) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = FNC_TYPE;
    node->fnc_type.arg_count = arg_len;
    if (arg_len > 0)
        node->fnc_type.args_types = calloc(arg_len, sizeof(astnode *));
    else
        node->fnc_type.args_types = NULL;
    node->fnc_type.return_type = NULL;
    node->fnc_type.fnc_body = NULL;
    return node;
}

/*
 * newNode_strctType - Creates a new AST node that holds
 * the symbol table of a struct type.
 */
astnode *newNode_strctType() {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = STRUCT_TYPE;
    node->strct.stable = sTableCreate();
    return node;
}

/*
 * newNode_conditionalStmt - Creates a new AST node that holds
 * an if-else conditional statment.
 */
astnode *newNode_conditionalStmt
        (astnode *expr, astnode *if_stmt, astnode *else_stmt) {
    
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    
    node->nodetype = CONDITIONAL_STMT;
    node->conditional_stmt.expr = expr;
    node->conditional_stmt.if_node = if_stmt;
    node->conditional_stmt.else_node = else_stmt;

    return node;
}; 

/*
 * newNode_whileStmt - Creates a new AST node that holds
 * a while loop statement.
 */
astnode *newNode_whileStmt(astnode *expr, astnode *stmt) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    node->nodetype = WHILE_STMT;
    node->while_stmt.expr = expr;
    node->while_stmt.stmt = stmt;

    return node;
}


/*
 * newNode_doWhileStmt - Creates a new AST node that holds
 * a do-while loop statment.
 */
astnode *newNode_doWhileStmt(astnode *expr, astnode *stmt) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    node->nodetype = DO_WHILE_STMT;
    node->while_stmt.expr = expr;
    node->while_stmt.stmt = stmt;

    return node;
}

/*
 * newNode_forLoop - Creates a new AST node that holds
 * a for loop statment.
 */
astnode *newNode_forLoop() {
    astnode *node;
    if ((node = calloc(1, sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    node->nodetype = FOR_STMT;

    return node;
}   

/*
 * newNode_switch - Creates a new AST node that holds
 * a switch statment.
 */
astnode *newNode_switch(astnode *expr, astnode *stmt) {
    astnode *node;
    if ((node = malloc(sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }

    node->nodetype = SWITCH_STMT;
    node->switch_stmt.expr = expr;
    node->switch_stmt.stmt = stmt;
    node->switch_stmt.labels = NULL;

    return node;
}

/*
 * newNode_flowControl - Creates a new AST node that holds
 * a flow control statment (break or continue statement).
 */
astnode *newNode_flowControl() {
    astnode *node;
    if ((node = calloc(1, sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    return node;
}

/*
 * newNode_returnStmt - Creates a new AST node that holds
 * a return statement.
 */
astnode *newNode_returnStmt() {
    astnode *node;
    if ((node = calloc(1, sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = RETURN_STMT;
    return node;
}

/*
 * newNode_gotoStmt - Creates a new AST node that holds
 * a goto statement.
 */
astnode *newNode_gotoStmt() {
    astnode *node;
    if ((node = calloc(1, sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    node->nodetype = GOTO_STMT;
    return node;
}


/*
 * newNode_compoundStmt - Creates a new AST node that holds
 * a compound statnement.
 */
astnode *newNode_compoundStmt() {
    astnode *node;
    if ((node = calloc(1, sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    
    node->nodetype = COMPOUND_STMT;

    node->compound_stmt.scope_layer = NULL;
    node->compound_stmt.astnode_ll = NULL;
    
    return node;
}


/*
 * newNode_labelHack - creates an astnode for the hackish solution
 * of having the label statement point no to a stable_entry, but
 * rather to a node in the linked list of astnodes, so that we could 
 * continue the statement flow inside a compound statement from this label
 * instead of only having access to a single statement.
 */
astnode *newNode_labelHack(struct AstnodeLinkedListNode *ll_node) {
    astnode *node;
    if ((node = calloc(1, sizeof(astnode))) == NULL) {
        fprintf(stderr, "Error allocating memory for AST node: %s\n", 
                                                    strerror(errno));
        exit(-1);
    }
    
    node->nodetype = LABEL_DEREF_HACK;
    node->label_deref_hack.ptr = ll_node;
    return node;   
}


/*
 * newNode_sTableEntry - creates a new AST node
 * as a symbol table entry. There are several different
 * versions of this AST node, and will be decided 
 * based on the inputted enum.
 * 
 * This function gets called only after an error checking 
 * function called 'is_tmp_STentry_correct' is called on 
 * the temporary symbol table entry. This function is 
 * defined in 'symbol_table.h'.
 */
astnode *newNode_sTableEntry(TmpSymbolTableEntry *tmp_entry) {
    
    astnode *new_entry = calloc(1, sizeof(astnode));
    if (!new_entry) {
        fprintf(stderr, "Unable to allocate memory for "
                "a new Symbol Table Entry: %s\n", strerror(errno));
    }

    if (!tmp_entry) {  /* applies to struct declarations */
        new_entry->nodetype = IDENT_TYPE;
        return new_entry;
    }
    new_entry->stable_entry.file_name = tmp_entry->file_name;
    new_entry->stable_entry.line_num = tmp_entry->line_num;
    new_entry->stable_entry.type = tmp_entry->type;
    new_entry->stable_entry.node = tmp_entry->node;

    astnode *tmp = tmp_entry->node;
    switch(tmp_entry->type) {
        case Variable_Type:
            new_entry->nodetype = STABLE_VAR;
            new_entry->stable_entry.var.storage_class = tmp_entry->var_fnc_storage_class;
            new_entry->stable_entry.var.type_qualifier = tmp_entry->var_type_qualifier;
            new_entry->stable_entry.var.offset_within_stack_frame = tmp_entry->var_offset_within_stack_frame;
            break;
        case Function_Type:
            new_entry->nodetype = STABLE_FNC_DECLARATOR;
            new_entry->stable_entry.fnc.storage_class = tmp_entry->var_fnc_storage_class;
            new_entry->stable_entry.fnc.is_inline = tmp_entry->fnc_is_inline;
            new_entry->stable_entry.fnc.return_type = tmp_entry->fnc_return_type;
            new_entry->stable_entry.fnc.args_types = tmp_entry->fnc_args_type;
            new_entry->stable_entry.fnc.function_body = NULL;
            break;
        case S_Tag_Type:
        case U_Tag_Type:
            new_entry->nodetype = STABLE_SU_TAG;
            new_entry->stable_entry.sutag.is_defined = tmp_entry->su_tag_is_defined;
            new_entry->stable_entry.sutag.su_table = sTableCreate();
            break;
        case Enum_Tag:
            new_entry->nodetype = STABLE_ENUM_TAG;
            new_entry->stable_entry.enumtag.is_defined = tmp_entry->enum_tag_is_defined;
            break;
        case Statement_Label:
            new_entry->nodetype = STABLE_STMT_LABEL;
            new_entry->stable_entry.stmtlabel.IR_assembly_label = tmp_entry->stmt_IR_assembly_label;
            new_entry->stable_entry.stmtlabel.label_type = tmp_entry->stmt_label_type;
            new_entry->stable_entry.stmtlabel.case_label_value = tmp_entry->stmt_case_label_value;
            break;
        case Enum_Const_Type:
            new_entry->nodetype = STABLE_ENUM_CONST;
            new_entry->stable_entry.enumconst.tag = tmp_entry->enum_parent_tag;
            new_entry->stable_entry.enumconst.val = tmp_entry->enum_const_val;
            break;
        case Typedef_Name:
            new_entry->nodetype = STABLE_TYPEDEF;
            new_entry->stable_entry.typedef_name.equivalent_type = tmp_entry->typedef_type;
            break;
        case SU_Member_Type:
            new_entry->nodetype = STABLE_SU_MEMB;
            new_entry->stable_entry.sumemb.type = tmp_entry->su_memb_type;
            new_entry->stable_entry.sumemb.offset_within_s_u = tmp_entry->su_memb_offset;
            new_entry->stable_entry.sumemb.bit_field_width = tmp_entry->su_memb_bit_field_width;
            new_entry->stable_entry.sumemb.bit_offset = tmp_entry->su_memb_bit_offset;
            break;
    }

    return new_entry;

}


/* 
 * The constructor for the astnode_list struct.
 * This struct will be used for a declarator list. 
 */
astnode_list *newASTnodeList(int len, astnode_list *cur_list) {
    astnode_list *new_list = malloc(sizeof(astnode_list));
    if (!new_list) {
        fprintf(stderr, "Error allocating memory for astnode list: %s\n", strerror(errno));
        exit(-1);
    }

    new_list->list = malloc(len*sizeof(astnode *));
    if (!new_list->list) {
        fprintf(stderr, "Error allocating memory for astnode list: %s\n", strerror(errno));
        exit(-1);
    }

    if (cur_list)
        for (int i = 0 ; i < cur_list->len ; ++i)
            new_list->list[i] = cur_list->list[i];   

    new_list->len = len;
    return new_list;
}


/* 
 * The constructor for the AstnodeLinkedListNode struct.
 * This struct will be used for a list of statements, ie
 * a compound statement. 
 */
AstnodeLinkedListNode *newASTnodeLinkedListNode(astnode *node) {
    AstnodeLinkedListNode *new_ll_node = malloc(sizeof(AstnodeLinkedListNode));
    if (!new_ll_node) {
        fprintf(stderr, "Error allocating memory for astnode list: %s\n", strerror(errno));
        exit(-1);
    }

    new_ll_node->node = node;
    new_ll_node->next = NULL;

    return new_ll_node;
}


/*
 * The constructor for the AstnodeLinkedList struct.
 * This struct will be used for a list of statements, ie
 * a compound statement. 
 */
AstnodeLinkedList *newASTnodeLinkedList(astnode *node) {
    AstnodeLinkedList *new_ll = malloc(sizeof(AstnodeLinkedList));
    if (!new_ll) {
        fprintf(stderr, "Error allocating memory for astnode list: %s\n", strerror(errno));
        exit(-1);
    }

    new_ll->first = newASTnodeLinkedListNode(node);
    new_ll->last = new_ll->first;

    return new_ll;
};


/*
 * addASTnodeLinkedList - Adds an astnode containing a statement 
 * into an ASTnodeLinkedList struct.
 * 
 * Note: no need to save declarations into astnode lists - their
 * usefullness is in the symbol table, which was updated earlier.  
 */
void addASTnodeLinkedList(AstnodeLinkedList *ll, astnode *node) {
    /* some basic error checking */
    if(!node)
        yyerror("Can't add NULL node to astnode linked list");
    else if (!ll)
        yyerror("Can't add node to a NULL astnode linked list");
    else if (node->nodetype == STABLE_ENUM_CONST || node->nodetype == STABLE_ENUM_TAG ||
                node->nodetype == STABLE_FNC_DECLARATOR ||
                node->nodetype == STABLE_SU_MEMB || node->nodetype == STABLE_SU_TAG ||
                node->nodetype == STABLE_TYPEDEF || node->nodetype == STABLE_VAR)
        return;

    ll->last->next = newASTnodeLinkedListNode(node);
    ll->last = ll->last->next;
}

//////////////////////////////////////////////////////////
///////////// Abstract Syntax Tree Functions /////////////
//////////////////////////////////////////////////////////


/*
 * token2op - Takes the token name of an operator and 
 * returns how the operator is printed in Hakne'r AST example.
 * 
 * Ex: token name 'DIVEQ' will return the string "/=", while
 * "++" will return "POSTINC". Use this function as a reference
 * as to what should return what.
 */
char *token2op(int token_name) {
    switch(token_name) {
        case DIVEQ:
            return "/=";
        case MODEQ:
            return "%=";
        case SHLEQ:
            return "<<=";
        case SHREQ:
            return ">>=";
        case ANDEQ:
            return "&=";
        case XOREQ:
            return "^=";
        case OREQ:
            return "|=";
        case INDSEL:
            return "->";
        case SHL:
            return "<<";
        case SHR:
            return ">>";
        case LTEQ:
            return "<=";
        case GTEQ:
            return ">=";
        case EQEQ:
            return "==";
        case NOTEQ:
            return "!=";
        case LOGAND:
            return "&&";
        case LOGOR:
            return "||";
        case PLUSEQ:
            return "+=";
        case MINUSEQ:
            return "-=";
        case TIMESEQ:
            return "*=";
        case PLUSPLUS:
            /* This will only appear in postincrements, b/c 
               preincrement "++" will get reduced to "+= 1".   */
            return "POSTINC"; 
        case MINUSMINUS:
            return "--";
    }
}



/*
 * printAST - Given the root node of an Abstract Syntax Tree,
 * this function prints out the tree in a format matching 
 * Hakner's in the assignment sheet.
 * 
 * This is in essense a preorder traversal.
 * 
 * If the output_file input is NULL, then the AST is printed
 * to standard out.
 */
void printAST(astnode *root, FILE *output_file) {

    FILE *output = (output_file) ? output_file : stdout;
    preorderTraversal(root, output, 0);
    fprintf(output, "\n");  /* keeping consistent with Hakner's format */
}


/*
 * printStructAST - A quick way to specify (print out) the
 * members of a struct or union. This is only called when
 * a struct or union are first defined.
 */
void printStructAST(astnode *root, FILE *output_file, int depth) {

    FILE *output = (output_file) ? output_file : stdout;

    for (int i = 0 ; i < depth ; ++i)
        fprintf(output, "    ");

    if (root->stable_entry.type == S_Tag_Type) { /* struct type */
        fprintf(output, "struct %s definition at %s:%d{\n", 
                            root->stable_entry.ident, 
                            root->stable_entry.file_name, 
                            root->stable_entry.line_num);
    }
    else {                                      /* union type */
        fprintf(output, "union %s definition at %s:%d {\n", 
                            root->stable_entry.ident, 
                            root->stable_entry.file_name, 
                            root->stable_entry.line_num);
    }
    for (int i = 0 ; i < root->stable_entry.sutag.su_table->size; ++i) {
        if (root->stable_entry.sutag.su_table->data[i])
            preorderTraversal(root->stable_entry.sutag.su_table->data[i], output, depth+1);
    }
    
    fprintf(output, "}\n");

    fprintf(output, "\n");  /* keeping consistent with Hakner's format */
}


/*
 * preorderTraversal - A helper function for printAST. This
 * function implements preorder traversal for the AST printing.
 */
void preorderTraversal(astnode *cur, FILE *output, int depth) {
    if (cur == NULL)    /* for error checking, might be beneficial to throw error here */
        return;

    /* format the tab spacing correctly */
    for (int i = 0; i < depth; ++i)
        fprintf(output, "    ");


    AstnodeLinkedListNode *astnode_ll_iter; /* used in COMPOUND_STMT */
    switch(cur->nodetype) {
        case IDENT_TYPE:
            fprintf(output, "UNKNOWN IDENT  %s\n", cur->stable_entry.ident);
            if (cur->stable_entry.node)
                preorderTraversal(cur->stable_entry.node, output, depth+1);
            break;
        case NUM_TYPE:
            if (cur->num.types & NUMMASK_INTGR)
                fprintf(output, "CONSTANT:  (type=int)%lld\n", cur->num.val);
            else if (cur->num.types & NUMMASK_DOUBLE)
                fprintf(output, "CONSTANT:  (type=double)%g\n", cur->num.d_val);
            else
                fprintf(output, "CONSTANT:  (type=float)%g\n", cur->num.d_val);
            break;
        case CHRLIT_TYPE:
            fprintf(output, "CONSTANT:  (type=int)%d\n", cur->chrlit.c_val);
            break;
        case STRLIT_TYPE:
            fprintf(output, "STRING\t%s\n", cur->strlit.str);
            break;
        case BINOP_TYPE:
            if (cur->binop.op < 258)
                fprintf(output, "BINARY  OP  %c\n", cur->binop.op);
            else
                fprintf(output, "BINARY  OP  %s\n", token2op(cur->binop.op));
            preorderTraversal(cur->binop.left, output, depth+1);
            preorderTraversal(cur->binop.right, output, depth+1);
            break;
        case COMPARE_TYPE:
            if (cur->binop.op < 258)
                fprintf(output, "COMPARISON  OP  %c\n", cur->binop.op);
            else
                fprintf(output, "COMPARISON  OP  %s\n", token2op(cur->binop.op));
            preorderTraversal(cur->binop.left, output, depth+1);
            preorderTraversal(cur->binop.right, output, depth+1);
            break;
        case LOG_TYPE:
            fprintf(output, "LOGICAL  OP  %s\n", token2op(cur->binop.op));
            preorderTraversal(cur->binop.left, output, depth+1);
            preorderTraversal(cur->binop.right, output, depth+1);            break;
        case UNOP_TYPE:
            if (cur->binop.op < 258)
                fprintf(output, "UNARY  OP  %c\n", cur->unop.op);
            else
                fprintf(output, "UNARY  OP  %s\n", token2op(cur->unop.op));
            preorderTraversal(cur->unop.expr, output, depth+1);
            break;
        case ADDR_TYPE:
            fprintf(output, "ADDRESSOF\n");
            preorderTraversal(cur->unop.expr, output, depth+1);
            break;
        case DEREF_TYPE:
            fprintf(output, "DEREF\n");
            preorderTraversal(cur->unop.expr, output, depth+1);
            break;
        case SIZEOF_TYPE:
            fprintf(output, "SIZEOF\n");
            preorderTraversal(cur->unop.expr, output, depth+1);
            break;
        case FNC_CALL:
            fprintf(output, "FNCALL, %d  arguments\n", cur->fnc.arg_count);

            if (cur->fnc.ident->nodetype == IDENT_TYPE)
                preorderTraversal(cur->fnc.ident, output, depth+1);
            else {
                for (int i = 0; i < depth+1; ++i)
                    fprintf(output, "    ");

                if (cur->fnc.ident->nodetype == STABLE_FNC_DECLARATOR) {
                    fprintf(output, "stab_fn name=%s declared @<%s>:%d\n", 
                        cur->fnc.ident->stable_entry.ident, 
                        cur->fnc.ident->stable_entry.file_name,
                        cur->fnc.ident->stable_entry.line_num
                    );
                }
                else { /* nodetype == STABLE_FUN_DEFINITION */
                    fprintf(output, "stab_fn name=%s defined @<%s>:%d\n", 
                        cur->fnc.ident->stable_entry.ident, 
                        cur->fnc.ident->stable_entry.file_name,
                        cur->fnc.ident->stable_entry.line_num
                    );
                }
            }
            for (int i = 0 ; i < cur->fnc.arg_count ; ++i) {
                    preorderTraversal(cur->fnc.arguments[i], output, depth+1);
            }
            break;
        case ARG_TYPE:
            fprintf(output, "arg  #%d=\n", cur->arg.num);
            preorderTraversal(cur->arg.expr, output, depth+1);
            break;
        case SLCT_TYPE:
            fprintf(output, "SELECT\n");
            preorderTraversal(cur->slct.left, output, depth+1);
            preorderTraversal(cur->slct.right, output, depth+1);
            break;
        case TERNARY_TYPE:
            fprintf(output, "TERNARY  OP,  IF:\n");
            preorderTraversal(cur->ternary.if_expr, output, depth+1);
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");
            fprintf(output, "THEN:\n");
            preorderTraversal(cur->ternary.then_expr, output, depth+1);
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");
            fprintf(output, "ELSE:\n");
            preorderTraversal(cur->ternary.else_expr, output, depth+1);
            break;
        case ASS_TYPE:
            fprintf(output, "ASSIGNMENT\n");
            preorderTraversal(cur->assignment.left, output, depth+1);
            preorderTraversal(cur->assignment.right, output, depth+1);
            break;
        case PTR_TYPE:
            if (strcmp("", translateTypeQualifier(cur->ptr.type_qualifier)))
                fprintf(output, "%s pointer to:\n", translateTypeQualifier(cur->ptr.type_qualifier));
            else
                fprintf(output, "pointer to:\n");

            preorderTraversal(cur->ptr.pointee, output, depth+2);
            break;
        case ARRAY_TYPE:
            fprintf(output, "array of %d elements of type\n", cur->arr.size);
            preorderTraversal(cur->arr.ptr->ptr.pointee, output, depth+2);
            break;
        case SCALAR_TYPE:
            if (!cur->scalar_type.sign)
                fprintf(output, "unsigned ");
            switch(cur->scalar_type.type) {
                case Int: fprintf(output, "int\n");                break;
                case Void: fprintf(output, "void\n");              break;
                case Char: fprintf(output, "char\n");              break;
                case Short: fprintf(output, "short\n");            break;
                case Long: fprintf(output, "long\n");              break;
                case LongLong: fprintf(output, "long long\n");     break;
                case Bool: fprintf(output, "bool\n");              break;
                case Float: fprintf(output, "float\n");            break;
                case Double: fprintf(output, "double\n");          break;
                case LongDouble: fprintf(output, "long double\n"); break;
                case FloatComplex: fprintf(output, "float complex\n");            break;
                case DoubleComplex: fprintf(output, "double complex\n");          break;
                case LongDoubleComplex: fprintf(output, "long double complex\n"); break;
                case FloatImag: fprintf(output, "float imaginary\n");             break;
                case DoubleImag: fprintf(output, "double imaginary\n");           break;
                case LongDoubleImag: fprintf(output, "long double imaginary\n");  break;
            }
            break;
        case FNC_TYPE:
            fprintf(output, "function returning:\n");
            preorderTraversal(cur->fnc_type.return_type, output, depth+1);

            for (int i = 0 ; i < depth; ++i)
                fprintf(output, "    ");

            if (cur->fnc_type.arg_count > 0) {
                fprintf(output, "and taking the following arguments:");

                for (int i = 0 ; i < cur->stable_entry.node->fnc_type.arg_count; ++i)
                    preorderTraversal(cur->stable_entry.node->fnc_type.args_types[i], output, depth+2);
            }
            else {
                fprintf(output, "and taking an unspecified number of arguments.\n");
            }
            break;

        /* symbol table entries */
        case STABLE_VAR:
        case STABLE_ENUM_TAG:
        case STABLE_SU_MEMB:
            if (print_level == Verbose_Level) {

                fprintf( output, 
                    "%s is defined at %s:%d [in %s scope starting at %s:%d] "
                    "as a \n", 
                    cur->stable_entry.ident, 
                    cur_file_name, 
                    cur_line_num, 
                    translateScopeType(scope_stack.innermost_scope->scope_type),
                    scope_stack.innermost_scope->beginning_file, 
                    scope_stack.innermost_scope->begin_line_num
                );
                    
                for (int i = 0 ; i < depth; ++i)
                    fprintf(output, "    ");

                fprintf( output, 
                    "variable with stgclass %s of type:\n", 
                    translateStgClass(cur->stable_entry.var.storage_class)
                ); 
                
                for (int i = 0; i < depth+1; ++i)
                    fprintf(output, "    ");

                fprintf(output, "%s", translateTypeQualifier(cur->stable_entry.var.type_qualifier));
                preorderTraversal(cur->stable_entry.node, output, depth+1);
            }
            else {

                fprintf(output, "stab_var name=%s def @<%s>:%d\n", 
                    cur->stable_entry.ident,
                    cur->stable_entry.file_name, 
                    cur->stable_entry.line_num);
            }
            break;
        case STABLE_FNC_DECLARATOR:
            if (print_level == Verbose_Level) {
                fprintf( output, 
                    "%s is defined at %s:%d [in %s scope starting at %s:%d] "
                    "as a \n", 
                    cur->stable_entry.ident, 
                    cur_file_name, 
                    cur_line_num, 
                    translateScopeType(scope_stack.innermost_scope->scope_type),
                    scope_stack.innermost_scope->beginning_file, 
                    scope_stack.innermost_scope->begin_line_num
                );
                    
                for (int i = 0 ; i < depth; ++i)
                    fprintf(output, "    ");

                fprintf( output, 
                    "%s function with stgclass of type:\n", 
                    translateStgClass(cur->stable_entry.var.storage_class)
                );

                preorderTraversal(cur->stable_entry.fnc.return_type, output, depth+1);

                for (int i = 0 ; i < depth; ++i)
                    fprintf(output, "    ");

                fprintf(output, "and taking an unspecified number of arguments.\n");
            }
            else {
                fprintf(output, "stab_fn name=%s declared @<%s>:%d\n", 
                    cur->stable_entry.ident, 
                    cur->stable_entry.file_name,
                    cur->stable_entry.line_num
                );
            }
            break;
        case STABLE_SU_TAG:
            if (!cur->stable_entry.ident) {
                printStructAST(cur, output, depth +1);
                break;
            }
            if (!cur->stable_entry.sutag.is_defined) {
                fprintf( output, 
                    "incomplete struct %s is defined at %s:%d [in %s scope starting at %s:%d].\n", 
                    cur->stable_entry.ident, 
                    cur_file_name, 
                    cur_line_num, 
                    translateScopeType(scope_stack.innermost_scope->scope_type),
                    scope_stack.innermost_scope->beginning_file, 
                    scope_stack.innermost_scope->begin_line_num
                );               
                break;
            }

            if (cur->stable_entry.type == S_Tag_Type) { /* struct type */
                fprintf(output, "struct %s, defined at %s:%d.\n", 
                                    cur->stable_entry.ident, 
                                    cur->stable_entry.file_name, 
                                    cur->stable_entry.line_num);
            }
            else {                                      /* union type */
                fprintf(output, "union %s, defined at %s:%d.\n", 
                                    cur->stable_entry.ident, 
                                    cur->stable_entry.file_name, 
                                    cur->stable_entry.line_num);
            }
            if (print_level == Verbose_Level) {
                printStructAST(cur, output, depth+1);    
            }
            
            break;
        case STABLE_STMT_LABEL:
            switch(cur->stable_entry.stmtlabel.label_type) {
                case NAMED_LABEL:
                    fprintf(output, "LABEL(%s):\n", cur->stable_entry.ident);
                    break;
                case CASE_LABEL:
                    fprintf(output, "CASE, EXPR:\n");
                    for (int i = 0 ; i < depth+1; ++i)
                        fprintf(output, "    ");
                    fprintf(output, "CONSTANT: (type=int)%d\n", cur->stable_entry.stmtlabel.case_label_value);
                    
                    for (int i = 0 ; i < depth; ++i)
                        fprintf(output, "    ");
                    fprintf(output, "STMT:\n");
                    break;
                case DEFAULT_LABEL:
                    fprintf(output, "DEFAULT LABEL:\n");
                    break;
            }
            break;
        case CONDITIONAL_STMT:
            fprintf(output, "IF:\n");
            preorderTraversal(cur->conditional_stmt.expr, output, depth+1);
                
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");
            fprintf(output, "THEN:\n");
            preorderTraversal(cur->conditional_stmt.if_node, output, depth+1);
            
            if (cur->conditional_stmt.else_node) {
                for (int i = 0; i < depth; ++i)
                    fprintf(output, "    ");
                fprintf(output, "ELSE:\n");
                preorderTraversal(cur->conditional_stmt.else_node, output, depth+1);
            }
            break;
        case WHILE_STMT:
            fprintf(output, "WHILE\n");
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");

            fprintf(output, "COND:\n");
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");
            preorderTraversal(cur->while_stmt.expr, output, depth+1);

            fprintf(output, "BODY:\n");
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");
            preorderTraversal(cur->while_stmt.stmt, output, depth+1);

            break;
        case DO_WHILE_STMT:
            fprintf(output, "DO-WHILE\n");
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");

            fprintf(output, "BODY:\n");
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");
            preorderTraversal(cur->do_while_stmt.stmt, output, depth+1);
            
            fprintf(output, "COND:\n");
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");
            preorderTraversal(cur->do_while_stmt.expr, output, depth+1);

            break;
        case FOR_STMT:
            fprintf(output, "FOR\n");
            for (int i = 0; i < depth; ++i)
                fprintf(output, "    ");

            fprintf(output, "INIT:\n");

            for (int i = 0 ; i < cur->for_stmt.initial_clause->len; ++i)
                preorderTraversal(cur->for_stmt.initial_clause->list[i], output, depth+1);

            for(int i = 0 ; i < depth ; ++i)
                fprintf(output, "    ");
            fprintf(output, "COND:\n");

            preorderTraversal(cur->for_stmt.check_expr, output, depth+1);

            for(int i = 0 ; i < depth ; ++i)
                fprintf(output, "    ");
            fprintf(output, "BODY:\n");

            preorderTraversal(cur->for_stmt.stmt, output, depth+1);

            for(int i = 0 ; i < depth ; ++i)
                fprintf(output, "    ");
            fprintf(output, "INCR:\n");

            preorderTraversal(cur->for_stmt.iteration_expr, output, depth+1);

            break;
        case SWITCH_STMT:
            fprintf(output, "SWITCH, EXPR:\n");
            preorderTraversal(cur->switch_stmt.expr, output, depth+1);

            for (int i = 0 ; i < depth; ++i)
                fprintf(output, "    ");
            fprintf(output, "BODY:\n");
            
            preorderTraversal(cur->switch_stmt.stmt, output, depth+1);

            break;
        case BREAK_STMT:
            fprintf(output, "BREAK\n");
            break;
        case CONTINUE_STMT:
            fprintf(output, "CONTINUE\n");
            break;
        case NULL_STMT:
            fprintf(output, "NULL STATEMENT\n");
            break;
        case RETURN_STMT:
            fprintf(output, "RETURN\n");
            break;
        case GOTO_STMT:
            if (cur->goto_stmt.label_stmt->stable_entry.ident)
                fprintf(output, "GOTO %s\n", cur->goto_stmt.label_stmt->stable_entry.ident);
            else
                fprintf(output, "GOTO: <undefined>\n");
            break;
        case STABLE_FNC_DEFINITION:
            if (print_level == Verbose_Level) {
                fprintf( output, 
                    "%s is defined at %s:%d [in %s scope starting at %s:%d] "
                    "as a \n", 
                    cur->stable_entry.ident, 
                    cur_file_name, 
                    cur_line_num, 
                    translateScopeType(scope_stack.innermost_scope->scope_type),
                    scope_stack.innermost_scope->beginning_file, 
                    scope_stack.innermost_scope->begin_line_num
                );
                    
                for (int i = 0 ; i < depth; ++i)
                    fprintf(output, "    ");

                fprintf( output, 
                    "%s function with stgclass of type:\n", 
                    translateStgClass(cur->stable_entry.var.storage_class)
                );
            }
            else if (print_level == Mid_Level) {
                fprintf(output, "stab_fn name=%s declared @<%s>:%d\n", 
                    cur->stable_entry.ident, 
                    cur->stable_entry.file_name,
                    cur->stable_entry.line_num
                );
            }

            fprintf(output, "AST Dump for function called %s:\n", cur->stable_entry.ident);
            preorderTraversal(cur->stable_entry.fnc.function_body, output, depth+1);
            break;
        case COMPOUND_STMT:
            fprintf(output, "LIST {\n");

            astnode_ll_iter = cur->compound_stmt.astnode_ll->first;
            do {
                preorderTraversal(astnode_ll_iter->node, output, depth+1);            
            } while ( (astnode_ll_iter = astnode_ll_iter->next));
            for (int i = 0 ; i < depth ; ++i)
                fprintf(output, "    ");
            fprintf(output, "}\n");
            break;
        /* if I ever get to these: */
        case STABLE_ENUM_CONST:
            break;
        case STABLE_TYPEDEF:
            break;

    }
}




/*
 * translateStgclass - A helper function for gettint the correct
 * printing format for variable storage classes.s
 */
char *translateStgClass(enum SymbolTableStorageClass stgclass) {
    switch (stgclass) {
        case Auto:      return "auto ";      break;
        case Extern:    return "extern ";    break;
        case Static:    return "static ";    break;
        case Register:  return "register ";  break;
        default:        return "typedef ";
    }
}


/*
 * translateTypeQualifier - A helper function for gettint the correct
 * printing format for variable storage classes.s
 */
char *translateTypeQualifier(enum SymbolTableTypeQualifiers qualifier) {
    switch (qualifier) {
        case C:     return "const ";                   break;
        case V:     return "volatile ";                break;
        case R:     return "restrict ";                break;
        case CV:    return "const volatile ";          break;
        case CR:    return "const restrict ";          break;
        case VR:    return "volatile restrict ";       break;
        case CVR:   return "const volatile restrict "; break;
        default:    return ""; 
    }
}

/*
 * translateScopeType - A helper function for gettint the correct
 * printing format for variable storage classes.s
 */
char *translateScopeType(enum ScopeType type) {
    switch(type) {
        case File:          return "global";    break;
        case Function:      return "function";  break;
        case Block:         return "block";     break;
        default: Proto:     return "prototype"; break;
    }
}

/* 
 * freeTree - Frees the dynamically allocated memory 
 * that was used by the Abstract Syntax Tree.
 *
 * This function does not return anything.
 */
void freeTree(astnode *root) {
    if (root == NULL)
        return;
    
    /* node will only have children if it is an operator */
    if (root->nodetype == BINOP_TYPE) {
        freeTree(root->binop.left);
        freeTree(root->binop.right);
    }
    else if (root->nodetype == FNC_CALL) {
        freeTree(root->fnc.ident);
        for (int i = 0; i < root->fnc.arg_count ; ++i)
            freeTree(root->fnc.arguments[i]);
    }
    else if (root->nodetype == UNOP_TYPE)
        freeTree(root->unop.expr);
    else if (root->nodetype == PTR_TYPE)
        freeTree(root->ptr.pointee);
    else if (root->nodetype == ARRAY_TYPE)
        freeTree(root->arr.ptr);
    else if (root->nodetype == FNC_TYPE) {
        freeTree(root->fnc_type.return_type);
        for (int i = 0 ; i < root->fnc_type.arg_count ; ++i)
            freeTree(root->fnc_type.args_types[i]);
    }

    free(root);
}