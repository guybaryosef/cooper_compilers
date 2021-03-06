/**
 * ECE:466 Compilers
 * By: Guy Bar Yosef
 * 
 * symbol_table.c - Defines the functions declared in symbol_table.h. 
 */

#include "symbol_table.h"
#include "../front_end_header.h"
#include "../lexer/lheader2.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////
/////////////////////// Symbol Table Functions ///////////////////////
//////////////////////////////////////////////////////////////////////

/**
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
    if (sTableResize(new_table) < 0)
        exit(-1);

    return new_table;
}


/**
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


/**
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
        sTableResize(table);

    int data_ind = sTableHash(entry->stable_entry.ident, table->size);
    // linear probing
    while ( table->data[data_ind] && 
            strcmp(table->data[data_ind]->stable_entry.ident, entry->stable_entry.ident)) {
        data_ind = (data_ind+1) % table->size;
    }

    if (table->data[data_ind]) { /* identifier already appears in scope & namespace */
        if (dup_toggle) {
            free(table->data[data_ind]);
            table->data[data_ind] = entry;
        }
        else {
            yyerror("Attempted duplicate definition for identifier");
            return -1;
        }
    }
    else {        // Insert entry into correct place in table
        table->data[data_ind] = entry;
        table->filled++;
    }

    return 1;
}


/**
 * sTableLookUp - Given a symbol table and a symbol table entry, this
 * function returns a pointer to the symbol table entry that represents this
 * identifier. If no such entry is found, NULL is returned.
 * 
 * Initially, the symbol table will use linear probing to address
 * hash collisions.
 * */
astnode *sTableLookUp(SymbolTable *table, char *entry_name) {
    int data_ind = sTableHash(entry_name, table->size);

    // linear probing
    while (table->data[data_ind] && strcmp(table->data[data_ind]->stable_entry.ident, entry_name) )
        data_ind = (data_ind+1) % table->size;

    if (!table->data[data_ind])
        return NULL;
    else  /* we found our symbol table entry */
        return table->data[data_ind];

}


/**
 * sTableResize - Resizes a symbol table by increasing its size 
 * to the next predetermined hash table size.
 * 
 * Returns 1 on sucess, -1 on error.
 */
int sTableResize(SymbolTable *table) {

    // Determine new table size
    /* hash table size options. 
       Lets be honest, wont be larger than 3569 */
    const int primes[] = {0, 5, 11, 23, 37, 97, 199, 409, 823, 1741, 3569};
    
    int new_size = 0;
    int prime_ind;
    for (prime_ind = 0 ; prime_ind < 10 ; ++prime_ind) {
        if (table->size < primes[prime_ind]) {
            new_size = primes[prime_ind];
            break;
        }
    }
    // save pointer to old hash table 
    astnode **old_data = table->data;

    // update new table size
    table->size = new_size;

    // Allocate space for the new array
    table->data = calloc(new_size, sizeof(astnode *));
    if (!table->data) {
        fprintf(stderr, "Error expanding a symbol table: %s\n", strerror(errno));
        return -1;
    } 


    // Iterate through old data, updating new data array
    for (int i = 0 ; i < primes[prime_ind-1] ; ++i) {
        if (old_data[i]) {
            sTableInsert(table, old_data[i], 0);
        }
    }

    // Free old data array and update table with the new data array
    free (old_data);
    return 1;
}


/**
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


/**
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

    new_entry->line_num = cur_line_num;
    new_entry->file_name = cur_file_name;
    return new_entry;
}



/**
 * helperTypeQualifierAddition - A funciton to add two type qualifiers 
 * together.
 */
