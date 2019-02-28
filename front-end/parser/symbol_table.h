/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * symbol_table.h - A header file for a symbol table. 
 * 
 * Data structures include:
 *  -   Scopes Stack
 *  -   Symbol Table
 *  -   Symbol Table Entry
 * 
 * Functions includes:
 *  -   Create a new (& empty) symbol table.
 *  -   Destroy an existing symbol table.
 *  -   Lookup a symbol table entry inside of a symbol table.
 *  -   Insert a symbol table entry into a symbol table. 
 */


#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE


#include "pheader_ast.h"


struct SymbolTable; /* Incomplete definition. Defined fully after SymbolTableEntry. */


/* There are 10 different uses for identifiers in the C language, most
 * of which require different attributes to be kept by the compiler.
 * This will be implemented through an anonymous union. 
 * 
 * The 10 identifier purposes are: variable name, function name, 
 * typedef name, enum constant, struct tag, union tag, enum tag, label, 
 * struct member, and union member.
 */
enum SymbolTableStorageClass { Auto = 1, Register, Extern, Static};
enum possibleTypeQualifiers { Const, Volatile, Restrict};
enum SymbolTableTypeQualifiers { None = 0, C, V, R, CV, CR, VR, CVR};


enum STEntry_Type { NO_TYPE = 0, VARIABLE_TYPE, FUNCTION_TYPE, SU_TAG_TYPE, ENUM_TAG, STATEMENT_LABEL, 
                    ENUM_CONST_TYPE, TYPEDEF_NAME, SU_MEMBER_TYPE};

typedef struct SymbolTableEntry {
    char *file_name;        /* file name where the identifier first appeared  */
    int line_num;           /* line number of file where identifier appeared  */
    char *ident;            /* identifier name */
    enum STEntry_Type type;     /* the type of identifier that the entry holds    */
    struct astnode *node;   /* the AST node that represents this entry    */

    union { /* anonymous union for the different identifier uses */
        struct variable {
            enum SymbolTableStorageClass storage_class;
            enum SymbolTableTypeQualifiers type_qualifier;
            int offset_within_stack_frame;
        } variable;
        struct function {
            enum SymbolTableStorageClass storage_class;
            int is_inline;
            int is_defined;
            struct astnode *return_type;
            struct astnode **args_types;
        } function;
        struct s_u_tag {
            int is_defined;
            struct SymbolTable *s_u_table;
        } s_u_tag;
        struct enum_tag {
            int is_defined;
        } enum_tag;
        struct statement_label {
            int IR_assembly_label;
        } statement_label;
        struct enum_const {
            struct SymbolTableEntry *tag;
            int val;
        } enum_const;
        struct typedef_name {
            struct astnode *equivalent_type;
        } typedef_name;
        struct s_u_member {
            struct astnode *type;
            int offset_within_s_u;
            int bit_field_width;
            int bit_offset;
        } s_u_member;
    };
} SymbolTableEntry;





/* To allow for efficient symbol table lookups, the symbol table will be
   built as a hash table. Therefore we will build the infrastructure to
   implement all the necessary hash table operations.   */
#define SYMBOL_TABLE_INIT_SIZE 5

typedef struct SymbolTable {
    int size;                   /* number of spaces for enties  */
    int filled;                 /* number of entries in table   */
    SymbolTableEntry **data;    /* hash table (pointer to an array of SymbolTableEntries) */
} SymbolTable;


/* Each scope has 4 distinct symbol tables, corresponding to the
   4 possible namespaces. The namespaces are, in the order inside
   the array:
    1. Labels.
    2. Tags - The identifiers of a struct, union or enum.
    3. Struct/union members.
    4. All other identifier classes.
 */
enum ScopeType { File = 1, Function, Block, Proto };

typedef struct ScopeStackLayer {
    enum ScopeType scope_type;  /* one of the 4 types of scopes     */
    SymbolTable *tables[4];     /* diff table for each namespace    */
    struct ScopeStackLayer *parent;    /* the scope layer above this layer */ 
} ScopeStackLayer;


/* contains a linked list of scopes, begining with the innermost scope. */
typedef struct ScopeStack {
    ScopeStackLayer *innermost_scope;
} ScopeStack;


