/* $Id: texinfo.c,v 1.1 2024-01-03 16:35:07 behdad Exp $
 * functions for texinfo style output.
 */
#include "c2man.h"
#include "manpage.h"
#include "output.h"
#include <ctype.h>
#include "semantic.h"

static char *heading_not_in_contents[] = {"@chapheading ", "@heading ", "@subheading ", "@subsubheading "};
static char *heading_in_contents[]     = {"@chapter ", "@section ", "@subsection ", "@subsubsection "};

#define n_levels  (sizeof(heading_not_in_contents) / sizeof(char *))
#define level(n) ((n) >= n_levels ? n_levels - 1 : (n))


/*----------------------------------*/
/* section level for man page entry */
/*----------------------------------*/

static int top_level = 1;


/*--------------------------------------------------*/
/* always output node for manpage, even if embedded */
/*--------------------------------------------------*/

static int embed_node_info = 0;


/*--------------------------------------------------*/
/* use title as name of section, rather than "NAME" */
/*--------------------------------------------------*/

static int title_name = 0;


/*------------------------------------------------*/
/* the section title, filled in by texinfo_header */
/*------------------------------------------------*/

static const char *title = "INTERNAL ERROR, BOGUS TITLE DUDE!";


/*------------------------------------*/
/* do section titles get capitalized? */
/*------------------------------------*/

static int capitalize_sections = 0;


/*--------------------*/
/* Texinfo write char */
/*--------------------*/

void texinfo_char(const int c)
{   unsigned int i;

    switch (c)
    {      case '\t':
	               for (i = 0; i < NUM_TAB_SPACES; i++)
	                   putc(' ',stdout);
	               break;
           default:
    	               putc(c,stdout);
                       break;
    }
}


/*--------------------*/
/* Texinfo write text */
/*--------------------*/

void texinfo_text(const char *text)
{   while (*text)
          texinfo_char(*text++);
}


/*---------------------*/
/* Texinfo put section */
/*---------------------*/

void put_section(const char *text)
{  if (capitalize_sections)
   {   int first_letter = 1;

       for ( ; *text ; text++)
       {   texinfo_char(first_letter ? toupper(*text) : tolower(*text));
           first_letter = isspace(*text);
       }
   }
   else
     texinfo_text(text);
}


/*-----------------*/
/* Texinfo comment */
/*-----------------*/

void texinfo_comment(void)
{   put_string("@c ");
}


/*----------------*/
/* Texinfo header */
/*----------------*/

void texinfo_header(ManualPage *firstpage,
                    int        input_files,
                    boolean    grouped,
                    const char *name,
                    const char *terse,
                    const char *section)
{
    if (! make_embeddable)
    {   put_string("\\input texinfo @c -*-texinfo-*-\n");
	output_warning();
	put_string("@c %**start of header\n");
	put_string("@setfilename ");
	texinfo_text(name);
	put_string(".info\n@settitle ");
	texinfo_text(name);
	putchar('\n');
	put_string("@c %**end of header\n");

	put_string("@node Top, ");
	texinfo_text(name);
	put_string(", (dir), (dir)\n");
    }

    if (! make_embeddable || embed_node_info)
    {   put_string("@node ");
        texinfo_text(name);
        put_string(", (dir), Top, (dir)\n");
    }

    title = name;
}


/*--------------*/
/* Texinfo dash */
/*--------------*/

void texinfo_dash(void)
{   put_string("---");
}


/*-----------------*/
/* Texinfo section */
/*-----------------*/

void texinfo_section(const char *name)
{   put_string(heading_not_in_contents[level(top_level)]);
    put_section(name);
    (void)putc('\n',stdout);
    put_string("@noindent\n");
}


/*-------------------------------*/
/* Texinfo section (in contents) */
/*-------------------------------*/

void texinfo_section_in_contents(const char *name)
{   put_string(heading_in_contents[level(top_level)]);
    put_section(name);
    (void)putc('\n',stdout);
    put_string("@noindent\n");
}


/*---------------------*/
/* Texinfo sub section */
/*---------------------*/

void texinfo_sub_section(const char *name)
{   put_string(heading_not_in_contents[level(top_level+1)]);
    put_section(name);
    (void)putc('\n',stdout);
    put_string("@noindent\n");
}


/*--------------------*/
/* Texinfo break line */
/*--------------------*/

void texinfo_break_line(void)
{   /* put_string("@*\n"); */
}


/*--------------------*/
/* Texinfo blank line */
/*--------------------*/

void texinfo_blank_line(void)
{   put_string("@sp 1\n");
}


