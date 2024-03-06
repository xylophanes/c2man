/* $Id: enum.c,v 1.1 2024-01-03 11:27:18 behdad Exp $
 * enumerator operations
 */
#include "c2man.h"
#include "strconcat.h"
#include "enum.h"
#include "manpage.h"


/*-------------------*/
/* enum symbol table */
/*-------------------*/

SymbolTable *enum_table = (SymbolTable *)NULL;

/*-------------------------------------------------------------------------*/
/* we have to keep a list of EnumeratorLists because:                      */
/*                                                                         */
/* - unnamed EnumeratorLists can't go in the symbol table.                 */
/* - a single EnumeratorList can be typedef'ed or enum'ed to more than one */
/*   symbol, in which case it is referenced from the typedef_table or      */
/*   enum_table repectively more than once.                                */
/*-------------------------------------------------------------------------*/

EnumeratorList *first_list      = (EnumeratorList *)NULL,
               **last_next_list = &first_list;


/*----------------------------------*/
/* Initialize a list of enumerators.*/
/*----------------------------------*/

EnumeratorList *new_enumerator_list ( Enumerator *enumerator)
{
    Enumerator     *p    = (Enumerator     *)NULL;
    EnumeratorList *list = (EnumeratorList *)NULL;
    
    list = (EnumeratorList *)safe_malloc(sizeof *list);

    *last_next_list = list;
    last_next_list  = &list->next;
    list->next      = ( EnumeratorList *)NULL;
    
    p  = (Enumerator *)safe_malloc(sizeof(Enumerator));
    *p = *enumerator;
    
    list->first = list->last = p;
    p->next                  = (Enumerator *)NULL;

    return list;
}


/*---------------------------------*/
/* Add the enumerator to the list. */
/*---------------------------------*/

void add_enumerator_list(EnumeratorList *list,
                         Enumerator     *enumerator)
{
    Enumerator *p = (Enumerator *)NULL;

    p   = (Enumerator *)safe_malloc((unsigned)sizeof(Enumerator));
    *p = *enumerator;

    list->last->next = p;
    list->last       = p;
    p->next          = (Enumerator *)NULL;
}


/*----------------------------------------------------------*/
/* Free storage used by the elements in the enumerator list */
/*----------------------------------------------------------*/

void free_enumerator_list (EnumeratorList *enumerator_list)
{
    Enumerator *p    = (Enumerator *)NULL,
               *next = (Enumerator *)NULL;

    p = enumerator_list->first;
    while (p != (Enumerator *)NULL) 
    {   next = p->next;
	free_enumerator(p);
	safe_free(p); // MAO was free()

	p   = next;
    }
}


/*-----------------------*/
/* Create new enumerator */
/*-----------------------*/

void new_enumerator(Enumerator *e,
                    char *name,
                    char *comment_before,
                    char *comment_after)
{   e-> name = name;
    e-> comment = comment_after ? comment_after : comment_before;
    e-> group_comment = comment_before && comment_after ? comment_before : NULL;
}


/*-----------------------------------------*/
/* Free the storage used by the enumerator */
/*-----------------------------------------*/

void free_enumerator (Enumerator *param)
{
    safe_free(param->name); // MAO was free()
    safe_free(param->comment);
    safe_free(param->group_comment);
}


/*--------------------------------------------------*/
/* add a comment to the last enumerator in the list */
/*--------------------------------------------------*/
int comment_last_enumerator(EnumeratorList *list,
                            char           *comment)
{   if (list->last->comment)
    {   if (list->last->group_comment)
	{    enumerator_error(list->last->name);
	    safe_free(comment); // MAO was free()
	    return 0;
	}

	list->last->group_comment = list->last->comment;
    }

    list->last->comment = comment;
    return 1;
}


/*---------------------------*/
/* enum namespace management */
/*---------------------------*/

void add_enum_symbol(char *name, EnumeratorList *enum_list)
{   Symbol *entry = new_symbol(enum_table, name, DS_NONE);
    
    if (entry)
    {  entry->value.enum_list = enum_list;
       entry->valtype = SYMVAL_ENUM;
    }
}


/*---------------------------------------------------------*/
/* look for the Enumerator list associated with the symbol */
/*---------------------------------------------------------*/

EnumeratorList *find_enum_symbol(char *name)
{   Symbol *entry = find_symbol(enum_table, name);
    
    if (entry)
    	return entry->value.enum_list;
    else
    	return NULL;
}


/*-------------------------*/
/* Destroy enumerator list */
/*-------------------------*/

void destroy_enum_lists(void)
{
    EnumeratorList *list, *next = (EnumeratorList *)NULL;
   

    /*-------------------------------*/ 
    /* free all the enumerator lists */
    /*-------------------------------*/ 

    for (list = first_list; list; list = next)
    {   next = list->next;
	free_enumerator_list(list);
	safe_free(list); // MAO was free()
    }
}


/*----------------------------*/
/* create new typedef symbols */
/*----------------------------*/

void new_typedef_symbols(DeclSpec *decl_spec, DeclaratorList *decl_list)
{    Declarator *d = (Declarator *)NULL;
    
    for (d = decl_list->first; d; d = d-> next)
    {   Symbol *s = new_symbol(typedef_names, d->name, DS_NONE);
	
	if (s && decl_spec->enum_list)
	{  s->value.enum_list = decl_spec->enum_list;
	   s->valtype = SYMVAL_ENUM;
	}
    }
}

void enumerator_error(char *name)
{   yyerror("enumerator '%s' has multiple comments", name);
}
