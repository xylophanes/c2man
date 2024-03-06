/* $Id: manpage.h,v 1.1 2024-01-03 00:15:38 behdad Exp $
 * stuff to do with manual page outputing
 */

/*--------------------------------------------------------------*/
/* Tranlated to ANSI-C by Mark O'Neill <mao@tumblingdice.co.uk> */
/*--------------------------------------------------------------*/

#ifndef MANPAGE_H
#define MANPAGE_H

#include <time.h>
#include "c2man.h"


/*---------------------*/
/* Manual page section */
/*---------------------*/
/*---------------------------------------*/
/* Forward typedef as linked list struct */
/* is self referencing                   */
/*---------------------------------------*/

typedef struct Section Section;
struct Section
{
    Section *next;
    char    *name;
    char    *text;
    boolean been_output;
};



/*-------------*/
/* Manual page */
/*-------------*/
/*---------------------------------------*/
/* Forward typedef as linked list struct */
/* is self referencing                   */
/*---------------------------------------*/

typedef struct ManualPage ManualPage;
struct  ManualPage
{
    DeclSpec   *decl_spec;
    Declarator *declarator;
    ManualPage *next;
    Section    *first_section;
    char       *description;
    char       *returns;
    char       *sourcefile;
    time_t     sourcetime;
};



/*---------------*/
/* Types of link */
/*---------------*/

enum LinkType
{

    /*----------------------*/
    /* filesystem hard link */
    /*----------------------*/

    #ifdef HAS_LINK
    LINK_HARD, 
    #endif /* HAS_LINK */


    /*----------------------*/
    /* filesystem soft link */
    /*----------------------*/

    #ifdef HAS_SYMLINK
    LINK_SOFT,
    #endif


    /*-------------------------------*/
    /* nroff file with .so directive */
    /*-------------------------------*/

    LINK_FILE,


    /*---------------------------------*/
    /* don't create extra links for it */
    /*---------------------------------*/

    LINK_NONE,


    /*-------------------------------------------------*/
    /* don't create extra links & remove existing ones */
    /*-------------------------------------------------*/

    LINK_REMOVE	
};


/*--------------------*/
/* Exported variables */
/*--------------------*/
/*----------------------*/
/* list of manual pages */
/*----------------------*/

extern ManualPage *firstpage;


/*--------------------*/
/* Exported functions */
/*--------------------*/

void new_manual_page (char *comment, DeclSpec *decl_spec, Declarator *declarator);

/*-----------------------------------------------------------------*/
/* remember the terse description from the first comment in a file */
/*-----------------------------------------------------------------*/

void remember_terse (char *comment);
void output_manual_pages (ManualPage *first, int num_input_files, enum LinkType link_type);
void free_manual_pages   (ManualPage *first);
void output_format_string (const char *fmt);
void output_warning (void);
void output_comment (const char *comment);

#endif /* MANPAGE_H */
