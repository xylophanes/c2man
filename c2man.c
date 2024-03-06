/* $Id: c2man.c,v 1.1 2024-01-03 09:27:40 behdad Exp $
/*-------------------------------------------------------------
 * C Manual page generator.
 * Reads C source code and outputs manual pages.
 * Converted to ANSI-C by Mark O'Neill <mao@tumblingdice.co.uk>
 ------------------------------------------------------------*/

#include <ctype.h>
#include <errno.h>

#include "c2man.h"
#include "enum.h"
#include "strconcat.h"
#include "strappend.h"
#include "manpage.h"
#include "output.h"
#include "patchlevel.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef I_SYS_TYPES
#include <sys/types.h>
#endif /* I_SYS_TYPES */

#ifdef I_FCNTL
#include <fcntl.h>
#endif /* I_FCNTL */

#ifdef I_SYS_FILE
#include <sys/file.h>
#endif /* I_SYS_FILE */

#include <sys/stat.h>
#include <signal.h>


/*---------------------*/
/* getopt declarations */
/*---------------------*/

extern int getopt();
extern char *optarg;
extern int optind;


/*------------------*/
/* lex declarations */
/*------------------*/
/*------------------*/
/* lex input stream */
/*------------------*/

extern FILE *yyin;


/*---------------------*/
/* Name of the program */
/*---------------------*/

const char *progname = "c2man";


/*-----------------*/
/* Program options */
/*-----------------*/
/*----------------------------------------------*/
/* TRUE if static declarations are also output. */
/*----------------------------------------------*/

boolean static_out = FALSE;


/*-------------------------------------------*/
/* TRUE if variable declarations are output. */
/*-------------------------------------------*/

boolean variables_out = FALSE;


/*------------------------------------------------*/
/* TRUE if formal parameter promotion is enabled. */
/*------------------------------------------------*/

boolean promote_param = TRUE;


/*-------------------------------------------------------*/
/* String output before prototype declaration specifiers */
/*-------------------------------------------------------*/

const char *decl_spec_prefix = "";


/*-------------------------------------------*/
/* String output before prototype declarator */
/*-------------------------------------------*/

const char *declarator_prefix = " ";


/*------------------------------------------*/
/* String output after prototype declarator */
/*------------------------------------------*/

const char *declarator_suffix = "\n";

/*------------------------------------------------------------------*/
/* String output before the first parameter in a function prototype */
/*------------------------------------------------------------------*/

const char *first_param_prefix = "\n\t";


/*------------------------------------------------------------------------*/
/* String output before each subsequent parameter in a function prototype */
/*------------------------------------------------------------------------*/

const char *middle_param_prefix = "\n\t";


/*----------------------------------------------------------------*/
/* String output after the last parameter in a function prototype */
/*----------------------------------------------------------------*/

const char *last_param_suffix = "\n";


/*------------------------------------*/
/* Directory to write output files in */
/*------------------------------------*/

char *output_dir  = (char *)NULL;


/*--------------------*/
/* Name of the manual */
/*--------------------*/

char *manual_name = (char *)NULL;


/*-------------------------*/
/* Section for manual page */
/*-------------------------*/

const char *manual_section = (char *)NULL;


/*-------------------------------------*/
/* prefix for generated #include lines */
/*-------------------------------------*/

char *header_prefix = (char *)NULL;


/*----------------------------------------*/
/* list of include file specified by user */
/*----------------------------------------*/

IncludeFile *first_include             = (IncludeFile *)NULL;
static IncludeFile **last_next_include = &first_include;


/*---------------------------------------------*/
/* list of excluded sections specified by user */
/*---------------------------------------------*/

ExcludeSection *first_excluded_section             = (ExcludeSection *)NULL;;
static ExcludeSection **last_next_excluded_section = &first_excluded_section;


/*--------------------------------------------------------*/
/* TRUE if c2man should attempt to fixup comment sections */
/*--------------------------------------------------------*/

boolean fixup_comments = TRUE;


/*------------------------------------------*/
/* do we group related stuff into one file? */
/*------------------------------------------*/

boolean group_together;


/*--------------------------------------------------------------*/
/* was terse description read from file or command line option? */
/*--------------------------------------------------------------*/

boolean terse_specified;


