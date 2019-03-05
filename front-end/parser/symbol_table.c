/*
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * symbol_table.c - Defines the functions declared in symbol_table.h. 
 */

#include "symbol_table.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////
/////////////////////// Symbol Table Functions ///////////////////////
//////////////////////////////////////////////////////////////////////

/*
 * sTableCreate - Allocates memory for a new, empty symbol
 * table with the size specified in the macro SYMBOL_TABLE_INIT_SIZE.
 */
SymbolTable *sTableCreate() {
    SymbolTable *new_table = calloc(1, sizeof(SymbolTable));
    if (!new_table) {
        fprintf(stderr, "Error allocating memory for symbol table: %s\n",
                                                            strerror(errno));
        exit(-1);
    }

    new_table->filled = 0;
    new_table->size = 0;

    /* initialize the actual data array of the symbol table */
    if (symbol_table_resize(new_table) < 0)
        exit(-1);

    return new_table;
}


/*
 * sTableDestroy - Given a symbol table, this function 
 * frees all its associated memory and return a 1 on sucess and
 * a -1 on error.
 */
void sTableDestroy(SymbolTable *table) {
    for (int i = 0; i < table->size ; ++i ) {
        if (table->data[i])
            free(table->data[i]);
    }
    free(table->data);
    free(table);
}


/*
 * sTableInsert - Given a symbol table and a symbol table,
 * this function inserts the entry into the table and returns a 1
 * on sucess and a -1 if the entry already exists.
 * 
 * If dup_toggle is 1, then a duplication is valid (will delete 
 * old variable). Otherwise duplication is not allowed and an
 * error is thrown.
 * 
 * Initially, the symbol table will use linear probing to address
 * hash collisions.
 */
int sTableInsert(SymbolTable *table, astnode *entry, int dup_toggle) {
    
    /*  keep the symbol table at least twice the size of the 
        # of elements so that our lookups stay fast */
    if (table->filled >= table->size/2)
        symbol_table_resize(table);

    int data_ind = symbol_table_hash(entry->stable_entry.ident, table->size);

    // linear probing
    while (table->data[data_ind] && 
                table->data[data_ind]->stable_entry.ident != 
                entry->stable_entry.ident)
        data_ind = (data_ind+1) % table->size;

    if (table->data[data_ind]) { /* identifier already appears in scope & namespace */
        if (dup_toggle) {
            free(table->data[data_ind]);
            table->data[data_ind] = entry;
        }
        else {
        fprintf(stderr, "Attempted duplicate definition for identifier %s: %s\n",
                                                    entry->ident, strerror(errno));
        return -1;
        }
    }
    else {
        // Insert entry into correct place in table
        table->data[data_ind] = entry;
    }

    return 1;
}


/*
 * sTableLookUp - Given a symbol table and a symbol table entry, this
 * function returns a pointer to the symbol table entry that represents this
 * identifier. If no such entry is found in any scope, NULL is returned.
 * 
 * Initially, the symbol table will use linear probing to address
 * hash collisions.
 * */
astnode *sTableLookUp(SymbolTable *table, char *entry_name) {
    
    int data_ind = symbol_table_hash(entry_name, table->size);

    // linear probing
    while (table->data[data_ind] && 
           table->data[data_ind]->stable_entry.ident != entry_name)
        data_ind = (data_ind+1) % table->size;

    if (table->data[data_ind]->stable_entry.ident == entry_name)
        return table->data[data_ind];
    else
        return NULL;
}


/*
 * sTableResize - Resizes a symbol table by increasing its size 
 * to the next predetermined hash table size.
 * 
 * Returns 1 on sucess, -1 on error.
 */