/*----------------------------*/
/* Texinfo code start (c2man) */
/*----------------------------*/

void texinfo_code_start(void)
{   put_string("@example\n");
}


/*--------------------------*/
/* Texinfo code end (c2man) */
/*--------------------------*/

void texinfo_code_end(void)
{   put_string("@end example\n");
}


/*----------------------*/
/* Texinfo code (c2man) */
/*----------------------*/

void texinfo_code(const char *text)
{   put_string("@code{");
    texinfo_text(text);
    put_string("}");
}


/*------------------------*/
/* Texinfo tag list start */
/*------------------------*/

void texinfo_tag_list_start(void)
{   put_string("@quotation\n@table @code\n");
}


/*-------------------------*/
/* Texinfo tag entry start */
/*-------------------------*/

void texinfo_tag_entry_start(void)
{   put_string("@item ");
}


/*-----------------------*/
/* Texinfo tag entry end */
/*-----------------------*/

void texinfo_tag_entry_end(void)
{   putchar('\n');
}

void texinfo_tag_entry_end_extra(const char *text)
{   (void)putc('(',stdout);
    texinfo_text(text);
    (void)putc(')',stdout);
    texinfo_tag_entry_end();
}


/*----------------------*/
/* Texinfo tag list end */
/*----------------------*/

void texinfo_tag_list_end(void)
{   put_string("@end table\n@end quotation\n");
}


/*---------------------*/
/* Texinfo table start */
/*---------------------*/
	
void texinfo_table_start(const char *longestag)
{   put_string("@quotation\n@table @code\n");
}


/*---------------------*/
/* Texinfo table entry */
/*---------------------*/

void texinfo_table_entry(const char *name, const char *description)
{   put_string("@item ");
    texinfo_text(name);
    (void)putc('\n',stdout);

    if (description)
	output_comment(description);
    else
	(void)putc('\n',stdout);
}


/*-------------------*/
/* Texinfo table end */
/*-------------------*/

void texinfo_table_end(void)
{   put_string("@end table\n@end quotation\n");
}


/*--------------------*/
/* Texinfo list start */
/*--------------------*/

void texinfo_list_start(void)
{
}


/*--------------------*/
/* Texinfo list entry */
/*--------------------*/

void texinfo_list_entry(const char *text)
{  texinfo_code(text);
}


/*-------------------------*/
/* Texinfo list separators */
/*-------------------------*/

void texinfo_list_separator(void)
{   put_string(",\n");
}


/*------------------*/
/* Texinfo list end */
/*------------------*/

void texinfo_list_end(void)
{   putchar('\n');
}


/*-------------------------*/
/* Texinfo include (c2man) */
/*-------------------------*/

void texinfo_include(const char *filename)
{
    put_string("@include ");
    texinfo_text(filename);
    put_string("\n");
}


/*---------------------*/
/* Texinfo end of file */
/*---------------------*/

void texinfo_file_end(void)
{   put_string("@bye\n");
}

static int first_name = 1;
void texinfo_name(const char *name)
{
    if (name)
    {  if (!first_name || !title_name || strcmp(title,name))
	  texinfo_text(name);
       first_name = 0;
    }
    else
    {   first_name = 1;
	if (title_name)
	{

            /*------------------------------------------------*/
	    /* don't muck around with capitalization of title */
            /*------------------------------------------------*/

	    int capitalize_sections_save = capitalize_sections;
	    capitalize_sections          = 0;

	    texinfo_section_in_contents(title);
	    capitalize_sections          = capitalize_sections_save;
	}
	else
	    texinfo_section("NAME");
    }
}


/*---------------------------------*/
/* Texinfo terse separator (c2man) */
/*---------------------------------*/

void texinfo_terse_sep(void)
{
    if (!title_name || group_together)
    {   texinfo_char(' ');
	texinfo_dash();
	texinfo_char(' ');
    }
}


/*---------------------------*/
/* Texinfo reference (c2man) */
/*---------------------------*/

void texinfo_reference(const char *text)
{   texinfo_text(text);
    texinfo_char('(');
    texinfo_text(manual_section);
    texinfo_char(')');
}


/*-----------------------------------------------------------------*/
/* ideally, this should be made aware of embedded texinfo commands */
/*-----------------------------------------------------------------*/

