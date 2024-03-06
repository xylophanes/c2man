/*-----------------------------------------------*/
/* Example of a header file which can be used to */
/* generate a 'man' page                         */
/*-----------------------------------------------*/
/*------------------------------------*/
/* This enum will be documented using */
/* the (right hand) comments          */
/*------------------------------------*/

enum Place
{
               /*----------------------------*/
    HOME,      /* Home, Sweet Home           */
    WORK,      /* where I spend lots of time */
    MOVIES,    /* Saturday nights mainly     */
    CITY,      /* New York, New York         */
    COUNTRY    /* Bob's Country Bunker       */
               /*----------------------------*/

};


/*-------------------------------------------------*/
/* do some useful work for a change.               */
/* This function will actually get some productive */
/* work done, if you are really lucky.             */
/* returns the number of milliseconds in a second  */
/*-------------------------------------------------*/

int dowork(int,
                               /*--------------------------*/
           enum   Place where, /* where to do the work     */
           long   int);        /* How many seconds to work */
                               /*--------------------------*/


/*------------------------------------*/
/* Structs are not (yet) documented   */
/* but they can be appear as function */
/* arguments                          */
/*------------------------------------*/

typedef struct {   int  arg;
                   char string[256];
               } fred_type;


/*-----------------------------------------------*/
/*  mytype - tests passing structure to function */
/*-----------------------------------------------*/

int mytype(fred_type *fred);
