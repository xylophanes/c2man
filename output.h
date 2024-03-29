/* $Id: output.h,v 1.1 2024-01-03 23:35:57 behdad Exp $
 * format-independant output interface.
 */
#ifndef OUTPUT_H
#define OUTPUT_H

/*-----------------------------------------------------------------------------
 * To add a new output format:
 *
 * 1. Add the new -Tx suboption to the manual page.
 *
 * 2. Add handling for the new suboption to c2man.c, including the default
 *    output file extension.
 *
 * 3. Copy nroff.c to xxx.c and change the xxx_... output functions and the
 *    pointers in the xxx_output structure so the new xxx_output object
 *    generates the correct output constructs. Try to do this without modifying
 *    manpage.c if possible; add new output functions only if necessary.
 *
 * 4. Add the new xxx_output structure to the declaration of output structures
 *    at the end of this file.
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Output object defines what type of output is being generated          */
/* This contains pointers to functions that generate each type of output */
/* construct                                                             */
/*-----------------------------------------------------------------------*/

struct Output
{

    /*-----------------------------------*/
    /* comment until the end of the line */
    /*-----------------------------------*/

    void (*comment)(void);


    /*-------------------------------------*/
    /* header and introduction to the file */
    /*-------------------------------------*/

    void (*header)(ManualPage *firstpage, int input_files, boolean grouped,
		   const char *name, const char *terse, const char *section);
   
    /*--------*/ 
    /* a dash */
    /*--------*/ 

    void (*dash)(void);
   

    /*-------------------------*/ 
    /* start of a main section */
    /*-------------------------*/ 

    void (*section)(const char *name);


    /*------------------------*/
    /* start of a sub section */
    /*------------------------*/

    void (*sub_section)(const char * name);


    /*-----------------------------*/
    /* break the current line here */
    /*-----------------------------*/

    void (*break_line)(void);


    /*--------------*/
    /* a blank line */
    /*--------------*/

    void (*blank_line)(void);


    /*---------------------------------------------------------------------*/
    /* switch into the mode to include declarations (i.e. in program code) */
    /*---------------------------------------------------------------------*/

    void (*code_start)(void);


    /*--------------------------------------*/
    /* switch back from code mode to normal */
    /*--------------------------------------*/

    void (*code_end)(void);
   

    /*-------------------------------------*/ 
    /* output a single string in code font */
    /*-------------------------------------*/ 

    void (*code)(const char *text);


    /*-----------------------------------*/
    /* start a list of tagged paragraphs */
    /*-----------------------------------*/

    void (*tag_list_start)(void);


    /*--------------------------------*/
    /* end a list of tagged paragraph */
    /*--------------------------------*/

    void (*tag_list_end)(void);


    /*-----------------------------------------------------------------*/
    /* start a tagged paragraph: the tag should go straight after this */
    /*-----------------------------------------------------------------*/

    void (*tag_entry_start)(void);


    /*--------------------------------------------------------------------*/
    /* start a tagged paragraph that has an extra non-code bit at the end */
    /* of the tag: the tag should go straight after this                  */
    /*--------------------------------------------------------------------*/

    void (*tag_entry_start_extra)(void);


    /*-----------------------------------*/
    /* end the tag on a tagged paragraph */
    /*-----------------------------------*/

    void (*tag_entry_end)(void);


    /*-------------------------------------------------------------------------*/
    /* end the tag on a tagged paragraph with an extra non-code bit at the end */
    /* of the tag                                                              */
    /*-------------------------------------------------------------------------*/

    void (*tag_entry_end_extra)(const char *text);


    /*-------------------------------*/
    /* start a name/value pair table */
    /*-------------------------------*/

    void (*table_start)(const char *longestag);


    /*---------------------------------------*/
    /* an entry in the name/value pair table */
    /*---------------------------------------*/

    void (*table_entry)(const char *name, const char * description);


    /*-------------------------------*/
    /* end the name/value pair table */
    /*-------------------------------*/

    void (*table_end)(void);


    /*-----------------------*/
    /* an indented paragraph */
    /*-----------------------*/

    void (*indent)(void);


    /*--------------*/
    /* start a list */
    /*--------------*/

    void (*list_start)(void);


    /*----------------------*/
    /* an entry in the list */
    /*----------------------*/

    void (*list_entry)(const char *name);


    /*--------------------------------------------------------*/
    /* the seperator between one entry in a list and the next */
    /*--------------------------------------------------------*/

    void (*list_separator)(void);


    /*--------------*/
    /* end the list */
    /*--------------*/

    void (*list_end)(void);


    /*------------------------------------*/
    /* include another file in the output */
    /*------------------------------------*/

    void (*include)(const char *filename);


    /*--------------*/
    /* end the file */
    /*--------------*/

    void (*file_end)(void);


    /*-------------------------------------------------------------*/
    /* output string, quoted to protect against formatter controls */
    /*-------------------------------------------------------------*/

    void (*text) _((const char *text));


    /*-----------------------------------------------------------*/
    /* output char, quoted to protect against formatter controls */
    /*-----------------------------------------------------------*/

    void (*character)(const int c);


    /*---------------------------------------------------------------*/
    /* parse formatter specific option. set to NULL if not available */
    /*---------------------------------------------------------------*/

    int	 (*parse_option)(const char *option);


    /*---------------------------------------------*/
    /* print formatter specific options to stderr. */
    /*---------------------------------------------*/

    void (*print_options)(void);


    /*----------------------------------------------*/
    /* output NAME section header and section names */
    /*----------------------------------------------*/

    void (*name)(const char *name);


    /*--------------------------------------------------------------*/
    /* output separators between section name and terse description */
    /*--------------------------------------------------------------*/

    void (*terse_sep)(void);


    /*------------------------------------------------*/
    /* output string, making it a hypertext reference */
    /*------------------------------------------------*/

    void (*reference) _((const char *text));


    /*----------------------------------------------------------*/
    /* output string, displaying it emphasized (usually italic) */
    /*----------------------------------------------------------*/

    void (*emphasized) (const char *text);


    /*-----------------------------------------------------------------------*/
    /* output description, fixing punctuation but leaving formatter commands */
    /*-----------------------------------------------------------------------*/

    void (*description) _((const char *text));


    /*-----------------------------------------*/
    /* output returns text, fixing punctuation */
    /*  but leaving formatter commands         */
    /*-----------------------------------------*/

    void (*returns)(const char *text);
};


/*------------------------------------------*/
/* pointer to the relevant output structure */
/*------------------------------------------*/

extern struct Output *output;


/*----------------------------------------------*/
/* output structures for all formats we support */
/*----------------------------------------------*/

extern struct Output nroff_output,
                     texinfo_output,
                     latex_output,
                     html_output,
                     autodoc_output;


/*----------------------------------*/
/* dummy routine which does nothing */
/*----------------------------------*/

void dummy (void);

#endif /* OUTPUT_H */
