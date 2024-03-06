/* $Id: strconcat.c,v 1.1 2004-05-03 05:17:48 behdad Exp $
 * concatenate a list of strings, storing them in a malloc'ed region
 */
#include "c2man.h"
#include "strconcat.h"

#ifdef I_STDARG
#include <stdarg.h>
#endif /* I_STDARG */

#ifdef I_VARARGS
#include <varargs.h>
#endif /* I_VARARGS */

extern void outmem();

#ifdef I_STDARG
char *strconcat(const char *first, ...)
#else
char *strconcat(va_alist)
     va_dcl
#endif /* I_STDARG */
{
    size_t  totallen;
    va_list argp;

    char    *s         = (char *)NULL,
            *retstring = (char *)NULL;

    #ifndef I_STDARG
    char    *first     = (char *)NULL;
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

    #ifdef DEBUG
    (void)fprintf(stderr,"strconcat: \"%s\"",first);
    #endif /* DEBUG */

    totallen = strlen(first);
    while ((s = va_arg(argp,char *)) != NULL)
    {     totallen += strlen(s);

          #ifdef DEBUG
          (void)fprintf(stderr,",\"%s\"",s);
          #endif /* DEBUG */
    }

    #ifdef DEBUG
    (void)fprintf(stderr,"\nstrlen = %ld\n",(long)totallen);
    #endif /* DEBUG */

    va_end(argp);


    /*-------------------*/
    /* malloc the memory */
    /*-------------------*/

    if ((retstring = malloc(totallen + 1)) == 0)
	outmem();
	
    #ifdef I_STDARG
    va_start(argp,first);
    #else
    va_start(argp);
    first = va_arg(argp, char *);
    #endif /* I_STDARG */


    /*---------------*/
    /* copy stuff in */
    /*---------------*/

    (void)strcpy(retstring,first);

    while ((s = va_arg(argp,char *)) != (char *)NULL)
	  (void)strcat(retstring,s);

    va_end(argp);

    #ifdef DEBUG
    (void)fprintf(stderr,"strconcat returns \"%s\"\n",retstring);
    #endif /* DEBUG */

    return retstring;
}
