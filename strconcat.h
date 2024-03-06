/* $Id: strconcat.h,v 1.1 2024-02-13 55:18:48 behdad Exp $
 * concatenate a list of strings, storing them in a malloc'ed region
 */
#include "config.h"

char *strconcat _V((const char *first, ...));