/*-----------------------------------------*/
/* terse description when grouped together */
/*-----------------------------------------*/

char *group_terse = NULL;


/*--------------------------------------------------------------------------*/
/* should we always document parameters, even if it's only "Not Documented" */
/*--------------------------------------------------------------------------*/

boolean always_document_params = TRUE;


/*-------------------------------------------------------------------*/
/* look for a function def comment at the start of the function body */
/*-------------------------------------------------------------------*/

boolean look_at_body_start = FALSE;


/*------------------------------------------------------------------------*/
/* only look for a function def comment at the start of the function body */
/*------------------------------------------------------------------------*/

boolean body_start_only = FALSE;


/*------------------------------------------*/
/* default output info for each object type */
/*------------------------------------------*/

struct Output_Object_Info output_object[_OBJECT_NUM] =
{
    #if 0
    {'c', "class"},
    {'s', "struct"},
    {'e', "enum"},
    {'t', "typedef"},
    #endif /* 0 */

    {'f', "function"},
    {'v', "variable"},
    {'F', "static function"},
    {'V', "static variable"}
};


/*--------------------------*/
/* Include file directories */
/*--------------------------*/

int num_inc_dir = 2;
const char *inc_dir[MAX_INC_DIR] = { "./", "/usr/include/" };


/*------------------------------------*/
/* total number of errors encountered */
/*------------------------------------*/

int errors;


/*-----------------------------------------------------*/
/* name of the base file being processed; NULL = stdin */
/*-----------------------------------------------------*/

const char *basefile = (char *)NULL;

                        /*-----------------------------------*/
time_t basetime;	/* modification time of base file    */
boolean inbasefile;	/* are we parsing in that base file? */
                        /*-----------------------------------*/


/*---------------------------------*/
/* is the base file a header file? */
/*---------------------------------*/

boolean header_file;

/*-----------------------------*/
/* use nroff output by default */
/*-----------------------------*/

struct Output *output = &nroff_output;
const char *default_section = "3";


/*----------------------------------------------------------------*/
/* should we generate the output file named after the input file? */
/*----------------------------------------------------------------*/

boolean use_input_name = FALSE;


/*--------------------------------------*/
/* should we generate embeddable files? */
/*--------------------------------------*/

boolean make_embeddable = FALSE;

const char *cpp_cmd = CPP_FILE_COM;
boolean verbose     = FALSE;


/*------------------------------*/
/* can cpp read standard input? */
/*------------------------------*/

static boolean cppcanstdin
#ifdef CPP_CAN_STDIN
			    = 1
#endif
;


/*------------------------------*/
/* does cpp ignore header files */
/*------------------------------*/

static boolean cppignhdrs
#ifdef CPP_IGN_HDRS
			    = 1
#endif
;


/*-----------------------------------------------*/
/* nifty little function for handling I/O errors */
/*-----------------------------------------------*/

void my_perror(const char *action, const char *filename)
{   int err = errno;

    (void)fprintf(stderr,"%s: %s ", progname, action);
    errno = err;

    perror(filename);
}


/*---------------------------------------------------*/
/* write the #include lines as specified by the user */
/*---------------------------------------------------*/

void print_includes(FILE *f)
{    IncludeFile *incfile = (IncludeFile *)NULL;
    
    for (incfile = first_include; incfile; incfile=incfile->next)
    {   char *name = incfile->name;
	boolean surrounded = *name == '"' || *name == '<';
	
	(void)fputs("#include ", f);

	if (!surrounded)
           (void)fputc('<',f);
	(void)fputs(name, f);

	if (!surrounded)
           (void)fputc('>',f);
	(void)fputc('\n',f);
    }
}


/*-----------*/
/* No memory */
/*-----------*/

void outmem(void)
{   (void)fprintf(stderr,"%s: Out of memory!\n", progname);
    exit(1);
}


/*---------------------*/
/* Safe malloc routine */
/*---------------------*/

#ifndef DBMALLOC
void *safe_malloc(size_t size)
{   void *mem = (void *)NULL;

    if ((mem = (void *)malloc(size)) == (void *)NULL)
	outmem();

    return mem;
}
#endif /* DBMALLOC */


/*-------------------*/
/* Safe free routine */
/*-------------------*/

void safe_free(void *p)
{   do {
           if (p) (void)free((void *)p);
              p = (void *)NULL;
       } while(0);
}


