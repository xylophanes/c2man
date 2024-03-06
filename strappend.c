/* $Id: strappend.c,v 1.1 2024-01-03 15:51:28 behdad Exp $
 */
#include "c2man.h"
#include "strappend.h"

#ifdef I_STDARG
#include <stdarg.h>
#endif
#ifdef I_VARARGS
#include <varargs.h>
#endif


/*--------------------*/
/* Imported functions */
/*--------------------*/

extern void outmem(void);

/*------------------------------------------------------------------------------*/
/* append a list of strings to another, storing them in a malloc'ed region      */
/* The first string may be NULL, in which case the rest are simply concatenated */
/*------------------------------------------------------------------------------*/

#ifdef I_STDARG
char *strappend(char *first, ...)
#else
char *strappend(va_alist)
    va_dcl
#endif /* I_STDARG */

{   size_t  totallen;
    va_list argp;

    char *s         = (char *)NULL,
         *retstring = (char *)NULL;

    #ifndef I_STDARG
    char *first     = (char *)NULL;;
    #endif /* I_STDARG */


    /*-------------------------*/
    /* add up the total length */
    /*-------------------------*/

    #ifdef I_STDARG
    va_start(argp,first);
    #else
    va_start(argp);
    first = va_arg(argp, char *);
    #endif /* I_STDARG */

    totallen = first ? strlen(first) : 0;

    while ((s = va_arg(argp,char *)) != (char *)NULL)
	  totallen += strlen(s);
    va_end(argp);
   
    /*-------------------*/ 
    /* malloc the memory */
    /*-------------------*/

                /*----------------------------------*/
    totallen++;	/* add space for the nul terminator */
                /*----------------------------------*/

    if ((retstring = first ? realloc(first,totallen) : malloc(totallen)) == 0)
	outmem();

    if (first == (char *)NULL)	*retstring = '\0';

    #ifdef I_STDARG
    va_start(argp,first);
    #else
    va_start(argp);

                                        /*--------------------*/
    first = va_arg(argp, char *);	/* skip the first arg */
                                        /*--------------------*/

    #endif /* I_STDARG */

    while ((s = va_arg(argp,char *)) != (char *)NULL)
          (void)strcat(retstring,s);
    va_end(argp);

    return retstring;
}