int sTableResize(SymbolTable *table) {

    // Determine new table size
    /* hash table size options. Lets be honest, wont be larger than 3569 */
    const int primes[] = {0, 5, 11, 23, 37, 97, 199, 409, 823, 1741, 3569};
    
    int new_size = 0;
    int prime_ind;
    for (prime_ind = 0 ; prime_ind < 10 ; ++prime_ind) {
        if (table->size < primes[prime_ind]) {
            new_size = primes[prime_ind];
            break;
        }
    }

    // Allocate space for the new array
    astnode **new_data = calloc(new_size, sizeof(astnode *));
    if (!new_data) {
        fprintf(stderr, "Error expanding a symbol table: %s\n", strerror(errno));
        return -1;
    } 

    // Iterate through old data, updating new data array
    for (int i = 0 ; i < primes[prime_ind-1] ; ++i) {
        if (table->data[i]) {
            new_data[symbol_table_hash(table->data[i]->stable_entry.ident, new_size)] = table->data[i];
        }
    }

    // Free old data array and update table with the new data array
    free(table->data);
    table->data = new_data;

    return 1;
}


/*
 * sTableHash - Hashes an identifier, modulus the hashtable's capacity.
 */
int sTableHash(char *ident, int capacity) {
    int hashVal = 0;

    for (int i = 0 ;i < strlen(ident) ; ++i)
        hashVal = 37*hashVal + ident[i];

    hashVal %= capacity;
    if (hashVal < 0)
        hashVal += capacity;

    return hashVal;
}


/*
 * createTmpSTableEntry - creates a temporary symbol table
 * entry strucutre, which will serve as a temporary place holder
 * for all the different declarator specifiers, until a type can 
 * be decided on, in which case an error checking function will
 * make sure the declaration specifiers make sense for the given
 * type and then form an actual symbol table entry out of this
 * temporary structure.
 */
TmpSymbolTableEntry *createTmpSTableEntry() {

    TmpSymbolTableEntry *new_entry = 
        calloc(1, sizeof(TmpSymbolTableEntry));
    if (!new_entry) {
        fprintf(stderr, "Unable to allocate memory for a new"
                "temporary Symbol Table Entry: %s\n", strerror(errno));
    }
    return new_entry;
}


/*
 * typeQualifierSTableEntry - Updates the
 * type qualifier of the temporary symbol table entry.
 */
void typeQualifierSTableEntry(TmpSymbolTableEntry *entry,
                                                enum possibleTypeQualifiers qualifier) {
    switch(entry->var_type_qualifier) {
        case None:
            switch(qualifier) {
                case Const:
                    entry->var_type_qualifier = C;
                    break;
                case Restrict:
                    entry->var_type_qualifier = R;
                    break;
                case Volatile:
                    entry->var_type_qualifier = V;
                    break;
            }
            break;
        case C:
            switch(qualifier) {
                case Restrict:
                    entry->var_type_qualifier = CR;
                    break;
                case Volatile:
                    entry->var_type_qualifier = CV;
                    break;
            }                                                                        break;
        case V:
            switch(qualifier) {
                case Const:
                    entry->var_type_qualifier = CV;
                    break;
                case Restrict:
                    entry->var_type_qualifier = VR;
                    break;
            }                                                                        break;
        case R:
            switch(qualifier) {
                case Const:
                    entry->var_type_qualifier = CR;
                    break;
                case Volatile:
                    entry->var_type_qualifier = VR;
                    break;
            }                                                                        break;
        case CV:
            switch(qualifier) {
                case Restrict:
                    entry->var_type_qualifier = CVR;
                    break;
            }                                                                        break;
        case CR:
            switch(qualifier) {
                case Volatile:
                    entry->var_type_qualifier = CVR;
                    break;
                break;
            }                                                                        break;
        case VR:
            switch(qualifier) {
                case Const:
                    entry->var_type_qualifier = CVR;
                    break;
            }                                                                                 break;
    }                                                               
}




/*
 * isTmpSTableEntryValid - Given a TmpSymbolTAbleEntry object,
 * this function determins whether the declaration specifiers
 * for the specified type are valid. 
 * 
 * Returns 1 (true) or 0(false).
 */