/*--------------------------------------------------------------------------*/
/* Replace any character escape sequences in a string with the actual       */
/* characters.  Return a pointer to malloc'ed memory containing the result. */
/* This function knows only a few escape sequences.                         */
/*--------------------------------------------------------------------------*/

static char *escape_string (char *src)
{
    char *result = (char *)NULL,
         *get    = (char *)NULL,
         *put    = (char *)NULL;

    result = strduplicate(src);
    put    = result;
    get    = src;

    while (*get != '\0')
    {     

          /*------------------*/
          /* Control sequence */
          /*------------------*/

          if (*get == '\\')
	  {   switch (*(++get))
	      {

                     /*---------*/
                     /* Newline */
                     /*---------*/

                     case 'n':
		                    *put++ = '\n';
		                    ++get;
		                    break;


                     /*----------------*/
                     /* Horizontal tab */
                     /*----------------*/

	             case 't':
		                    *put++ = '\t';
		                    ++get;
		                    break;


                     /*--------------*/
                     /* Vertical tab */
                     /*--------------*/

	             case 'v':
		                    *put++ = '\v';
		                    ++get;
		                    break;


                    /*--------------------------------*/
                    /* Ignore other control sequences */
                    /*--------------------------------*/

	            default:
		                   if (*get != '\0')
		                      *put++ = *get++;
	      }
	}


        /*---------------------*/
        /* Printable character */
        /*---------------------*/

        else
	    *put++ = *get++;
    }

    *put = *get;
    return result;
}


/*-------------------------------*/
/* Output usage message and exit */
/*-------------------------------*/

static void usage (void)
{
    unsigned int i;

    (void)fprintf(stderr, "usage: %s [ option ... ] [ file ... ]\n", progname);
    (void)fputs(" -o directory\twrite output files in directory\n",stderr);
    (void)fputs(" -p\t\tdisable prototype promotion\n", stderr);
    (void)fputs(" -s\t\toutput static declarations\n", stderr);
    (void)fputs(" -v\t\toutput variable declarations\n", stderr);
    (void)fputs(" -k\t\tdon't attempt to fixup comments\n", stderr);
    (void)fputs(" -b\t\tlook for descriptions at top of function bodies\n", stderr);
    (void)fputs(" -B\t\tonly look for descriptions by applying -b\n", stderr);
    (void)fputc('\n', stderr);
    (void)fputs(" -i incfile\n", stderr);
    (void)fputs(" -i \"incfile\"\n", stderr);
    (void)fputs(" -i <incfile>\tadd #include for incfile to SYNOPSIS\n", stderr);
    (void)fputc('\n', stderr);
    (void)fputs(" -H prefix\tspecify prefix for #include in SYNOPSIS\n", stderr);
    (void)fputc('\n', stderr);
    (void)fputs(" -g\n", stderr);
    (void)fputs(" -G terse\tgroup info from each file into a single page\n", stderr);
    (void)fputs(" -e\t\tmake embeddable files\n", stderr);
    (void)fputc('\n', stderr);
    (void)fputs(" -l ", stderr);

    #ifdef HAS_LINK
    (void)fputs("h|", stderr);
    #endif

    #ifdef HAS_SYMLINK
    (void)fputs("s|", stderr);
    #endif

    (void)fputs("f|n|r\t", stderr);
    (void)fputs("linking for grouped pages: ", stderr);

    #ifdef HAS_LINK
    (void)fputs("hard, ", stderr);
    #endif

    #ifdef HAS_SYMLINK
    (void)fputs("soft, ", stderr);
    #endif

    (void)fputs("file, none or remove\n", stderr);
    (void)fputs(" -n\t\tName output file after input source file\n", stderr);
    (void)fputs(" -L\t\tLazy: Be silent about undocumented parameters\n", stderr);
    (void)fputs(" -T n|l|h|t|a[,options]\tselect typesetting output format: nroff, LaTeX, HTML ,TeXinfo or AutoDoc\n", stderr);

    nroff_output.print_options();
    latex_output.print_options();
    html_output.print_options();
    texinfo_output.print_options();
    autodoc_output.print_options();

    (void)fputs(" -M name\tset name of the manual in which the page goes\n", stderr);
    (void)fputs(" -x section\texclude section from ouput\n", stderr);
    (void)fputc('\n', stderr);
    (void)fputs(" -D name[=value]\n", stderr);
    (void)fputs(" -U name\n", stderr);
    (void)fputs(" -I directory\tC preprocessor options\n", stderr);
    (void)fputc('\n', stderr);
    (void)fputs(" -F template\tset prototype template in the form ", stderr);
    (void)fputs("\"int f (a, b)\"\n",stderr);
    (void)fputs(" -P preprocessor\tAlternate C preprocessor ", stderr);
    (void)fputs("(e.g., \"gcc -E -C\")\n", stderr);
    (void)fputs(" -V\t\tbe verbose and print version information\n", stderr);
    (void)fputs(" -S section\tset the section for the manual page (default = 3)\n",stderr);
    (void)fputs(" -O ", stderr);

    for (i = 0; i < _OBJECT_NUM; i++)
	(void)fputc(output_object[i].flag, stderr);

    (void)fputs("[subdir][.ext]", stderr);
    (void)fputs("\tOutput control over different object types:\n\t\t", stderr);
    for (i = 0; i < _OBJECT_NUM; i++)
    {   (void)fputs(output_object[i].name, stderr);

	if (i <= _OBJECT_NUM - 2)
	    (void)fprintf(stderr,i == _OBJECT_NUM-2 ? " or " : ", ");
    }
    (void)fputs(".\n", stderr);

    exit(1);
}