typedef struct TmpSymbolTableEntry {
    char *file_name;        /* file name where the identifier first appeared  */
    int line_num;           /* line number of file where identifier appeared  */
    char *ident;            /* identifier name */
    enum STEntry_Type type;/* the type of identifier that the entry holds    */
    struct astnode *node;   /* the AST node that represents this entry    */

    /* variable and function */
    enum SymbolTableStorageClass var_fnc_storage_class;

    /* variable */
    enum SymbolTableTypeQualifiers var_type_qualifier;
    int var_offset_within_stack_frame;

    /* function */
    _Bool fnc_is_inline;
    _Bool fnc_is_defined;
    struct astnode *fnc_return_type;
    struct astnode **fnc_args_type;

    /* struct/union tag */
    int su_tag_is_defined;
    struct SymbolTable *su_tag_su_table;

    /* enum tag */
    int enum_tag_is_defined;

    /* enum const */
    struct SymbolTableEntry *enum_parent_tag;
    int enum_const_val;

    /* stmt label */
    int stmt_IR_assembly_label;

    /* typedef - needs only equivalent type */
    struct astnode *typedef_type;

    /* struct/union members */
    struct astnode *su_memb_type;
    int su_memb_offset;
    int su_memb_bit_field_width;
    int su_memb_bit_offset;
} TmpSymbolTableEntry;


/////////////////////////////////////////////////////////////////////
///////////////////////////// FUNCTIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////

/*
 * symbol_table_create - Allocates memory for a new, empty symbol
 * table with the size specified in the macro SYMBOL_TABLE_INIT_SIZE.
 */
SymbolTable *symbol_table_create();


/*
 * symbol_table_destroy - Given a symbol table, this function 
 * frees all its associated memory and return a 1 on sucess and
 * a -1 on error.
 */
void symbol_table_destroy(SymbolTable *table);


/*
 * symbol_table_insert - Given a symbol table and a symbol table,
 * this function inserts the entry into the table and returns a 1
 * on sucess and a -1 on error.
 */
int symbol_table_insert(SymbolTable *table, SymbolTableEntry *entry, 
                        int dup_toggle);


/*
 * symbol_table_lookup - Given a symbol table and a symbol table entry, this
 * function returns a pointer to the symbol table entry that represents this
 * identifier. If no such entry is found in any scope, NULL is returned.
 */
SymbolTableEntry *symbol_table_lookup(SymbolTable *table, char *entry);


/*
 * symbol_table_resize - Resizes a symbol table by increasing its size 
 * to the next predetermined hash table size.
 */
int symbol_table_resize(SymbolTable *table);


/*
 * symbol_table_hash - Produces the hash of an identifier.
 */
int symbol_table_hash(char *ident, int mod);


/*
 * symbol_table_creat_tmpentry - creates a temporary symbol table
 * entry strucutre, which will serve as a temporary place holder
 * for all the different declarator specifiers, until a type can 
 * be decided on, in which case an error checking function will
 * make sure the declaration specifiers make sense for the given
 * type and then form an actual symbol table entry out of this
 * temporary structure.
 */
TmpSymbolTableEntry *symbol_table_create_tmpentry();


/*
 * update_type_qualifier_for_tmp_stable_entry - Updates the
 * type qualifier of the temporary symbol table entry.
 */
void update_type_qualifier_for_tmp_stable_entry(TmpSymbolTableEntry *entry,
                                                enum possibleTypeQualifiers qualifier);



/*
 * is_tmp_STentry_correct - Given a TmpSymbolTAbleEntry object,
 * this function determins whether the declaration specifiers
 * for the specified type are valid. 
 * 
 * Returns 1 (true) or 0(false).
 */
_Bool is_tmp_STentry_correct(TmpSymbolTableEntry *entry);


/*
 * create_new_entry - Creates a new Symbol Table Entry. Will be called
 * after checking that the TempSymbolTableEntry is valid with regards
 * to the actual identifier type.
 */
SymbolTableEntry *create_new_entry(TmpSymbolTableEntry *tmp_entry);

#endif