_Bool isTmpSTableEntryValid(TmpSymbolTableEntry *entry) {
    
    /* This function checks, for each declared type, whether
    all specifier incompatible with this type are NULL or 0,
    (which are the values they are initialized with).   */
    switch(entry->type) {
        case VARIABLE_TYPE:
            if (entry->fnc_is_inline || entry->fnc_is_defined || 
                entry->fnc_return_type || entry->fnc_args_type || 
                entry->su_tag_is_defined  ||
                entry->enum_tag_is_defined || entry->enum_parent_tag || 
                entry->enum_const_val || entry->stmt_IR_assembly_label ||
                entry->typedef_type || entry->su_memb_offset ||
                entry->su_memb_type ||
                entry->su_memb_bit_field_width || entry->su_memb_bit_offset)
                return 0;
            break;
        case FUNCTION_TYPE:
            if (entry->var_type_qualifier ||
                entry->var_offset_within_stack_frame ||
                entry->su_tag_is_defined  ||
                entry->enum_tag_is_defined || entry->enum_parent_tag || 
                entry->enum_const_val || entry->stmt_IR_assembly_label ||
                entry->typedef_type || entry->su_memb_offset ||
                entry->su_memb_type || 
                entry->su_memb_bit_field_width || entry->su_memb_bit_offset)
                return 0;
            break;
        case SU_TAG_TYPE:
            if (entry->var_fnc_storage_class || entry->var_type_qualifier ||
                entry->var_offset_within_stack_frame ||
                entry->fnc_is_inline || entry->fnc_is_defined || 
                entry->fnc_return_type || entry->fnc_args_type ||
                entry->enum_tag_is_defined || entry->enum_parent_tag || 
                entry->enum_const_val || entry->stmt_IR_assembly_label ||
                entry->typedef_type || entry->su_memb_offset || 
                entry->su_memb_type ||
                entry->su_memb_bit_field_width || entry->su_memb_bit_offset)
                return 0;
            break;
        case ENUM_TAG:
            if (entry->var_fnc_storage_class || entry->var_type_qualifier ||
                entry->var_offset_within_stack_frame ||
                entry->fnc_is_inline || entry->fnc_is_defined || 
                entry->fnc_return_type || entry->fnc_args_type || 
                entry->su_tag_is_defined  ||
                entry->enum_parent_tag || 
                entry->enum_const_val || entry->stmt_IR_assembly_label ||
                entry->su_memb_type ||
                entry->typedef_type || entry->su_memb_offset || 
                entry->su_memb_bit_field_width || entry->su_memb_bit_offset)
                return 0;
            break;
        case STATEMENT_LABEL:
            if (entry->var_fnc_storage_class || entry->var_type_qualifier ||
                entry->var_offset_within_stack_frame ||
                entry->fnc_is_inline || entry->fnc_is_defined || 
                entry->fnc_return_type || entry->fnc_args_type || 
                entry->su_tag_is_defined  ||
                entry->enum_tag_is_defined || entry->enum_parent_tag || 
                entry->enum_const_val || entry->su_memb_type ||
                entry->typedef_type || entry->su_memb_offset || 
                entry->su_memb_bit_field_width || entry->su_memb_bit_offset)
                return 0;
            break;
        case ENUM_CONST_TYPE:
            if (entry->var_fnc_storage_class || entry->var_type_qualifier ||
                entry->var_offset_within_stack_frame ||
                entry->fnc_is_inline || entry->fnc_is_defined || 
                entry->fnc_return_type || entry->fnc_args_type || 
                entry->su_tag_is_defined  ||
                entry->enum_tag_is_defined || entry->stmt_IR_assembly_label ||
                entry->typedef_type || entry->su_memb_offset || 
                entry->su_memb_type ||
                entry->su_memb_bit_field_width || entry->su_memb_bit_offset)
                return 0;
            break;
        case TYPEDEF_NAME:
            if (entry->var_fnc_storage_class || entry->var_type_qualifier ||
                entry->var_offset_within_stack_frame ||
                entry->fnc_is_inline || entry->fnc_is_defined || 
                entry->fnc_return_type || entry->fnc_args_type || 
                entry->su_tag_is_defined  ||
                entry->enum_tag_is_defined || entry->enum_parent_tag || 
                entry->enum_const_val || entry->stmt_IR_assembly_label ||
                entry->su_memb_offset || entry->su_memb_type ||
                entry->su_memb_bit_field_width || entry->su_memb_bit_offset)
                return 0;
            break;
        case SU_MEMBER_TYPE:
            if (entry->var_fnc_storage_class || entry->var_type_qualifier ||
                entry->var_offset_within_stack_frame ||
                entry->fnc_is_inline || entry->fnc_is_defined || 
                entry->fnc_return_type || entry->fnc_args_type || 
                entry->su_tag_is_defined ||
                entry->enum_tag_is_defined || entry->enum_parent_tag || 
                entry->enum_const_val || entry->stmt_IR_assembly_label ||
                entry->typedef_type)
                return 0;
            break;
        case NO_TYPE:
        default:
            return 0;
            break;
    }
    return 1;
}