/*-----------------------------------------------------------------------------*/
/* name of the temporary file; kept here so we can blast it if hit with ctrl-C */
/*-----------------------------------------------------------------------------*/

static char temp_name[20] = "";
Signal_t (*old_interrupt_handler)();


/*-------------------------------------------------------------*/
/* ctrl-C signal handler for use when we have a temporary file */
/*-------------------------------------------------------------*/

static Signal_t interrupt_handler(const int signum)
{   (void)unlink(temp_name);
    exit(128 + signum);
}


/*------------------------------*/
/* open a unique temporary file */
/*------------------------------*/
/*----------------------------------------------------------------------------*/
/* To be universally accepted by cpp's, the file's name must end in .c; so we */
/* can't use mktemp, tmpnam or tmpfile.                                       */
/* returns an open stream & sets ret_name to the name.                        */
/*----------------------------------------------------------------------------*/

FILE *open_temp_file(void)
{
    des_t fd     = 0;
    pid_t n      = getpid();
    FILE  *tempf = (FILE *)NULL;

    boolean remove_temp_file();


    /*----------------------------------------------------------------*/
    /* keep generating new names until we hit one that does not exist */
    /*----------------------------------------------------------------*/

    do
    {

        /*---------------------------------------------------------------------*/
	/* ideally we'd like to put the temporary file in /tmp, but it must go */
	/* in the current directory because when cpp processes a #include, it  */
	/* looks in the same directory as the file doing the include; so if we */
	/* use /tmp/blah.c to fake reading fred.h via `#include "fred.h"', cpp */
	/* will look for /tmp/fred.h, and fail.                                */
	/*---------------------------------------------------------------------*/

	(void)snprintf(temp_name,20,"c2man%ld.c",(long int)n++ % 1000000);

  
        #ifdef HAS_OPEN3
        /*---------*/
        /* New way */
        /*---------*/

        fd = open(temp_name,O_WRONLY|O_CREAT|O_EXCL,0666);

        #else 
        /*---------*/
        /* Old way */
        /*---------*/

        fd = creat(temp_name,O_EXCL|0666);
        #endif /* HAS_OPEN3 */
       
    } while (fd == (-1) && errno == EEXIST);


    /*--------------------------------------------------------*/
    /* install interrupt handler to remove the temporary file */
    /*--------------------------------------------------------*/

    old_interrupt_handler = signal(SIGINT, interrupt_handler);


    /*------------------------*/
    /* convert it to a stream */
    /*------------------------*/

    if ((fd == (-1) && errno != EEXIST) || (tempf = fdopen(fd, "w")) == (FILE *)NULL)
    {   my_perror("error fdopening temp file",temp_name);
	remove_temp_file();
	return (FILE *)NULL;
    }

    return tempf;
}


/*-----------------------------------------------------*/
/* remove the temporary file & restore ctrl-C handler. */
/* returns FALSE in the event of failure.              */
/*-----------------------------------------------------*/

