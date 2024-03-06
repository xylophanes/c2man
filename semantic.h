/* $Id: semantic.h,v 1.1 2004-02-03 13:09:48 behdad Exp $
 *
 * Declarations for semantic action routines
 */
#include "config.h"

extern boolean is_typedef_name _(( char *name));


/*------------------------------*/
/* New declerator speoification */
/*------------------------------*/

void new_decl_spec (DeclSpec *decl_spec, char *text, int flags);


/*--------------------------------------*/
/* Merge two declarator spoecifications */
/*--------------------------------------*/

extern void join_decl_specs (DeclSpec *result,
	                     DeclSpec *a,
	                     DeclSpec *b);


/*----------------------------------*/
/* Destroy declarator specification */
/*----------------------------------*/

extern void free_decl_spec (DeclSpec *decl_spec);


/*--------------------------*/
/* New declarator parameter */
/*--------------------------*/

                                                    /*----------------------------------------*/
void new_parameter (Parameter  *param,		    /* pointer to structure to be initialized */
                    DeclSpec   *decl_spec,	    /* declaration specifier structure        */
                    Declarator *declarator,	    /* declarator structure                   */
                    char       *comment_before,	    /* comment before the param               */
                    char       *comment_after);     /* comment after the param                */
                                                    /*----------------------------------------*/


/*--------------------------------------------------*/
/* add a comment to the last declarator in the list */
/*--------------------------------------------------*/

int comment_last_decl(DeclaratorList *list, char *comment);


/*------------------------------*/
/* Destroy declarator parameter */
/*------------------------------*/

extern void free_parameter (Parameter *param);


/*-------------------------------------------------*/
/* add a comment to the last parameter in the list */
/*-------------------------------------------------*/

int comment_last_parameter (ParameterList *list, char *comment);


/*---------------------------*/
/* Create new parameter list */
/*---------------------------*/

extern void new_param_list (ParameterList *param_list, Parameter *param);


/*----------------------------------------*/
/* Add parameter to list 'from' returning */
/* merged list as 'to'                    */
/*----------------------------------------*/

extern void add_param_list (ParameterList *to,
	                    ParameterList *from,
	                    Parameter     *param);


/*------------------------*/
/* Destroy parameter list */
/*------------------------*/

extern void free_param_list (ParameterList *param_list);


/*----------------------------*/
/* Create new identifier list */
/*----------------------------*/

extern void new_ident_list (ParameterList *param_list);


/*---------------------------------------*/
/* Add identity to list 'from' returning */
/* merged list as 'to'                   */
/*---------------------------------------*/

extern void add_ident_list (ParameterList *to,
	                    ParameterList *from,
	                    Identifier    *ident);


/*-----------------------*/
/* Create new declarator */
/*-----------------------*/

extern Declarator *new_declarator (char *text, char *name);


/*--------------------*/
/* Destroy declarator */
/*--------------------*/

extern void free_declarator (Declarator *d);


/*----------------------------*/
/* Create new declarator list */
/*----------------------------*/

extern void new_decl_list (DeclaratorList *decl_list,
	                   Declarator     *declarator);



/*-----------------------------------------*/
/* Add declarator to list 'from' returning */
/* merged list as 'to'                     */
/*-----------------------------------------*/

extern void add_decl_list (DeclaratorList *to,
	                   DeclaratorList *from,
	                   Declarator     *declarator);


/*-------------------------*/
/* Destroy declarator list */
/*-------------------------*/

extern void free_decl_list (DeclaratorList *decl_list);


/*---------------------*/
/* Set parameter types */
/*---------------------*/

extern void set_param_types (ParameterList  *params,
	                     DeclSpec       *decl_spec,
	                     DeclaratorList *declarators,
	                     char           *comment,
	                     char           *eolcomment);


/*-----------------------------*/
/* Output a function parameter.*/
/*-----------------------------*/

void output_parameter (Parameter *);


/*---------------------------------------------*/
/* Rememember stuff associated with declarator */
/*---------------------------------------------*/

                                                           /*----------------------------*/
int remember_declarations (char            *comment,	   /* comment associated         */
                           DeclSpec        *decl_spec,	   /* declaration specifier      */
                           DeclaratorList  *decl_list,     /* list of declared variables */
                           char            *eolcomment);   /* eol comment after          */
                                                           /*----------------------------*/


/*------------------------------------------------------------------------*/
/* Initialize a new declaration specifier part, but don't strdup the text */
/*------------------------------------------------------------------------*/

void dyn_decl_spec (DeclSpec      *decl_spec,
                    char          *text,
                    unsigned int  flags);


/*---------------------------------------------------------------------*/
/* Initialize a new declaration specifier part, including an enum part */
/*---------------------------------------------------------------------*/

void new_enum_decl_spec (DeclSpec       *decl_spec,
                         char           *text,
                         int            flags,
                         EnumeratorList *enum_list);


/*------------------------------------------------------------*/
/* Output a declaration specifier for an external declaration */
/*------------------------------------------------------------*/

void output_decl_spec (DeclSpec *decl_spec);


/*---------------------*/
/* Output a declarator */
/*---------------------*/

void output_declarator (Declarator *d, boolean format);


/*-----------------*/
/* Parameter error */
/*-----------------*/

void parameter_error (Parameter *param);


/*-------------------*/
/*  Declarator error */
/*-------------------*/

void declarator_error (Declarator *decl);


/*--------------------------------*/
/* Does function have parameters? */
/*--------------------------------*/

boolean has_parameters (const Declarator *d);


/*--------------------------------*/
/* Is this a function declarator? */
/*--------------------------------*/

boolean is_function_declarator (const Declarator *d);


/*----------------------------------------------*/
/* is a comment a start of a numbered list item */
/*----------------------------------------------*/

boolean is_numbered(const char *text);

