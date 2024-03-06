/* $Id: string.c,v 1.1 2024-01-03 16:03:18 behdad Exp $
 *
 * Some string handling routines
 */
#include "c2man.h"
#include <ctype.h>
#include <string.h>


/*------------------------------------------------*/
/* Copy the string into an allocated memory block */
/* Return a pointer to the copy                   */
/*------------------------------------------------*/

                                        /*---------------------------------*/
char *strduplicate (const char *s)	/* The string to copy. May be NULL */
                                        /*---------------------------------*/
{
    char *dest = (char *)NULL;


    /*-------------*/
    /* NULL string */
    /*-------------*/

    if (!s)
       return (char *)NULL;
    
    if ((dest = malloc((unsigned int)(strlen(s)+1))) == (char *)NULL)
	outmem();

    (void)strcpy(dest, s);
    return dest;
}


#ifndef HAS_STRSTR
/*----------------------------------------------------------*/
/* Return a pointer to the first occurence of the substring */
/* within the string, or NULL if not found                  */
/*----------------------------------------------------------*/

char *strstr (const char *src, const char *key)
{
    char   *s = (char *)NULL;
    size_t keylen;

    keylen = strlen(key);
    s      = strchr(src, *key);

    while (s != (char *)NULL)
    {     if (strncmp(s, key, keylen) == 0)
	     return s;
	s = strchr(s+1, *key);
    }
    return (char *)NULL;
}
#endif /* HAS_STRSTR */


/*-----------------------------------------------------------------------------*/
/* Case insensitive comparison of two strings (up to n significant characters) */
/*-----------------------------------------------------------------------------*/

int strncmpi(const char *s1, const char *s2, size_t n)
{
    while(n--)
    {
	char c1 = *s1,
             c2 = *s2;

	if (c1 == '\0' && c2 == '\0')
           break;

	if (isalpha(c1) && isupper(c1))
           c1 = tolower(c1);

	if (isalpha(c2) && isupper(c2))
           c2 = tolower(c2);

	if (c1 < c2)
           return -1;

	if (c1 > c2)
           return 1;

	s1++; s2++;
    }

    return 0;
}


/*------------------------------*/
/* convert string to upper case */
/*------------------------------*/

char *strtoupper(const char *in)
{
    char *s = (char *)NULL;

    for (s = (char *)in; *s; s++)
    {   if (islower(*s))
           *s = toupper(*s);
    }

    return s;
}