enum possibleTypeQualifiers helperTypeQualifierAddition(enum possibleTypeQualifiers one,
                                                        enum possibleTypeQualifiers two) {
    switch(one) {
        case None:
            switch(two) {
                case Const:     return C;   break;
                case Restrict:  return R;   break;
                case Volatile:  return V;   break;
            }
            break;
        case C:
            switch(two) {
                case R: return CR;  break;
                case V: return CV;  break;
                default:return one;
            }                                                                        break;
            break;
        case V:
            switch(two) {
                case C: return CV;  break;
                case R: return VR;  break;
                default:return one;
            }                                                                        break;
            break;
        case R:
            switch(two) {
                case C: return CR;  break;
                case V: return VR;  break;
                default:return one;
            }                                                                        break;
            break;
        case CV:
            switch(two) {
                case R:     return CVR;     break;
                default:    return one;
            }                                                                        break;
            break;
        case CR:
            switch(two) {
                case V:     return CVR;  break;
                default:    return one;
            }                                                                        break;
            break;
        case VR:
            switch(two) {
                case Const: return CVR; break;
                default:    return one;
            }
            break;
        default:
            return one;                                                                                break;
    }   
}


/**
 * typeQualifierSTableEntry - Updates the
 * type qualifier of the temporary symbol table entry.
 */
void typeQualifierSTableEntry(TmpSymbolTableEntry *entry,
                              enum possibleTypeQualifiers qualifier) {
    entry->var_type_qualifier = helperTypeQualifierAddition(qualifier,
                                entry->var_fnc_storage_class);                             
}


/**
 * isTmpSTableEntryValid - Given a TmpSymbolTAbleEntry object,
 * this function determins whether the declaration specifiers
 * for the specified type are valid. 
 * 
 * Returns 1 (true) or 0(false).
 */
_Bool isTmpSTableEntryValid(TmpSymbolTableEntry *entry) {

    /* This function checks whether all declaration specifiers 
       incompatible with this type are NULL or 0,
       (which are the values they are initialized with). */ 
    if ( (entry->type == Variable_Type && (
            entry->fnc_is_inline || entry->fnc_args_type || 
            entry->su_tag_is_defined  || entry->enum_tag_is_defined || 
            entry->enum_parent_tag || entry->enum_const_val || 
            entry->stmt_IR_assembly_label || entry->typedef_type || 
            entry->su_memb_offset || entry->su_memb_type ||
            entry->su_memb_bit_field_width || entry->su_memb_bit_offset
        ) ) || 
        ( entry->type == Function_Type && (
            entry->var_type_qualifier ||
            entry->var_offset_within_stack_frame ||
            entry->su_tag_is_defined  ||
            entry->enum_tag_is_defined || entry->enum_parent_tag || 
            entry->enum_const_val || entry->stmt_IR_assembly_label ||
            entry->typedef_type || entry->su_memb_offset ||
            entry->su_memb_type || 
            entry->su_memb_bit_field_width || entry->su_memb_bit_offset
        ) ) ||
        ((entry->type == S_Tag_Type || entry->type == U_Tag_Type) && (
            entry->var_fnc_storage_class || entry->var_type_qualifier ||
            entry->var_offset_within_stack_frame ||
            entry->fnc_is_inline || 
            entry->fnc_return_type || entry->fnc_args_type ||
            entry->enum_tag_is_defined || entry->enum_parent_tag || 
            entry->enum_const_val || entry->stmt_IR_assembly_label ||
            entry->typedef_type || entry->su_memb_offset || 
            entry->su_memb_type ||
            entry->su_memb_bit_field_width || entry->su_memb_bit_offset
        ) ) ||
        (entry->type == Enum_Tag && (
            entry->var_fnc_storage_class || entry->var_type_qualifier ||
            entry->var_offset_within_stack_frame ||
            entry->fnc_is_inline || entry->fnc_return_type || 
            entry->fnc_args_type || entry->su_tag_is_defined  ||
            entry->enum_parent_tag || entry->enum_const_val || 
            entry->stmt_IR_assembly_label || entry->su_memb_type ||
            entry->typedef_type || entry->su_memb_offset || 
            entry->su_memb_bit_field_width || entry->su_memb_bit_offset
        ) ) ||
        (entry->type == Statement_Label && (
            entry->var_fnc_storage_class || entry->var_type_qualifier ||
            entry->var_offset_within_stack_frame ||
            entry->fnc_is_inline || entry->fnc_return_type || 
            entry->fnc_args_type || entry->su_tag_is_defined  ||
            entry->enum_tag_is_defined || entry->enum_parent_tag || 
            entry->enum_const_val || entry->su_memb_type ||
            entry->typedef_type || entry->su_memb_offset || 
            entry->su_memb_bit_field_width || entry->su_memb_bit_offset
        ) ) ||
        (entry->type == Enum_Const_Type && (
            entry->var_fnc_storage_class || entry->var_type_qualifier ||
            entry->var_offset_within_stack_frame ||
            entry->fnc_is_inline || entry->fnc_return_type || 
            entry->fnc_args_type || entry->su_tag_is_defined  ||
            entry->enum_tag_is_defined || entry->stmt_IR_assembly_label ||
            entry->typedef_type || entry->su_memb_offset || 
            entry->su_memb_type ||
            entry->su_memb_bit_field_width || entry->su_memb_bit_offset
        ) ) ||
        (entry->type == Typedef_Name && (
            entry->var_fnc_storage_class || entry->var_type_qualifier ||
            entry->var_offset_within_stack_frame ||
            entry->fnc_is_inline || entry->fnc_return_type || 
            entry->fnc_args_type || entry->su_tag_is_defined  ||
            entry->enum_tag_is_defined || entry->enum_parent_tag || 
            entry->enum_const_val || entry->stmt_IR_assembly_label ||
            entry->su_memb_offset || entry->su_memb_type ||
            entry->su_memb_bit_field_width || entry->su_memb_bit_offset
        ) ) ||
        (entry->type == SU_Member_Type && (
            entry->var_fnc_storage_class || entry->var_type_qualifier ||
            entry->var_offset_within_stack_frame ||
            entry->fnc_is_inline || entry->fnc_return_type || 
            entry->fnc_args_type || entry->su_tag_is_defined ||
            entry->enum_tag_is_defined || entry->enum_parent_tag || 
            entry->enum_const_val || entry->stmt_IR_assembly_label ||
            entry->typedef_type
        ) ) ) {
        return 0;

        }
        
    // else all is good, return 1
    return 1;
}


