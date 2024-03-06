/* $Id: symbol.c,v 1.1 2004-01-03 16:22:05 behdad Exp $
 *
 * Symbol table maintenance. Implements an abstract data type called
 * the symbol table.
 */
#include "c2man.h"
#include "symbol.h"

/*------------------------------------------------------------------*/
/* Create a symbol table                                            */
/* Return a pointer to the symbol table or NULL if an error occurs  */
/*------------------------------------------------------------------*/

SymbolTable *create_symbol_table (void)
{
    SymbolTable *symtab = (SymbolTable *)NULL;
    unsigned int i;

    symtab = (SymbolTable *)safe_malloc(sizeof(SymbolTable));
    for (i = 0; i < SYM_MAX_HASH; ++i)
	symtab->bucket[i] = (Symbol *)NULL;

    return symtab;
}


/*-----------------------------------------------*/
/* Free the memory allocated to the symbol table */
/*-----------------------------------------------*/

void destroy_symbol_table (SymbolTable *symtab)
{
    unsigned int i;

    Symbol *sym  = (Symbol *)NULL,
           *next = (Symbol *)NULL;

    for (i = 0; i < SYM_MAX_HASH; ++i)
    {   sym = symtab->bucket[i];

	while (sym != NULL)
        {   next = sym->next;

	    safe_free(sym->name); // MAO was free()
	    safe_free(sym);       // MAO was free()

	    sym  = next;
	}
    }

    safe_free(symtab); // MAO was free()
}


/*-----------------------------------------------------------------------*/
/* This is a simple hash function mapping a symbol name to a hash bucket */
/*-----------------------------------------------------------------------*/

static unsigned int hash (const char *name)
{
    char        *s = (char *)NULL;
    unsigned int h;

    h = 0;
    s = (char *)name;

    while (*s != '\0')
          h = (h << 1) ^ *s++;

    return h % SYM_MAX_HASH;
}


/*----------------------------------------------------------*/
/* Search the list of symbols <list> for the symbol <name>. */
/* Return a pointer to the symbol or NULL if not found      */
/*----------------------------------------------------------*/

static Symbol *search_symbol_list (Symbol *list, char *name)
{
    Symbol *sym = (Symbol *)NULL;

    for (sym = list; sym != NULL; sym = sym->next)
    {   if (strcmp(sym->name, name) == 0)
           return sym;
    }

    return (Symbol *)NULL;
}


/*------------------------------------------------------*/
/* Look for symbol <name> in symbol table <symtab>.     */
/* Return a pointer to the symbol or NULL if not found. */
/*------------------------------------------------------*/

Symbol *find_symbol (SymbolTable *symtab, char *name)
{   return search_symbol_list(symtab->bucket[hash(name)], name);
}


/*-----------------------------------------------------------------------*/
/* If the symbol <name> does not already exist in symbol table <symtab>, */
/* then add the symbol to the symbol table.                              */
/* Return a pointer to the symbol or NULL on an error.                   */
/*-----------------------------------------------------------------------*/
                                                    /*-------------------*/
Symbol *new_symbol (SymbolTable *symtab,            /* symbol table      */
                    char        *name,              /* symbol name       */
                    int         flags)              /* symbol attributes */
                                                    /*-------------------*/
{   Symbol  *sym = (Symbol *)NULL;
    int     i;

    if ((sym = find_symbol(symtab, name)) == (Symbol *)NULL)
    {   sym               = (Symbol *)safe_malloc(sizeof(Symbol));
	sym->name         = strduplicate(name);
	sym->flags        = flags;
	sym->valtype      = SYMVAL_NONE;
	i                 = hash(name);
	sym->next         = symtab->bucket[i];
	symtab->bucket[i] = sym;
    }

    return sym;
}
