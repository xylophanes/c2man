/* $Id: symbol.h,v 1.1 2024-02-03 13:55:30 behdad Exp $
 *
 * Definitions for a symbol table
 */
#include "config.h"

#ifndef _SYMBOL_H
#define _SYMBOL_H

typedef struct _symbol
{
                                                        /*---------------------*/
                           struct _symbol *next;	/* next symbol in list */
                           char           *name;	/* name of symbol      */
                           unsigned short flags;	/* symbol attributes   */
                                                        /*---------------------*/

	                   enum { SYMVAL_NONE,
                                  SYMVAL_ENUM } valtype;
	
                          union { struct _enumerator_list *enum_list;
	                        } value;
} Symbol;


/*------------------------------------------------*/
/* The hash table length should be a prime number */
/*------------------------------------------------*/

#define SYM_MAX_HASH 251


/*---------------------*/
/* Hashed symbol table */
/*---------------------*/

typedef struct _symbol_table
{
                                        /*--------------*/
    Symbol *bucket[SYM_MAX_HASH];	/* hash buckets */
                                        /*--------------*/
} SymbolTable;


/*---------------------*/
/* Create symbol table */
/*---------------------*/

extern SymbolTable *create_symbol_table(void);


/*----------------------*/
/* destroy symbol table */
/*----------------------*/

extern void destroy_symbol_table (SymbolTable *symtab);


/*--------------------*/
/* Lookup symbol name */
/*--------------------*/

extern Symbol *find_symbol (SymbolTable *symtab, char *name);


/*-------------------*/
/* Define new symbol */
/*-------------------*/

extern Symbol *new_symbol (SymbolTable *symtab, char *name, int flags);

#endif /* SYMBOL_H */