/*
 * Takes in a temporary symbol table entry of declaration specifiers
 * (which includes type specifiers, qualifiers, storage class, etc) as 
 * well as an astnode_list struct that contains an array of declarators,
 * and combines them into a astnode_list of symbol table entries.
 * 
 * This function is used for both the declarator grammar and for the 
 * grammar that makes up a struct definition.
 * 
 * Note that this function is called after any error checking and assumes
 * the input to be valid.
 */
astnode_list *combineSpecifierDeclarator(TmpSymbolTableEntry *specifier, 
                                         astnode_list decl_list) {
    
    astnode_list *new_entry = newASTnodeList(decl_list.len, NULL);
    
    for (int i = 0; i < decl_list.len; ++i) {
        astnode *cur_node = newNode_sTableEntry(specifier);
        
        cur_node->stable_entry.ident = decl_list.list[i]->stable_entry.ident;
        
        switch(cur_node->nodetype) {
            case VARIABLE_TYPE:
                break;
            case FUNCTION_TYPE:
                break;
            case SU_TAG_TYPE:
                break;
            case ENUM_TAG:
                break;
            case STATEMENT_LABEL:
                break;
            case ENUM_CONST_TYPE:
                break;
            case TYPEDEF_NAME:
                break;
            case SU_MEMBER_TYPE:
                break;
        }
        
        free(decl_list.list[i]);
    }
    free(specifier);
}



//////////////////////////////////////////////////////////////////////
////////////////////// Scope Related Functions ///////////////////////
//////////////////////////////////////////////////////////////////////

/*
 * searchStackScope - Given a namespace and an identifier, this 
 * function searches through the scope stack (innermost to outermost) 
 * in search of the first variable identifier that correctly matches 
 * the identifier.
 * 
 * If no such identifier it found, a NULL pointer is returned.
 */
astnode *searchStackScope(enum ScopeType namespace, char *ident) {
    
    ScopeStackLayer *cur_scope = scope_stack.innermost_scope;
    while(cur_scope){
        astnode *res = sTableLookup(cur_scope->tables[namespace], ident);
        if (res)
            return res;
        
        cur_scope = cur_scope->child;
    }

    return NULL;
}


/*
 * createNewScope - This function creates a new scope.
 * 
 * Implementation-wise, this function acts as a constructor for the
 * ScopeStackLayer struct.
 */
ScopeStackLayer *createNewScope(enum ScopeType type) {
    ScopeStackLayer *new_scope = malloc(sizeof(ScopeStackLayer));
    if (!new_scope) {
        fprintf(stderr, "Unable to allocate memory for a new"
                "temporary Symbol Table Entry: %s\n", strerror(errno));
    }

    for (int i = 0 ; i < 4 ; ++i)
        new_scope->tables[i] = sTableCreate();

    new_scope->child = NULL;
    new_scope->scope_type = type;
    return new_scope;
}


/*
 * deleteInnermostScope - This function deletes the innermost scope
 * that is in the scope stack. Deleting here refers to freeing the
 * memory, erasing all the symbol tables in the scope. 
 */
void deleteInnermostScope() {
    ScopeStackLayer *new_innermost = scope_stack.innermost_scope->child;
    if (!new_innermost) {   /* the scope stack consists of only file scope */
        error("Unable to delete file (global) scope of a translation unit");
        exit(-1);
    }

    for (int i = 0 ; i < 4 ; ++i)
        free(scope_stack.innermost_scope->tables[i]);
    free(scope_stack.innermost_scope);
    scope_stack.innermost_scope = new_innermost;
}