boolean remove_temp_file(void)
{

    /*----------------------------*/
    /* this should always succeed */
    /*----------------------------*/

    int ok = unlink(temp_name) == 0; 
    (void)signal(SIGINT, old_interrupt_handler);

    return ok;
}


/*-----------------------------------------------------------------------------*/
/* process the specified source file through the pre-processor.                */
/* This is a lower level routine called by both process_stdin and process_file */
/* to actually get the work done once any required temporary files have been   */
/* generated.                                                                  */
/*-----------------------------------------------------------------------------*/

int process_file_directly(const char *base_cpp_cmd,
                          const char *name        )
{
    char *full_cpp_cmd = (char *)NULL;

    #ifdef DEBUG
    (void)fprintf(stderr,"process_file_directly: %s, %s\n", base_cpp_cmd, name);
    #endif /* DEBUG */

    full_cpp_cmd = strconcat(base_cpp_cmd, " ", name, NULLCP);
    if (verbose)
	(void)fprintf(stderr,"%s: running `%s'\n", progname, full_cpp_cmd);

    if ((yyin = popen(full_cpp_cmd, "r")) == (FILE *)NULL)
    { 
	my_perror("error running", base_cpp_cmd);
	(void)safe_free(full_cpp_cmd);  // MAO was free()

	return 0;
    }

    parse_file(name);

    (void)safe_free(full_cpp_cmd); // MAO was free()
    if (pclose(yyin) & 0xFF00)
	return 0;

    return !errors;
}


/*--------------------------*/
/* process a specified file */
/*--------------------------*/

int process_file(const char *base_cpp_cmd,
                 const char *name        )

{   char        *period = (char *)NULL;
    struct stat statbuf;
    
    #ifdef DEBUG
    (void)fprintf(stderr,"process_file: %s, %s\n", base_cpp_cmd, name);
    #endif /* DEBUG */

    basefile    = name;
    header_file = (period = strrchr(name,'.')) && (period[1] == 'h' || period[1] == 'H');


    /*----------------------------------------------------*/
    /* use the file's date as the date in the manual page */
    /*----------------------------------------------------*/

    if (stat(name,&statbuf) != 0)
    {    my_perror("can't stat", name);
	return 0;
    }
    basetime = statbuf.st_mtime;


    /*---------------------------------------------------------------------*/
    /* should we do this via a temporary file?                             */
    /* Only if it's a header file and either CPP ignores them, or the user */
    /* has specified files to include.                                     */
    /*---------------------------------------------------------------------*/

    if (header_file && (cppignhdrs || first_include))
    {   FILE *tempf = (FILE *)NULL;
	int  ret;

	if (verbose)
	    fprintf(stderr, "%s: preprocessing via temporary file\n", progname);

	if ((tempf = open_temp_file()) == (FILE *)NULL)
	    return 0;

	print_includes(tempf);
	if (verbose)
           print_includes(stderr);

	(void)fprintf(tempf,"#include \"%s\"\n", basefile);
	if (verbose)
           (void)fprintf(stderr,"#include \"%s\"\n", basefile);

	if (fclose(tempf) == EOF)
	{
	    my_perror("error closing temp file", temp_name);
	    remove_temp_file();
	    return 0;
	}


        /*------------------------------------------------------------*/
	/* since we're using a temporary file, it's not the base file */
        /*------------------------------------------------------------*/

	inbasefile = 0;
	ret        = process_file_directly(base_cpp_cmd, temp_name);

	remove_temp_file();
	return ret;
    }


    /*--------------------------------*/
    /* otherwise, process it directly */
    /*--------------------------------*/

    inbasefile = 1;

    return process_file_directly(base_cpp_cmd,name);
}


/*------------------------*/
/* process standard input */
/*------------------------*/

