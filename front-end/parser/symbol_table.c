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


/*
 * symbol_table_create - Allocates memory for a new, empty symbol
 * table with the size specified in the macro SYMBOL_TABLE_INIT_SIZE.
 */
SymbolTable *symbol_table_create() {
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
 * symbol_table_destroy - Given a symbol table, this function 
 * frees all its associated memory and return a 1 on sucess and
 * a -1 on error.
 */
void symbol_table_destroy(SymbolTable *table) {
    for (int i = 0; i < table->size ; ++i ) {
        if (table->data[i])
            free(table->data[i]);
    }
    free(table->data);
    free(table);
}


/*
 * symbol_table_insert - Given a symbol table and a symbol table,
 * this function inserts the entry into the table and returns a 1
 * on sucess and a -1 if the entry already exists.
 * 
 * Initially, the symbol table will use linear probing to address
 * hash collisions.
 */
int symbol_table_insert(SymbolTable *table, SymbolTableEntry *entry, int dup_toggle) {
    
    /*  keep the symbol table at least twice the size of the 
        # of elements so that our lookups stay fast */
    if (table->filled >= table->size/2)
        symbol_table_resize(table);

    int data_ind = symbol_table_hash(entry->ident, table->size);

    // linear probing
    while (table->data[data_ind] && table->data[data_ind]->ident != entry->ident)
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
 * symbol_table_lookup - Given a symbol table and a symbol table entry, this
 * function returns a pointer to the symbol table entry that represents this
 * identifier. If no such entry is found in any scope, NULL is returned.
 * 
 * Initially, the symbol table will use linear probing to address
 * hash collisions.
 * */
SymbolTableEntry *symbol_table_lookup(SymbolTable *table, char *entry_name) {
    
    int data_ind = symbol_table_hash(entry_name, table->size);

    // linear probing
    while (table->data[data_ind] && table->data[data_ind]->ident != entry_name)
        data_ind = (data_ind+1) % table->size;

    // Insert entry into correct place in table
    table->data[data_ind];
}


/*
 * symbol_table_resize - Resizes a symbol table by increasing its size 
 * to the next predetermined hash table size.
 * 
 * Returns 1 on sucess, -1 on error.
 */
int symbol_table_resize(SymbolTable *table) {

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
    SymbolTableEntry **new_data = calloc(new_size, sizeof(SymbolTableEntry *));
    if (!new_data) {
        fprintf(stderr, "Error expanding a symbol table: %s\n", strerror(errno));
        return -1;
    } 

    // Iterate through old data, updating new data array
    for (int i = 0 ; i < primes[prime_ind-1] ; ++i) {
        if (table->data[i]) {
            new_data[symbol_table_hash(table->data[i]->ident, new_size)] = table->data[i];
        }
    }

    // Free old data array and update table with the new data array
    free(table->data);
    table->data = new_data;

    return 1;
}


/*
 * symbol_table_hash - Hashes an identifier, modulus the hashtable's capacity.
 */
int symbol_table_hash(char *ident, int capacity) {
    int hashVal = 0;

    for (int i = 0 ;i < strlen(ident) ; ++i)
        hashVal = 37*hashVal + ident[i];

    hashVal %= capacity;
    if (hashVal < 0)
        hashVal += capacity;

    return hashVal;
}


/*
 * symbol_table_creat_tmpentry - creates a temporary symbol table
 * entry strucutre, which will serve as a temporary place holder
 * for all the different declarator specifiers, until a type can 
 * be decided on, in which case an error checking function will
 * make sure the declaration specifiers make sense for the given
 * type and then form an actual symbol table entry out of this
 * temporary structure.
 */
TmpSymbolTableEntry *symbol_table_create_tmpentry() {

    TmpSymbolTableEntry *new_entry = 
        calloc(1, sizeof(TmpSymbolTableEntry));
    if (!new_entry) {
        fprintf(stderr, "Unable to allocate memory for a new"
                "temporary Symbol Table Entry: %s\n", strerror(errno));
    }
    return new_entry;
}


/*
 * update_type_qualifier_for_tmp_stable_entry - Updates the
 * type qualifier of the temporary symbol table entry.
 */
void update_type_qualifier_for_tmp_stable_entry(TmpSymbolTableEntry *entry,
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
 * is_tmp_STentry_correct - Given a TmpSymbolTAbleEntry object,
 * this function determins whether the declaration specifiers
 * for the specified type are valid. 
 * 
 * Returns 1 (true) or 0(false).
 */
_Bool is_tmp_STentry_correct(TmpSymbolTableEntry *entry) {
    
    /* This function checks, for each declared type, whether
    all specifier incompatible with this type are NULL or 0,
    (which are the values they are initialized with).   */
    switch(entry->type) {
        case VARIABLE_TYPE:
            if (entry->fnc_is_inline || entry->fnc_is_defined || 
                entry->fnc_return_type || entry->fnc_args_type || 
                entry->su_tag_is_defined || entry->su_tag_su_table ||
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
                entry->su_tag_is_defined || entry->su_tag_su_table ||
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
                entry->su_tag_is_defined || entry->su_tag_su_table ||
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
                entry->su_tag_is_defined || entry->su_tag_su_table ||
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
                entry->su_tag_is_defined || entry->su_tag_su_table ||
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
                entry->su_tag_is_defined || entry->su_tag_su_table ||
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
                entry->su_tag_is_defined || entry->su_tag_su_table ||
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
 * create_new_entry - Creates a new Symbol Table Entry. Will be called
 * after checking that the TempSymbolTableEntry is valid with regards
 * to the actual identifier type.
 */
SymbolTableEntry *create_new_entry(TmpSymbolTableEntry *tmp_entry) {
    SymbolTableEntry *new_entry = 
        calloc(1, sizeof(SymbolTableEntry));
    if (!new_entry) {
        fprintf(stderr, "Unable to allocate memory for "
                "a new Symbol Table Entry: %s\n", strerror(errno));
    }
    new_entry->file_name = tmp_entry->file_name;
    new_entry->line_num = tmp_entry->line_num;
    new_entry->ident = tmp_entry->ident;
    new_entry->type = tmp_entry->type;
    new_entry->node = tmp_entry->node;

    switch(new_entry->type) {
        case VARIABLE_TYPE:
            new_entry->variable.storage_class = tmp_entry->var_fnc_storage_class;
            new_entry->variable.type_qualifier = tmp_entry->var_type_qualifier;
            new_entry->variable.offset_within_stack_frame = tmp_entry->var_offset_within_stack_frame;
            break;
        case FUNCTION_TYPE:
            new_entry->function.storage_class = tmp_entry->var_fnc_storage_class;
            new_entry->function.is_inline = tmp_entry->fnc_is_inline;
            new_entry->function.is_defined = tmp_entry->fnc_is_defined;
            new_entry->function.return_type = tmp_entry->fnc_return_type;
            new_entry->function.args_types = tmp_entry->fnc_args_type;
            break;
        case SU_TAG_TYPE:
            new_entry->s_u_tag.is_defined = tmp_entry->su_tag_is_defined;
            new_entry->s_u_tag.s_u_table = tmp_entry->su_tag_su_table;
            break;
        case ENUM_TAG:
            new_entry->enum_tag.is_defined = tmp_entry->enum_tag_is_defined;
            break;
        case STATEMENT_LABEL:
            new_entry->statement_label.IR_assembly_label = tmp_entry->stmt_IR_assembly_label;
            break;
        case ENUM_CONST_TYPE:
            new_entry->enum_const.tag = tmp_entry->enum_parent_tag;
            new_entry->enum_const.val = tmp_entry->enum_const_val;
            break;
        case TYPEDEF_NAME:
            new_entry->typedef_name.equivalent_type = tmp_entry->typedef_type;
            break;
        case SU_MEMBER_TYPE:
            new_entry->s_u_member.type = tmp_entry->su_memb_type;
            new_entry->s_u_member.offset_within_s_u = tmp_entry->su_memb_offset;
            new_entry->s_u_member.bit_field_width = tmp_entry->su_memb_bit_field_width;
            new_entry->s_u_member.bit_offset = tmp_entry->su_memb_bit_offset;
            break;
    }

    return new_entry;
}
