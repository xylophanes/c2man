/*-----------------------------------------------------------------
** $PROJECT:
**
** $VER: autodoc.c 2.2 (25.01.95)
**
** by
**
** Stefan Ruppert , Windthorststra_e 5 , 65439 Flvrsheim , GERMANY
**
** (C) Copyright 1995
** All Rights Reserved !
**
** $HISTORY:
**
** 25.01.95 : 002.002 : changed to patchlevel 33
** 22.01.95 : 000.001 : initial
**
** Converted to ANSI-C by Mark O'Neill <mao@tumblingdice.co.uk>
----------------------------------------------------------------*/

#include "c2man.h"
#include "manpage.h"
#include "output.h"
#include <ctype.h>

#ifdef DEBUG
#define D(x)  x
#else
#define D(x)
#endif /* DEBUG */

#define MAX_TAG      10

static const int linelength   = 79;
static const int tablength    =  4;
static const int indentlength =  4;
static int indent             = 4;
static int list_indent        = 0;
static int column             = 0;
static int newline            = FALSE;
static int breakline          = FALSE;
static int see_also           = FALSE;
static int fileend            = FALSE;
static int acttable           = -1;

static int tablemaxtag[MAX_TAG];

void autodoc_format(const char *text)
{
    if(see_also)
    {  if(column + ((text) ? strlen(text) + 2 : 1) > linelength)
       {   (void)putc('\n',stdout);
           newline = TRUE;
       }
    }

    if(newline)
    {  int i;
       column = i = indent + list_indent;

       for(; i ; i--)
          (void)putc(' ',stdout);
       newline = FALSE;
    }
}


void autodoc_text(const char *text)
{
    int br = 1;
    autodoc_format(text);

    if(!see_also || (br = strcmp(text,",\n")))
    {   if(see_also < 2)
        {  (void)put_string(text);
           column += strlen(text);
        }
    }
    else if(!br)
    {
        column += 2;
        put_string(", ");
    }
}


void autodoc_char(const int c)
{
    if(c != '\f')
    {  autodoc_format(NULL);

       if(c == '\t')
       {  int i = tablength - (column % tablength);

          column += i;
          for(; i ; i--)
             (void)putc(' ',stdout);
       }
       else
       {
          if(see_also)
          {
             if(c == '(')
                 see_also++;
             else if(c == ')')
                 see_also--;
          }

          (void)putc(c,stdout);
          column++;
       }

       if((newline = (c == '\n')))
           column = 0;
   }
}

void autodoc_comment()
{ }


void autodoc_header(ManualPage *firstpage,
                    int        input_files,
                    boolean    grouped,
                    const char *name,
                    const char *terse,
                    const char *section)
{
    const char *basename = strrchr(firstpage->sourcefile, '/');
    int len;
    int spc;

    fileend = FALSE;
    if(basename && *basename == '/')
        basename++;

    len =  ((basename) ? strlen(basename) + 1 : 0) + strlen(name);
    spc  = linelength - 2 * len;

    see_also = FALSE;

    if(basename)
    {  autodoc_text(basename);
       autodoc_char('/');
    }
    autodoc_text(name);

    if(spc > 0)
    {
      while(spc)
      {
         autodoc_char(' ');
         spc--;
      }
       if(basename)
       {
          autodoc_text(basename);
          autodoc_char('/');
       }
       autodoc_text(name);
    } else
    {
       const char *ptr = name;
       len = linelength - 1 - len;

       while(len)
       {
          if(basename && *basename)
          {  autodoc_char(*basename);
             basename++;
          }
          else
          {
             if(ptr == name && basename)
                autodoc_char('/');
             else
             {
                autodoc_char(*ptr);
                ptr++;
             }
          }
          len--;
       }
    }

    put_string("\n");
}


void autodoc_dash()
{    put_string("-");
}


void autodoc_section(const char *name)
{
    D((fprintf(stderr,"section : %s\n",name)));
    newline  = FALSE;
    see_also = FALSE;
    put_string("\n");

    if(!strcmp(name,"DESCRIPTION"))
       name = "FUNCTION";
    else if(!strcmp(name,"PARAMETERS"))
       name = "INPUTS";
    else if(!strcmp(name,"RETURNS"))
       name = "RESULT";
    else if(!strcmp(name,"SEE ALSO"))
       see_also = TRUE;

    put_string("    ");
    autodoc_text(name);
    indent = 8;
    list_indent = 0;
    autodoc_char('\n');
}


void autodoc_sub_section(const char *name)
{   autodoc_text(name);
    indent = 12;
}

void autodoc_break_line(void)
{   breakline = TRUE;
}


void autodoc_blank_line(void)
{   autodoc_char('\n');
}


void autodoc_code_start()
{  }


void autodoc_code_end()
{  }

void autodoc_code(const char *text)
{   autodoc_text(text);
}


void autodoc_tag_entry_start(void)
{   if(list_indent > 0)
    {   autodoc_char('\n');
        list_indent -= indentlength;
    }
}


void autodoc_tag_entry_start_extra(void)
{   if(list_indent > 0)
    {   autodoc_char('\n');
        list_indent -= indentlength;
    }
}


void autodoc_tag_entry_end(void)
{  list_indent += indentlength;
   autodoc_char('\n');
}


void autodoc_tag_entry_end_extra(const char *text)
{   put_string("\" \"\t(");
    autodoc_text(text);
    put_string(")\"\n");
    list_indent += indentlength;
}
        

void autodoc_table_start(const char *longestag)
{  if(acttable < MAX_TAG - 1)
   {
      acttable++;
      tablemaxtag[acttable] = strlen(longestag);
   }

   indent += indentlength;
   newline = TRUE;
}

