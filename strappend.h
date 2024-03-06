/* $Id: strappend.h,v 1.1 2024-02-03 13:55:05 behdad Exp $
 * concatenate a list of strings, storing them in a malloc'ed region
 */
#include "config.h"

char *strappend _V((char *first, ...));