/**
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
 * 
 * Input:
 *      - specifier: A TmpSymbolTableEntry struct that contains the declaration
 * specifiers of the declaration(storage class, type specifiers & qualifiers, etc).
 *      - decl_list: A astnode_list struct whose list member is made up of symbol
 * table entries that contain a name (ident field) and a node pointer if needed
 * (will be needed if it is a pointer or an array or a function, but not otherwise).
 * 
 * Output:
 *      - An astnode_list whose list members are astnodes containing Symbol
 * Table Entries that are ready to be inputted into the relevant scope. 
 */
astnode_list *combineSpecifierDeclarator(TmpSymbolTableEntry *specifier, 
                                         struct astnode_list *decl_list) {
    
    astnode_list *new_entries = newASTnodeList(decl_list->len, NULL);

    for (int i = 0; i < decl_list->len; ++i) {
        new_entries->list[i] = newNode_sTableEntry(specifier);
        new_entries->list[i]->stable_entry.ident = decl_list->list[i]->stable_entry.ident;
        
        /* get second_handle to be the last node of the type of this declarator */
        astnode *cur_handle = decl_list->list[i]->stable_entry.node;
        astnode *second_handle = cur_handle;
        while   (   cur_handle &&
                    (cur_handle->nodetype == PTR_TYPE    ||
                     cur_handle->nodetype == ARRAY_TYPE)
                ) {

            second_handle = cur_handle;

            if (cur_handle->nodetype == PTR_TYPE)
                cur_handle = cur_handle->ptr.pointee;
            else 
                cur_handle = cur_handle->arr.ptr;
        }

        if (decl_list->list[i]->nodetype == STABLE_FNC_DECLARATOR) {
            /* get to tip or return type nodes, and append the type specifier */
            if (!decl_list->list[i]->stable_entry.fnc.return_type) {
                decl_list->list[i]->stable_entry.fnc.return_type = specifier->node;
            }
            else {
                astnode *cur_handle = decl_list->list[i]->stable_entry.fnc.return_type;
                astnode *second_handle = cur_handle;
                while   (   cur_handle &&
                            (cur_handle->nodetype == PTR_TYPE    ||
                            cur_handle->nodetype == ARRAY_TYPE)
                        ) {

                    second_handle = cur_handle;

                    if (cur_handle->nodetype == PTR_TYPE)
                        cur_handle = cur_handle->ptr.pointee;
                    else 
                        cur_handle = cur_handle->arr.ptr;
                }
                second_handle->ptr.pointee = specifier->node;
                
            }
            new_entries->list[i] = decl_list->list[i];
        }
        else if (cur_handle && cur_handle->nodetype == FNC_TYPE) {
            cur_handle->fnc_type.return_type = specifier->node;
            new_entries->list[i]->stable_entry.node = decl_list->list[i]->stable_entry.node;
            free(decl_list->list[i]);
        }   
        else if (second_handle && second_handle->nodetype == PTR_TYPE) {
            /* deals with array and pointer types */
            new_entries->list[i]->stable_entry.node = decl_list->list[i]->stable_entry.node;
            second_handle->ptr.pointee = specifier->node;
        }     

    }
    free(specifier);

    return new_entries;
}



