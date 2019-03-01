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





/* To allow for efficient symbol table lookups, the symbol table will be
   built as a hash table. Therefore we will build the infrastructure to
   implement all the necessary hash table operations.   */
#define SYMBOL_TABLE_INIT_SIZE 5

typedef struct SymbolTable {
    int size;          /* number of spaces for enties  */
    int filled;        /* number of entries in table   */
    astnode **data;    /* hash table (pointer to an array of SymbolTableEntries) */
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
    struct ScopeStackLayer *child;  /* next link in linked list of scopes */ 
} ScopeStackLayer;


/* The ScopeStack will be a singly linked list of scopes,
   beginning with the innermost scope and working outwards
   towards file (global) scope. */
typedef struct ScopeStack {
    ScopeStackLayer *innermost_scope;
} ScopeStack;


/* The TmpSymbolTableEntry struct serves as an intermediatary
   symbol table entry builder. It will hold all the declaration
   types that are specified and will then be converted into an
   actual symbol table entry (which is an abstract syntax tree
   (AST) node) using an error checking function. */
typedef struct TmpSymbolTableEntry {
    char *file_name;        /* file name where the identifier first appeared  */
    int line_num;           /* line number of file where identifier appeared  */
    char *ident;            /* identifier name */
    enum STEntry_Type type;/* the type of identifier that the entry holds    */
    astnode *node;          /* the AST node that represents this entry    */

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
    _Bool su_tag_is_defined;
    struct SymbolTable *su_tag_su_table;

    /* enum tag */
    _Bool enum_tag_is_defined;

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
int symbol_table_insert(SymbolTable *table, astnode *entry, int dup_toggle);


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


#endif