int process_stdin(const char *base_cpp_cmd)
{
    if (isatty(fileno(stdin)))
	(void)fprintf(stderr,"%s: reading standard input\n", progname);


    /*----------------------------------------*/
    /*  assume it's not since it's from stdin */
    /*----------------------------------------*/

    header_file = 0;
    basefile    = NULL;


    /*--------------------------------------*/
    /* use the current date in the man page */
    /*--------------------------------------*/

    basetime = time((time_t *)NULL);

                                /*------------------------------------------*/
    inbasefile = 1;		/* reading stdin, we start in the base file */
                                /*------------------------------------------*/

    /*------------------------------------------------------------------------*/
    /* always use a temp file if the preprocessor can't read stdin, otherwise */
    /* only use one if the user specified files for inclusion.                */
    /*------------------------------------------------------------------------*/

                                           /*---------------------------------*/ 
    if (!cppcanstdin || first_include)     /* did user specify include files? */
                                           /*---------------------------------*/
    {   FILE *tempf = (FILE *)NULL;

	int  c,
             ret;

	if (verbose)
	   (void)fprintf(stderr,"%s: reading stdin to a temporary file\n", progname);

	if ((tempf = open_temp_file()) == NULL)
	   return 0;

	print_includes(tempf);
	if (verbose)
           print_includes(stderr);

	(void)fprintf(tempf,"#line 1 \"stdin\"\n");

	while ((c = getchar()) != EOF)
	      putc(c,tempf);

	if (fclose(tempf) == EOF)
	{  my_perror("error closing temp file", temp_name);
	   remove_temp_file();
	   return 0;
	}

	ret = process_file_directly(base_cpp_cmd, temp_name);
	remove_temp_file();

	return ret;
    }
    else
    {  char *full_cpp_cmd = strconcat(base_cpp_cmd," ", CPP_STDIN_FLAGS, NULLCP);
    
       if (verbose)
          (void)fprintf(stderr,"%s: running `%s'\n", progname, full_cpp_cmd);
    
       if ((yyin = popen(full_cpp_cmd, "r")) == NULL)
       {   my_perror("error running", full_cpp_cmd);
	   return 0;
       }
    
       parse_file(basefile);
    
       (void)safe_free(full_cpp_cmd);  // MAO was free()
       if (pclose(yyin) & 0xFF00)
          return 0;
    
       return !errors;
    }
}


/*------------------*/
/* Main entry point */
/*------------------*/

