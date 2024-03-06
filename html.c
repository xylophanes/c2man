/* $Id: html.c,v 1.1 2004-05-03 05:17:49 behdad Exp $
 * functions for html style output.
 */
#include "c2man.h"
#include "manpage.h"
#include "output.h"
#include <ctype.h>
#include "semantic.h"

static int html_in_code = 0;

void html_terse_sep();
void html_description _((const char *text));


/*----------------------------------------------------------*/
/* Convert special HTML character when generating HTML text */
/*----------------------------------------------------------*/

void html_char(const int c)
{
    switch (c)
    {
           case '<':
                        put_string("&lt;");
                        break;
           case '>':
                        put_string("&gt;");
                        break;
           case '&':
                        put_string("&amp;");
                        break;
           case '"':
                       put_string("&quot;");
                       break;
           default:
                       putchar(c);
                       break;
    }
}


/*----------------------*/
/* Convert text to HTML */
/*----------------------*/

void html_text(const char *text)
{    while (*text)
           html_char(*text++);
}


/*--------------*/
/* HTML comment */
/*--------------*/

void html_comment()
{    put_string("<!");
}



/*----------------------*/
/* Header for HTML page */
/*----------------------*/

void html_header(ManualPage *firstpage,
                 int        input_files,
                 boolean    grouped,
                 const char *name,
                 const char *terse,
                 const char *section)

{    output_warning();
     put_string("<header>\n");
     put_string("<title>");
     html_text(name);
     html_terse_sep();
     html_text(terse);
     put_string("</title>\n");
     put_string("</header>\n");
     put_string("<body>\n");
}


/*------------------*/
/* End of HTML text */
/*------------------*/

void html_file_end()
{   put_string("\n</body>\n");
}


/*------------------*/
/* HTML dash symbol */
/*------------------*/

void html_dash()
{
  put_string("-");
}


/*--------------*/
/* HTML section */
/*--------------*/

void html_section(const char *name)
{    put_string("<h1>");
     html_text(name);
     put_string("</h1>\n");
}


/*------------------*/
/* HTML sub section */
/*------------------*/

void html_sub_section( const char *name)
{    put_string("<h2>");
     html_text(name);
     put_string("</h2>");
}


/*-----------------*/
/* HTML break line */
/*-----------------*/

void html_break_line(void)
{   if (!html_in_code)
       put_string("<br>\n");
}


/*-----------------*/
/* HTML blank line */
/*-----------------*/

void html_blank_line(void)
{   if (!html_in_code)
       put_string("<p>\n");
    else
       (void)putc('\n',stdout);
}


/*----------------------------*/
/* Start of HTML code section */
/*----------------------------*/

void html_code_start(void)
{    put_string("<pre>");
     html_in_code = 1;
}


/*--------------------------*/
/* End of HTML code section */
/*--------------------------*/

void html_code_end(void)
{    put_string("</pre>\n");
     html_in_code = 0;
}


/*-------------------*/
/* HTML code section */
/*-------------------*/

void html_code(const char *text)
{   html_code_start();
    html_text(text);
    html_code_end();
}


/*---------------------*/
/* HTML tag list start */
/*---------------------*/

void html_tag_list_start(void)
{   put_string("<dl>");
}


/*-------------------*/
/* HTML tag list end */
/*-------------------*/

void html_tag_list_end()
{    put_string("</dl>\n");
}


/*----------------------*/
/* HTML tag entry start */
/*----------------------*/

void html_tag_entry_start(void)
{   put_string("<dt>\n");
}   
    
void html_tag_entry_start_extra(void)
{   put_string("<dt>\n");
}   


/*--------------------*/
/* HTML tag entry end */
/*--------------------*/
 
void html_tag_entry_end(void)
{   put_string("<dd>\n");
}

void html_tag_entry_end_extra(const char *text)
{   put_string(" <em>");
    put_string(text);
    put_string("</em>)");
    put_string("<dd>\n");
}


/*------------------*/
/* HTML table start */
/*------------------*/

void html_table_start(const char *longestag)
{   put_string("<ul>");
}


/*------------------*/
/* HTML table entry */
/*------------------*/

void html_table_entry(const char  *name,
                      const char  *description)
{   put_string("<li>");
    html_text(name);

    if (description)
    {   html_terse_sep();
        html_description(description);
    }
    put_string("<p>\n");
}


/*----------------*/
/* HTML table end */
/*----------------*/

void html_table_end(void)
{    put_string("</ul>");
}


/*-------------------*/
/* HTML indent (tab) */
/*-------------------*/

void html_indent(void)
{    put_string("\t");
}


/*-----------------*/
/* HTML list start */
/*-----------------*/

void html_list_start(void)
{    put_string("<ul>");
}