//////////////////////////////////////////////////////////////////////
////////////////////// Scope Related Functions ///////////////////////
//////////////////////////////////////////////////////////////////////

/**
 * searchStackScope - Given a namespace and an identifier, this 
 * function searches through the scope stack (innermost to outermost) 
 * in search of the first variable identifier that correctly matches 
 * the identifier.
 * 
 * If no such identifier it found, a NULL pointer is returned.
 */
astnode *searchStackScope(enum Namespace ns, char *ident) {

    ScopeStackLayer *cur_scope = scope_stack.innermost_scope;
    while(cur_scope){
        astnode *res = sTableLookUp(cur_scope->tables[ns], ident);
        if (res)
            return res;
        
        cur_scope = cur_scope->child;
    }

    return NULL;
}


/**
 * createNewScope - This function creates a new scope.
 * 
 * Implementation-wise, this function acts as a constructor for the
 * ScopeStackLayer struct.
 */
void createNewScope(enum ScopeType type, char *name) {
    ScopeStackLayer *new_scope = malloc(sizeof(ScopeStackLayer));
    if (!new_scope)
        yyerror("Unable to allocate memory for a new scope");

    for (int i = 0 ; i < 4 ; ++i)
        new_scope->tables[i] = sTableCreate();

    new_scope->name = name;
    new_scope->child = scope_stack.innermost_scope;
    new_scope->scope_type = type;
    new_scope->begin_line_num = cur_line_num;
    new_scope->beginning_file = cur_file_name;

    scope_stack.innermost_scope = new_scope;


    if (!(scope_stack.global_scope))
        scope_stack.global_scope = new_scope;
}


/**
 * deleteInnermostScope - This function deletes the innermost scope
 * that is in the scope stack. Deleting here refers to freeing the
 * memory, erasing all the symbol tables in the scope. 
 */
void deleteInnermostScope() {
    ScopeStackLayer *new_innermost = scope_stack.innermost_scope->child;
    if (!new_innermost) {   /* the scope stack consists of only file scope */
        yyerror("Unable to delete file (global) scope of a translation unit.");
        exit(-1);
    }

    for (int i = 0 ; i < 3 ; ++i)
        free(scope_stack.innermost_scope->tables[i]);
    free(scope_stack.innermost_scope);
    scope_stack.innermost_scope = new_innermost;
}