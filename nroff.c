/* $Id: nroff.c,v 1.1 2024-01-03 13:07:20 behdad Exp $
 * functions for nroff style output.
 */
#include "c2man.h"
#include "manpage.h"
#include "output.h"
#include "semantic.h"
#include <ctype.h>
#include <time.h>


/*-------------------*/
/* Output nroff text */
/*-------------------*/

void nroff_text( const char *text)
{    put_string(text);
}


/*------------------------*/
/* Output nroff character */
/*------------------------*/

void nroff_char(const int c)
{   putchar(c);
}


/*---------------*/
/* nroff comment */
/*---------------*/

void nroff_comment(void)
{   put_string(".\\\" ");
}


/*--------------*/
/* nroff header */
/*--------------*/

void nroff_header(ManualPage *firstpage,
                  int        input_files,
                  boolean    grouped,
                  const char *name,
                  const char *terse,
                  const char *section)
{
    #ifdef HAS_STRFTIME
    char month[20];
    #else
    char *month;
    static char *month_list[] =
    { "January", "February", "March", "April", "May", "June",
      "July", "August", "September", "October", "November", "December" };
    #endif /* HAS_STRFTIME */

    time_t     raw_time;
    struct tm *filetime = (struct tm *)NULL;
    
    if (make_embeddable)
       return;

    output_warning();
    put_string(".TH \"");


    /*-----------------------------------------------------------------------*/
    /* if lots of files contributed, use the current time; otherwise use the */
    /* time of the source file they came from.                               */
    /*-----------------------------------------------------------------------*/

    raw_time = time((time_t *)NULL); //(grouped && input_files > 1) ? time((time_t *)NULL) : firstpage->sourcetime;

    //raw_time = time((time_t *)NULL);
    filetime = localtime(&raw_time);

    #ifdef HAS_STRFTIME
    /*---------------------------------*/
    /* generate the date format string */
    /*---------------------------------*/

    (void)strftime(month, sizeof month,"%B",filetime);
    #else
    month = month_list[filetime->tm_mon];
    #endif /* HAS_STRFTIME */

    nroff_text(name);

    (void)fprintf(stdout,"\" %s \"%d %s %d\"",section,filetime->tm_mday,month,filetime->tm_year+1900);

    /*-----------------------------------------------------------*/
    /* I have conflicting info about how the .TH macro works.... */
    /*-----------------------------------------------------------*/

                            /*-----------------------------------------------------*/ 
    #ifdef HENRYS_TH        /* As per Henry Spencer's "How to write a manual page" */
                            /*-----------------------------------------------------*/ 
    if (manual_name)
       (void)fprintf(stdout," \"%s\"", manual_name);
    put_string("\n.BY");    
    #endif /* HENRYS_TH */

    (void)fprintf(stdout," \"%s",progname);
     if ((input_files <= 1 || !grouped) && firstpage->sourcefile)
     {   const char *basename = strrchr(firstpage->sourcefile, '/');
         if (basename == (char *)NULL)
             basename = firstpage->sourcefile;
	 else
	    basename++;
	 (void)fprintf(stdout," %s", basename);
     }

     #ifndef HENRYS_TH
     if (manual_name)
        (void)fprintf(stdout,"\" \"%s", manual_name);
     #endif /* HENRYS_TH */
     put_string("\"\n");    
}


/*------------*/
/* nroff dash */
/*------------*/

void nroff_dash(void)
{   put_string("\\-");
}


/*---------------*/
/* nroff section */
/*---------------*/

void nroff_section(const char *name)
{   put_string(".SH \"");
    nroff_text(name);
    put_string("\"\n");
}


/*-------------------*/
/* nroff sub-section */
/*-------------------*/

void nroff_sub_section(const char *name)
{   put_string(".SS \"");
    nroff_text(name);
    put_string("\"\n");
}


/*------------------*/
/* nroff break line */
/*------------------*/

void nroff_break_line(void)
{   put_string(".br\n");
}


/*------------------*/
/* nroff blank line */
/*------------------*/
void nroff_blank_line(void)
{   put_string(".sp\n");
}


/*------------------*/
/* nroff code start */
/*------------------*/

void nroff_code_start(void)
{   put_string(".ft B\n");
}


/*----------------*/
/* nroff code end */
/*----------------*/

void nroff_code_end(void)
{   put_string(".ft R\n");
}


/*---------------------*/
/* nroff code (output) */
/*---------------------*/

void nroff_code(const char *text)
{   put_string("\\fB");
    nroff_text(text);
    put_string("\\fR");
}


/*-----------------------*/
/* nroff tag entry start */
/*-----------------------*/

void nroff_tag_entry_start(void)
{    put_string(".TP\n.B \"");
}