/*---------------*/
/* HTML list end */
/*---------------*/

void html_list_end(void)
{    put_string("</ul>");
}


/*-----------------*/
/* HTML list entry */
/*-----------------*/

void html_list_entry(const char *name)
{    put_string("<li>");
     put_string(name);
     put_string("\n");
}

/*---------------------*/
/* HTML list separator */
/*---------------------*/

void html_list_separator(void)
{    put_string(",\n");
}


/*-------------------------------*/
/* HTML include (c2man specific) */
/*-------------------------------*/

void html_include(const char *filename)
{   (void)fprintf(stdout,".so %s\n", filename);
}

/*----------------------------*/
/* HTML name (c2man specific) */
/*----------------------------*/

void html_name( const char *name)
{   if (name)
       html_text(name);
    else
       html_section("NAME");
}


/*---------------------------------------*/
/* HTML terse separator (c2man specific) */
/*---------------------------------------*/

void html_terse_sep(void)
{   html_char(' ');
    html_dash();
    html_char(' ');
}


/*-------------------------*/
/* HTML href (link to URL) */
/*-------------------------*/

void html_reference(const char *name)
{   put_string("<a href=");
    put_string(name);
    put_string(".html>");
    put_string(name);
    put_string("</a>\n");
}  


/*-----------*/
/* HTML bold */
/*-----------*/

void html_emphasized(const char *text)
{   put_string("<em>");
    put_string(text);
    put_string("</em>");
}


/*--------------------------------------------------------------*/
/* ideally, this should be made aware of embedded html commands */
/*--------------------------------------------------------------*/

void html_description(const char *text)
{
    enum { TEXT, PERIOD, CAPITALISE } state = CAPITALISE;
    boolean new_line = TRUE;
    

    /*-------------------------------------------*/
    /* correct punctuation a bit as it is output */
    /*-------------------------------------------*/

    for (;*text;text++)
    {    int c = *text;

	if (new_line && (c == '-' || c == '*' || c == '\n' || is_numbered(text)))
	{   output->break_line();
	   state = CAPITALISE;
	}


        /*--------------------------------*/
        /* Two newlines - Paragraph break */
        /*--------------------------------*/

	else if (new_line && c == '\n')
	    output->blank_line();
	else if (c == '.')
	    state = PERIOD;
	else if (isspace(c) && state == PERIOD)
	    state = CAPITALISE;
	else if (isalnum(c))
	{   if (islower(c) && state == CAPITALISE)
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

/*--------------------------------------------------------------*/
/* ideally, this should be made aware of embedded html commands */
/*--------------------------------------------------------------*/

void html_returns(const char *comment)
{
    enum { TEXT, PERIOD, CAPITALISE } state = CAPITALISE;
    char lastchar = '\n';
    boolean tag_list_started = FALSE;

    /*------------------*/
    /* for each line... */
    /*------------------*/

    while (*comment)
    {     boolean    tagged = FALSE;
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
	         {  tagged = TRUE;
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
	      {  if (state == TEXT && !ispunct(lastchar))
                    output->character('.');
		 output->character(lastchar = '\n');
	      }

	      if (!tag_list_started)
	      {  output->tag_list_start();
		 tag_list_started = TRUE;
	      }

              /*----------------------*/
	      /* output the taggy bit */
              /*----------------------*/

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
	   {    char c = *comment;

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

	   if (*comment)
              comment++;
    }


    /*---------------------------------------*/
    /* output lingering newline if necessary */
    /*---------------------------------------*/

    if (lastchar != '\n')
    {   if (state == TEXT && !ispunct(lastchar) && fixup_comments)
           output->character('.');
	output->character('\n');
    }

    if (tag_list_started)
       output->tag_list_end();
}


/*-----------------------*/
/* HTML output structure */
/*-----------------------*/

struct Output html_output =
{
      html_comment,
      html_header,
      html_dash,
      html_section,
      html_sub_section,
      html_break_line,
      html_blank_line,
      html_code_start,
      html_code_end,
      html_code,
      html_tag_list_start,
      html_tag_list_end,
      html_tag_entry_start,
      html_tag_entry_start_extra,
      html_tag_entry_end,
      html_tag_entry_end_extra,
      html_table_start,
      html_table_entry,
      html_table_end,
      html_indent,
      html_list_start,
      html_list_entry,
      html_list_separator,
      html_list_end,
      html_include,
      html_file_end,
      html_text,
      html_char,

                                /*--------------------*/
      NULL,                     /* html_parse_option  */
      dummy,                    /* html_print_options */
                                /*--------------------*/

      html_name,
      html_terse_sep,
      html_reference,      
      html_emphasized,
      html_description,
      html_returns
  };