void autodoc_table_entry(const char *name, const char *description)
{   int i = tablemaxtag[acttable] - strlen(name) + 1;

    autodoc_code(name);
    while(i > 0)
    {  (void)putc(' ',stdout);
       i--;
    }
    (void)putc('-',stdout);
    (void)putc(' ',stdout);

    if (description)
        output_comment(description);
    else
        autodoc_char('\n');
}


void autodoc_table_end(void)
{   if(acttable > -1)
      acttable--;

    autodoc_char('\n');
    indent -= indentlength;
    if(list_indent > 0)
       list_indent -= indentlength;
}

void autodoc_indent(void)
{   int i;

    for(i = indent + list_indent; i ; i--)
       autodoc_char(' ');
}

void autodoc_list_start(void)
{  indent += indentlength;
   newline = TRUE;
}

void autodoc_list_entry(const char *name)
{   autodoc_code(name);
}


void autodoc_list_separator(void)
{    put_string(" ,");
}


void autodoc_list_end(void)
{   autodoc_char('\n');
    autodoc_table_end();
}

void autodoc_include(const char *filename)
{
}

void autodoc_terse_sep(void)
{   autodoc_char(' ');
    autodoc_dash();
    autodoc_char(' ');
}


void autodoc_name(const char *name)
{  if(name)
      autodoc_text(name);
   else
      autodoc_section("NAME");
}


void autodoc_file_end(void)
{  if(!fileend)
      (void)putc('\f',stdout);

   fileend = TRUE;
   newline = FALSE;
}


/*-----------------------------------------------------------------*/
/* ideally, this should be made aware of embedded autodoc commands */
/*-----------------------------------------------------------------*/

void autodoc_description(const char *text)
{   enum { TEXT, PERIOD, CAPITALISE } state = CAPITALISE;
    boolean new_line = TRUE;


    /*------------------------------------------*/    
    /* correct punctuation a bit as it goes out */
    /*------------------------------------------*/    

    for (;*text;text++)
    {   int c = *text;

	if (new_line && (c == '-' || c == '*'))
	{   output->break_line();
	    state = CAPITALISE;
	}
	else if (c == '.')
	    state = PERIOD;
	else if (isspace(c) && state == PERIOD)
	    state = CAPITALISE;
	else if (isalnum(c))
	{   if (islower(c) && state == CAPITALISE)	c = toupper(c);
	    state = TEXT;
	}
           
	output->character(c);
	new_line = c == '\n';
    }


    /*------------------------------------*/
    /* do a full stop if there wasn't one */
    /*------------------------------------*/

    if (state == TEXT)	output->character('.');
}


/*-----------------------------------------------------------------*/
/* ideally, this should be made aware of embedded autodoc commands */
/*-----------------------------------------------------------------*/

void autodoc_returns(const char *comment)
{   enum { TEXT, PERIOD, CAPITALISE } state = CAPITALISE;
    char lastchar                           = '\n';
    boolean tag_list_started                = FALSE;


    /*------------------*/
    /* for each line... */
    /*------------------*/

    while (*comment)
    {   boolean tagged = FALSE;

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
	    {
		if (*c == '\t' || *c == ':')
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
	    {   if (state == TEXT && !ispunct(lastchar))
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


        /*------------------------------------------------*/
	/* correct punctuation a bit as the line goes out */
        /*------------------------------------------------*/

	for (;*comment && *comment != '\n'; comment++)
	{   char c = *comment;

	    if (c == '.')
		state = PERIOD;
	    else if (isspace(c) && state == PERIOD)
		state = CAPITALISE;
	    else if (isalnum(c))
	    {   
		if (islower(c) && state == CAPITALISE && fixup_comments)
		    c = toupper(c);
		state = TEXT;
	    }

	    output->character(lastchar = c);
	}

        /*-------------------------------------------------------------------*/
	/* if it ended in punctuation, just output the newline straight away */
        /*-------------------------------------------------------------------*/

	if (ispunct(lastchar))
	{   if (lastchar == '.')
               state = CAPITALISE;
	    output->character(lastchar = '\n');
	}

	if (*comment)
           comment++;
    }


    /*-------------------------------------*/
    /* output hanging newline if necessary */
    /*-------------------------------------*/

    if (lastchar != '\n')
    {  if (state == TEXT && !ispunct(lastchar) && fixup_comments)
	    output->character('.');
	output->character('\n');
    }

    if (tag_list_started)
	output->tag_list_end();
}


struct Output autodoc_output =
{
    autodoc_comment,
    autodoc_header,
    autodoc_dash,
    autodoc_section,
    autodoc_sub_section,
    autodoc_break_line,
    autodoc_blank_line,
    autodoc_code_start,
    autodoc_code_end,
    autodoc_code, 
    dummy,                               // autodoc_tag_list_start
    dummy,                               // autodoc_tag_list_end 
    autodoc_tag_entry_start, 
    autodoc_tag_entry_start_extra,
    autodoc_tag_entry_end,
    autodoc_tag_entry_end_extra,
    autodoc_table_start,
    autodoc_table_entry,
    autodoc_table_end,
    autodoc_indent,
    autodoc_list_start,
    autodoc_code,                        // autodoc_list_entry
    autodoc_list_separator,
    autodoc_list_end,
    autodoc_include,
    autodoc_file_end,                    // autodoc_file_end
    autodoc_text,
    autodoc_char,
    NULL,                                // autodoc_parse_option
    dummy,                               // autodoc_print_options 
    autodoc_name,
    autodoc_terse_sep,
    autodoc_text,                        // autodoc_reference 
    autodoc_text,                        // autodoc_emphasized 
    autodoc_description,
    autodoc_returns
};