void nroff_tag_entry_start_extra(void)
{   put_string(".TP\n.BR \"");
}


/*---------------------*/
/* nroff tag entry end */
/*---------------------*/

void nroff_tag_entry_end(void)
{    put_string("\"\n");
}

void nroff_tag_entry_end_extra(const char *text)
{   put_string("\" \"\t(");
    nroff_text(text);
    put_string(")\"\n");
}


/*-------------------*/
/* nroff table start */
/*-------------------*/

void nroff_table_start(const char *longestag)
{
    void nroff_list_start();
    nroff_list_start();

    /*--------------------------------------------------------------------------*/
    /* We measure the length of the longest tag in the table by changing to the */
    /* code font, taking it's width with \w'string' and adding a little for     */
    /* the space between the tag and description.  This gets stored in the TL   */
    /* number register, where the nroff_table_entry can find it.                */
    /* This isn't foolproof, because a shorter tag may be longer if it contains */
    /* wider characters, but the extra space gives a little head room anyway.   */
    /*--------------------------------------------------------------------------*/

    nroff_code_start();
    (void)fprintf(stdout,".nr TL \\w'%s'u+0.2i\n", longestag);
    nroff_code_end();
}


/*-------------------*/
/* nroff table entry */
/*-------------------*/

void nroff_table_entry(const char *name, const char *description)
{
    put_string(".TP \\n(TLu\n");

    nroff_code(name);
    nroff_char('\n');
    if (description)
	output_comment(description);
    else
	nroff_char('\n');
}


/*------------------*/
/* nroff tabble end */
/*------------------*/

void nroff_table_end(void)
{   put_string(".RE\n.PD\n");
}


/*-------------------*/
/* nroff indent line */
/*-------------------*/

void nroff_indent()
{   put_string(".IP\n");
}


/*------------------*/
/* nroff list start */
/*------------------*/

void nroff_list_start(void)
{   put_string(".RS 0.75in\n.PD 0\n");
}


/*------------------*/
/* nroff list entry */
/*------------------*/

void nroff_list_entry(const char *name)
{   nroff_code(name);
}



/*----------------------*/
/* nroff list separator */
/*----------------------*/

void nroff_list_separator(void)
{   put_string(",\n");
}

void nroff_list_end()	{ nroff_char('\n'); nroff_table_end(); }

void nroff_include(filename)
const char *filename;
{
    printf(".so %s\n", filename);
}

void nroff_name(name)
const char *name;
{
    if (name) nroff_text(name);
    else      nroff_section("NAME");
}


/*-----------------------*/
/* nroff terse separator */
/*-----------------------*/

void nroff_terse_sep(void)
{   nroff_char(' ');
    nroff_dash();
    nroff_char(' ');
}


/*------------*/
/* nroff bold */
/*------------*/

void nroff_emphasized(const char *text)
{   put_string("\\fI");
    nroff_text(text);
    put_string("\\fR");
}


/*-------------------------*/
/* nroff reference (c2man) */
/*-------------------------*/

void nroff_reference(const char *text)
{   nroff_text(text);
    nroff_char('(');
    nroff_text(manual_section);
    nroff_char(')');
}


/*---------------------------*/
/* nroff description (c2man) */
/*---------------------------*/

void nroff_description(const char *text)
{   enum { TEXT, PERIOD, CAPITALISE } state = CAPITALISE;

    boolean new_line    = TRUE;
    boolean dot_command = FALSE;
  

    /*---------------------------------------------*/
    /* correct punctuation a bit as text is outout */
    /*---------------------------------------------*/

    for (;*text;text++)
    {   int c = *text;

	if (dot_command)
	{  if (c == '\n')
              dot_command = FALSE;
	}
	else if (new_line && c == '.')
	    dot_command = TRUE;
	else if (new_line && (c == '-' || c == '*' || is_numbered(text)))
	{   output->break_line();
	   state = CAPITALISE;
	}
	else if (c == '.')
	   state = PERIOD;
	else if (isspace(c) && state == PERIOD)
	   state = CAPITALISE;
	else if (isalnum(c))
	{  if (islower(c) && state == CAPITALISE)	c = toupper(c);
	      state = TEXT;
	}
           
	output->character(c);
	new_line = c == '\n';
    }


    /*------------------------------------*/
    /* do a full stop if there wasn't one */
    /*------------------------------------*/

    if (!dot_command && state == TEXT)
       output->character('.');
}