int main ( int argc, char **argv)
{
    int i,
        c,
        ok                         = 0;

    char *s                        = (char *)NULL,
         *cpp_opts                 = (char *)NULL,
         cbuf[2]                   = "";

    const char     *base_cpp_cmd   = (char *)NULL;
    IncludeFile    *includefile    = (IncludeFile *)NULL;
    ExcludeSection *excludesection = (ExcludeSection *)NULL;

    #ifdef HAS_LINK
                                                /*----------*/
    enum LinkType link_type = LINK_HARD;	/* for -g/G */
                                                /*----------*/
    #else
    enum LinkType link_type = LINK_FILE;
    #endif /* HAS_LINK */

    #ifdef YYDEBUG
    extern int yydebug;
    #endif /* YYDEBUG */


    /*-----------------------------------------*/
    /* initialise CPP options with -D__C2MAN__ */
    /*-----------------------------------------*/

    cbuf[0]  = VERSION + '0';
    cbuf[1]  = '\0';
    cpp_opts = strconcat("-D_C2MAN__=", cbuf, NULLCP);
 

    /*---------------------------*/
    /* Scan command line options */
    /*---------------------------*/

    while ((c = getopt(argc, argv, "P:D:F:I:psU:Vvo:eM:H:G:gi:x:S:l:LT:nO:kbB")) != EOF)
    {
	switch (c)
        {      case 'I':
	       case 'D':
	       case 'U':
	                    cbuf[0] = c; cbuf[1] = '\0';
	                    if (cpp_opts)
		               cpp_opts = strappend(cpp_opts," -",cbuf,optarg,NULLCP);
	                    else
		               cpp_opts = strconcat("-",cbuf,optarg,NULLCP);
	                    break;
	       case 'P':
	                    cpp_cmd = optarg;

                            /*-----------------------------------------------------------*/
	                    /* with no better info to go on, we have to assume that this */
	                    /* preprocessor is minimally capable.                        */                
	                    /*-----------------------------------------------------------*/
	                    cppcanstdin = 0;
	                    cppignhdrs  = 1;
	                    break;
              case 'G':
	                    group_terse     = optarg;
	                    terse_specified = TRUE;


              /*-------------*/ 
              /* FALLTHROUGH */
              /*-------------*/ 

              case 'g':
	                    group_together = TRUE;
	                    break;

              case 'F':
                            s = escape_string(optarg);

                            decl_spec_prefix = s;
                            while (*s != '\0' && isascii(*s) && !isalnum(*s))
                                  ++s;

                            if (*s == '\0') usage();
	                       *s++ = '\0';

	                    while (*s != '\0' && isascii(*s) && isalnum(*s))
                                  ++s;

                            if (*s == '\0')
                               usage();

                            declarator_prefix = s;
                            while (*s != '\0' && isascii(*s) && !isalnum(*s))
                                  ++s;

                            if (*s == '\0')
                               usage();

                            *s++ = '\0';
                            while (*s != '\0' && isascii(*s) && isalnum(*s))
                                  ++s;

                            if (*s == '\0')
                               usage();

                            declarator_suffix = s;
                            while (*s != '\0' && *s != '(')
                                  ++s;

                            if (*s == '\0')
                               usage();
                            *s++ = '\0';

                            first_param_prefix = s;
                            while (*s != '\0' && isascii(*s) && !isalnum(*s))
                                  ++s;

 	                    if (*s == '\0')
                               usage();
	                    *s++ = '\0';

	                    while (*s != '\0' && *s != ',')
                                  ++s;

	                    if (*s == '\0')
                               usage();

	                    middle_param_prefix = ++s;
	                    while (*s != '\0' && isascii(*s) && !isalnum(*s))
                                  ++s;

                            if (*s == '\0')
                               usage();
	                    *s++ = '\0';

	                    while (*s != '\0' && isascii(*s) && isalnum(*s))
                                  ++s;
	                    if (*s == '\0')
                               usage();

	                    last_param_suffix = s;
	                    while (*s != '\0' && *s != ')')
                                  ++s;
	                    *s = '\0';
	                    break;

	        case 'p':
	                    promote_param = FALSE;
	                    break;

	        case 's':
	                    static_out = TRUE;
	                    break;

                case 'V':
	                    verbose = TRUE;
	                    (void)fprintf(stderr, "%s: Version %d, Patchlevel %d\n", progname, VERSION, PATCHLEVEL);
	                    break;

	        case 'v':
	                    variables_out = TRUE;
	                    break;

	        case 'k':
	                    fixup_comments = FALSE;
	                    break;

                case 'o':
	                    output_dir = optarg;
	                    break;

	        case 'M':
	                    manual_name = optarg;
	                    break;

	        case 'H':
	                    header_prefix = optarg;
	                    break;

	        case 'i':
	                    *last_next_include = includefile = (IncludeFile *)safe_malloc(sizeof *includefile);
	                    includefile->name  = optarg;
	                    includefile->next  = NULL;
	                    last_next_include  = &includefile->next;
	                    break;

	        case 'x':
                            *last_next_excluded_section = excludesection = (ExcludeSection *)safe_malloc(sizeof *excludesection);
	                    excludesection->name                         = optarg;
	                    excludesection->next                         = NULL;
	                    last_next_excluded_section                   = &excludesection->next;

	                    break;

	        case 'S':
	                    manual_section = optarg;
	                    break;


                /*--------------------------------------------*/
                /* This is NOT good programming practice YUK! */
                /*--------------------------------------------*/

                case 'l':
	                    switch (optarg[0])
	                    {
                                   #ifdef HAS_LINK
	                           case 'h':    link_type = LINK_HARD;
                                                break;
                                   #endif /* HAS_LINK */

                                   #ifdef HAS_SYMLINK
                                   case 's':    link_type = LINK_SOFT;
                                                break;
                                   #endif /* HAS_SYMLINK */

                                   case 'f':    link_type = LINK_FILE;
                                                break;

                                  case 'n':     link_type = LINK_NONE;
                                                break;

                                  case 'r':     link_type = LINK_REMOVE;
                                                break;

                                  default:      usage();
                            }
	                    break;

	      case 'e':
	                    make_embeddable = TRUE;
	                    break;

              case 'n':
                            use_input_name = TRUE;
                            break;

              case 'L':     always_document_params = FALSE;
	                    break;

              case 'T':
                            switch (optarg[0])
                            {
	                           case 'n':    output = &nroff_output;	default_section = "3";
			           break;

	                           case 'l':    output = &latex_output;	default_section = "tex";
			                        break;

                                   case 't':    output = &texinfo_output; default_section = "texi";
			                        break;

                                   case 'h':    output = &html_output; default_section = "html";
			                        break;

                                   case 'a':   output = &autodoc_output; default_section = "doc";
                                               break;

                                   default:    usage();
	                     }


	                     s = strtok(&optarg[1], ",");
	                     if (s && *output->parse_option == NULL)
                                usage();

	                     while (s)
	                     {     if (output->parse_option(s))
                                      usage();
	                           s = strtok(NULL, ",");
	                     }
	                     break;

	        case 'O':
	                     for (i = 0; i < _OBJECT_NUM; i++)
		             {   if (output_object[i].flag == optarg[0])
		                   break;
                             }

	                     if (i == _OBJECT_NUM)
	                     {  (void)fprintf(stderr,"%s: -O option must specify one of:\n\t",progname);

                                for (i = 0; i < _OBJECT_NUM; i++)
		                {   (void)fprintf(stderr,"%c (%s)", output_object[i].flag, output_object[i].name);
		                    if (i <= _OBJECT_NUM - 2)
			               (void)fprintf(stderr,i == _OBJECT_NUM-2 ? " or " : ", ");
		                }
		                (void)fprintf(stderr, ".\n");
		                exit(1);
	                     }


                             /*-----------------------*/
                             /* Look for an extension */
                             /*-----------------------*/

	                     if ((s = strchr(++optarg,'.')))
                             {  output_object[i].subdir = alloc_string(optarg, s);
		                output_object[i].extension = strduplicate(s+1);
	                     }
                             else
                                output_object[i].subdir = strduplicate(optarg);
	                     break;
                case 'b':
                             look_at_body_start = TRUE;
                             break;

	        case 'B':
                             body_start_only   = TRUE;
                             look_at_body_start = TRUE;
                             break;

                case '?':
	        default:
	                     usage();
	}
    }

    /*------------------------------------*/
    /* make sure we have a manual section */
    /*------------------------------------*/

    if (manual_section == (char *)NULL)
       manual_section = default_section;


    /*-----------------------------------------------*/
    /* wait so we can start up NeXT MallocDebug tool */
    /*-----------------------------------------------*/

    #ifdef MALLOC_DEBUG
                        /*-----------------------------------------------*/
    (void)getchar();	/* wait so we can start up NeXT MallocDebug tool */
                        /*-----------------------------------------------*/

    #endif /* MALLOC_DEBUG */

    #ifdef YYDEBUG
    yydebug = 1;
    #endif /* YYDEBUG */

    if (cpp_opts)
    {   base_cpp_cmd = strconcat(cpp_cmd, " ", cpp_opts, NULLCP);
        safe_free(cpp_opts); // MAO was free()
    }
    else
       base_cpp_cmd = cpp_cmd;

    if (optind == argc)
    {  if (use_input_name)
       {  (void)fprintf(stderr,"%s: %s\n", progname,
                        "cannot name output after input file if there isn't an input file!");
           usage();
	}
	ok = process_stdin(base_cpp_cmd);
    }    
    else
    {   for (i = optind; i < argc; ++i)
	    if (!(ok = process_file(base_cpp_cmd,argv[i])))
               break;
    }

    if (ok && firstpage)
	output_manual_pages(firstpage,argc - optind, link_type);
    free_manual_pages(firstpage);
    destroy_enum_lists();

    if (cpp_opts)
       safe_free((char *)base_cpp_cmd); // MAO was free()

    for (includefile = first_include; includefile;)
    {   IncludeFile *next = includefile->next;
	safe_free(includefile); // MAO was free()

	includefile        = next;
    }

    for (excludesection = first_excluded_section; excludesection;)
    {    ExcludeSection *next = excludesection->next;
	safe_free(excludesection); // MAO was free()

	excludesection = next;
    }

    for (i = 0; i < _OBJECT_NUM; i++)
    {   safe_free(output_object[i].subdir);
	safe_free(output_object[i].extension);
    }

    #ifdef DBMALLOC
    malloc_dump(2);
    malloc_chain_check(1);
    #endif /* DBMALLOC */

    #ifdef MALLOC_DEBUG
    sleep(1000000);
    #endif /* MALLOC_DEBUG */

    return !ok;
}