void texinfo_description(const char *text)
{   enum { TEXT, PERIOD, CAPITALISE } state = CAPITALISE;
    boolean new_line                        = TRUE;
   

    /*--------------------------- -----------------*/
    /* correct punctuation a bit as line is output */
    /*--------------------------- -----------------*/

    for (;*text;text++)
    {   int c = *text;

	if (new_line && (c == '-' || c == '*' || is_numbered(text)))
	{  output->break_line();
	   state = CAPITALISE;
	}
	else if (c == '.')
	   state = PERIOD;
	else if (isspace(c) && state == PERIOD)
	   state = CAPITALISE;
	else if (isalnum(c))
	{  if (islower(c) && state == CAPITALISE)
              c = toupper(c);
	   state = TEXT;
	}
           
	output->character(c);
	new_line = c == '\n';
    }

    /*------------------------------------*/
    /* do a full stop if there wasn't one */
    /*------------------------------------*/

    if (state == TEXT)
       output->character('.');
}


/*-----------------------------------------------------------------*/
/* ideally, this should be made aware of embedded texinfo commands */
/*-----------------------------------------------------------------*/

void texinfo_returns(const char *comment)
{   enum { TEXT, PERIOD, CAPITALISE } state = CAPITALISE;
    char lastchar = '\n';
    boolean tag_list_started = FALSE;


    /*------------------*/
    /* for each line... */
    /*------------------*/

    while (*comment)
    {   boolean    tagged = FALSE;
	const char *c     = comment;

        /*--------------------------------------*/
	/* search along until the end of a word */
        /*--------------------------------------*/

	while (*c && *c != ':' && !isspace(*c))
	      c++;

        /*----------------------------------------------*/
	/* skip all spaces or tabs after the first word */
        /*----------------------------------------------*/

	while (*c && *c != '\n')
	{     if (*c == '\t' || *c == ':')
              {   tagged = TRUE;
		   break;
	      }
              else if (!isspace(*c))
	         break;

              c++;
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
	    {  output->tag_list_start();
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
	    {  if (islower(c) && state == CAPITALISE && fixup_comments)
		  c = toupper(c);
               state = TEXT;
	    }

	    output->character(lastchar = c);
	}


        /*---------------------------------------------------------------*/
	/* if it ended in punctuation, just output the nl straight away. */
        /*---------------------------------------------------------------*/

	if (ispunct(lastchar))
	{  if (lastchar == '.')
              state = CAPITALISE;
	   output->character(lastchar = '\n');
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


/*--------------*/
/* Texinf parse */
/*--------------*/

int texinfo_parse_option(const char *option)
{
    if (option[0] == 't')
	title_name = 1;
    else if (option[0] == 'n')
	embed_node_info = 1;
    else if (option[0] == 's')
    {  top_level = atoi(&option[1]);
       if (top_level < 0)
          return 1;
    }
    else if (option[0] == 'C')
	capitalize_sections = 1;
    else return 1;

    return 0;
}


/*-----------------------*/
/* Texinfo print options */
/*-----------------------*/

void texinfo_print_options(void)
{
    (void)fputs("\ttexinfo options:\n", stderr);
    (void)fputs("\tt\tuse manpage title as NAME title\n", stderr);
    (void)fputs("\tn\toutput node info if embedded output\n", stderr);
    (void)fputs("\ts<n>\tset top heading level to <n>\n", stderr);
    (void)fputs("\tC\tcaptialize section titles\n", stderr);
}


/*--------------------------*/
/* Texinfo output structure */
/*--------------------------*/

struct Output texinfo_output =
{
        texinfo_comment,
        texinfo_header,
        texinfo_dash,
        texinfo_section,
        texinfo_sub_section,
        texinfo_break_line,
        texinfo_blank_line,
        texinfo_code_start,
        texinfo_code_end,
        texinfo_code,
        texinfo_tag_list_start,
        texinfo_tag_list_end,
        texinfo_tag_entry_start,


                                       /*-------------------*/
        texinfo_tag_entry_start,       /* entry_start_extra */
                                       /*-------------------*/

        texinfo_tag_entry_end,
        texinfo_tag_entry_end_extra,
        texinfo_table_start,
        texinfo_table_entry,
        texinfo_table_end,

                                        /*------------------*/
        dummy,                          /* texinfo_indent   */
                                        /*------------------*/

        texinfo_list_start,
        texinfo_list_entry,
        texinfo_list_separator,
        texinfo_list_end,
        texinfo_include,
        texinfo_file_end,
        texinfo_text,
        texinfo_char,
        texinfo_parse_option,
        texinfo_print_options,
        texinfo_name,
        texinfo_terse_sep,
        texinfo_reference,
        texinfo_text,
        texinfo_description,
        texinfo_returns
};