void nroff_returns(const char *comment)
{
    enum { TEXT, PERIOD, CAPITALISE } state = CAPITALISE;

    char    lastchar         = '\n';
    boolean tag_list_started = FALSE;


    /*------------------*/
    /* for each line... */
    /*------------------*/

    while (*comment)
    {
	boolean tagged = FALSE;

        /*--------------------------------*/
	/* explicitly reject dot commands */
        /*--------------------------------*/

	if (*comment && *comment != '.')
	{   const char *c = comment;


            /*--------------------------------------*/
	    /* search along until the end of a word */
            /*--------------------------------------*/

	    while (*c && *c != ':' && !isspace(*c))
		c++;


            /*----------------------------------------------*/
	    /* skip all spaces or tabs after the first word */
            /*----------------------------------------------*/

	    while (*c && *c != '\n')
	    {   if (*c == '\t' || *c == ':')
		{   tagged = TRUE;
		    break;
		}
		else if (!isspace(*c))
		    break;
		c++;
	    }
	}


        /*-----------------------------------------------*/
	/* is it tagged?; explicitly reject dot commands */
        /*-----------------------------------------------*/

	if (tagged)
	{

            /*-------------------------------------*/
	    /* output hanging newline if necessary */
            /*-------------------------------------*/

	    if (lastchar != '\n')
	    {  if (state == TEXT && !ispunct(lastchar))
                  output->character('.');
               output->character(lastchar = '\n');
	    }

	    if (!tag_list_started)
	    {   output->tag_list_start();
                tag_list_started = TRUE;
	    }


            /*-----------------------*/
	    /* output the tagged bit */
            /*-----------------------*/

	    output->tag_entry_start();
	    while (*comment && *comment != ':' && !isspace(*comment))
		  output->character(*comment++);
	    output->tag_entry_end();

            /*-------------------------------*/
	    /* skip any extra tabs or spaces */
            /*-------------------------------*/

	    while (*comment == ':' || (isspace(*comment) && *comment != '\n'))
		  comment++;

	    state = CAPITALISE;
	}


        /*------------------------------------------*/
	/* terminate the previous line if necessary */
        /*------------------------------------------*/

	if (lastchar != '\n')
           output->character(lastchar = '\n');


        /*-----------------------------------*/
	/* dot commands are output unaltered */
        /*-----------------------------------*/

	if (*comment == '.')
	{   for (;*comment && *comment != '\n'; comment++)
		output->character(*comment);
	    output->character('\n');
	}
	else
	{
            /*---------------------------------------------*/
	    /* correct punctuation a bit as line is output */
            /*---------------------------------------------*/

	    for (;*comment && *comment != '\n'; comment++)
	    {   char c = *comment;

		if (c == '.')
		   state = PERIOD;
		else if (isspace(c) && state == PERIOD)
		   state = CAPITALISE;
		else if (isalnum(c))
		{  if (islower(c) && state == CAPITALISE && fixup_comments)
		      c = toupper(c);
		   state = TEXT;
		}

		output->character(lastchar = c);
	    }

            /*--------------------------------------------------------------*/
	    /* if it ended in punctuation, just output the nl straight away */
            /*--------------------------------------------------------------*/

	    if (ispunct(lastchar))
	    {  if (lastchar == '.')
                  state = CAPITALISE;
		output->character(lastchar = '\n');
	    }
	}

	if (*comment)
           comment++;
    }


    /*---------------------------------------*/
    /* output lingering newline if necessary */
    /*---------------------------------------*/

    if (lastchar != '\n')
    {  if (state == TEXT && !ispunct(lastchar) && fixup_comments)
	  output->character('.');
       output->character('\n');
    }

    if (tag_list_started)
       output->tag_list_end();
}


/*------------------------*/
/* nroff output structure */
/*------------------------*/

struct Output nroff_output =
{
        nroff_comment,
        nroff_header,
        nroff_dash,
        nroff_section,
        nroff_sub_section,
        nroff_break_line,
        nroff_blank_line,
        nroff_code_start,
        nroff_code_end,
        nroff_code,

                                       /*----------------------*/
        dummy,		               /* nroff_tag_list_start */
        dummy,		               /* nroff_tag_list_end   */
                                       /*----------------------*/

        nroff_tag_entry_start,
        nroff_tag_entry_start_extra,
        nroff_tag_entry_end,
        nroff_tag_entry_end_extra,
        nroff_table_start,
        nroff_table_entry,
        nroff_table_end,
        nroff_indent,
        nroff_list_start,

                                       /*---------------------*/
        nroff_code,		       /* nroff_list_entry    */
                                       /*---------------------*/

        nroff_list_separator,
        nroff_list_end,
        nroff_include,

                                       /*---------------------*/
        dummy,                         /* nroff_file_end      */
                                       /*---------------------*/

        nroff_text,
        nroff_char,

                                       /*---------------------*/
        NULL,                          /* nroff_parse_option  */
        dummy,                         /* nroff_print_options */
                                       /*---------------------*/

        nroff_name,
        nroff_terse_sep,
        nroff_reference,
        nroff_emphasized,
        nroff_description,
        nroff_returns
};
