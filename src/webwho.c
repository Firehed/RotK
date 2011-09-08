/*
 *  This Code originally created by Matthew Peck, aka Valatar
 *              sauron@digital-genesis.com
 *  Modified by Nebseni of Clandestine MUD
 *              http://clandestine.mudnet.net
 *
 *  Created so that I wouldn't have to make a CGI script that would update
 *  the page everytime you check.  All you have to do to use this is email 
 *  me and let me know that you are.  Include your mud address and webpage 
 *  if you don't mind.  You have to know a bit of HTML to 
 *  make the changes necessary for this to work for your mud, but other 
 *  than that, its all self contained.  This code is compatible with Lope's
 *  Colour codes, if you use some other system, you'll need to change 
 *  get_who_data to fit.  If there are any problems, email me and let me know.
 *
 *  To get this to work, save this as a .c file.  Then add a line in update.c
 *  that declares the function, and another that calls it.  I added it to 
 *  the pulse_music part, so it updates about every 5 seconds.  You can add
 *  it elsewhere if you like.  The declaration looks like this:
 *                  void    who_html_update args( ( void ) );
 *  Then the function call looks like this:
 *                  who_html_update();
 *  Update the makefile to include this code, then compile and you'll be all set.
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"
 
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif

int html_colour args (( char type, char *string ));
void html_colourconv args (( char *buffer, const char *txt, CHAR_DATA *ch ));

void who_html_update (void)
{

	/* this code assumes 45-character titles and max color switches (485 bytes).
	  if title length is increased the buf sizes must be increased */
  FILE *fp; 
  DESCRIPTOR_DATA *d;
  char buf[2*MAX_INPUT_LENGTH]; 
  char buf2[2*MAX_INPUT_LENGTH];
  char buf3[2*MAX_INPUT_LENGTH]; 
  char buf4[2*MAX_INPUT_LENGTH];
  char buf5[2*MAX_INPUT_LENGTH]; 
  char buf6[2*MAX_INPUT_LENGTH];
  char levelnum[2*MAX_INPUT_LENGTH];

#ifdef DEBUG
        Debug ("who_html_update");
#endif

  buf[0] = '\0';
  buf2[0] = '\0';
  
  fclose(fpReserve);
  if ( (fp = fopen("../../public_html/online.html", "w") ) == NULL)
     /* change the directory above to the absolute directory and filename
      * of the page you are going to make.  IMPORTANT:  The file needs to 
      * exist before you attempt to run this. 
      *         --Valatar
      */

  {
     bug( "online.html: fopen", 0 );
     perror( "online.html" );
  }
  else
  {
  fprintf(fp, "<html>\n");
  fprintf(fp, "<head>\n");
  fprintf(fp, "<META HTTP-EQUIV=refresh\n");
  fprintf(fp, "content=6; URL=http://rotk.tamarisk.ab.ca/online.html>\n");
  fprintf(fp, "<title>\n");
  fprintf(fp, "Players currently on RotK\n");
/* INSERT YOUR MUD NAME THERE AND THREE LINES DOWN */

  fprintf(fp, "</title>\n");
  fprintf(fp, "<BODY TEXT=""#C0C0C0"" BGCOLOR=""#000000"" LINK=""#00FFFF""\n");
  fprintf(fp, "VLINK=""#FFFFFF"" ALINK=""#008080"">\n");
  fprintf(fp, "<h1><center>Who's on Realm of the Keepers: The Rebirth?</center></h1>\n");

  fprintf(fp, "<CENTER><TABLE BORDER=1 BGCOLOR=""#000000"" >\n");
  fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
  fprintf(fp, "<TD><FONT COLOR=#FFFF00>LEVEL</FONT></TD>\n");
  fprintf(fp, "<TD><FONT COLOR=#OOFFFF>RACE</FONT></TD>\n");
  fprintf(fp, "<TD><FONT COLOR=#FF0000>CLASS</FONT></TD>\n");
  fprintf(fp, "<TD><FONT COLOR=#0000FF>CLAN</FONT></TD>\n");
  fprintf(fp, "<TD><FONT COLOR=#006400>NAME</FONT></TD></TR>\n");
    
  for ( d = descriptor_list; d != NULL ; d = d->next )
  {
    CHAR_DATA *wch;
    char class[5];
    
    if ( d->connected != CON_PLAYING)
        continue;
    wch   = ( d->original != NULL ) ? d->original : d->character;
    class[0] = '\0';

	sprintf(levelnum, " %3d", wch->level);
	switch ( wch->level )
	{
	default: break;
            {
		case MAX_LEVEL - 0 :
		    sprintf(levelnum, "IMP");
		    break;
		case MAX_LEVEL - 1 :
		    sprintf(levelnum, "HBLD");
		    break;
		case MAX_LEVEL - 2 :
		    sprintf(levelnum, "HQST");
		    break;
		case MAX_LEVEL - 3 :
		    sprintf(levelnum, "PKPR");
		    break;
		case MAX_LEVEL - 4 :
		    sprintf(levelnum, "CLAN");
		    break;
		case MAX_LEVEL - 5 :
		    sprintf(levelnum, "BLDR");
		    break;
		case MAX_LEVEL - 6 :
		    sprintf(levelnum, "DIVI");
		    break;
		case MAX_LEVEL - 7 :
		    sprintf(levelnum, "-Q-");
		    break;
		case MAX_LEVEL - 8 :
		    sprintf(levelnum, "NBLD");
		    break;
		case MAX_LEVEL - 9 :
		    sprintf(levelnum, "HERO");
		    break;

	    }
	}
    if (wch->invis_level <= LEVEL_HERO)
    {
    if (wch->incog_level <= LEVEL_HERO)
    {
    if (!IS_SHIELDED(wch, SHD_CAMO))
    {

	fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
	if (wch->level >= LEVEL_HERO)
	    fprintf(fp, "<TD><FONT COLOR=#FFFF00>%s</FONT></TD>\n", levelnum);
	else
	fprintf(fp, "<TD><FONT COLOR=#FFFF00>%d</FONT></TD>\n", wch->level);

	fprintf(fp, "<TD><FONT COLOR=#OOFFFF>%s</FONT></TD>\n", race_table[wch->race].name);
	fprintf(fp, "<TD><FONT COLOR=#FF0000>%s</FONT></TD>\n", class_table[wch->class].name);
      buf4[0] = '\0';
      sprintf(buf4, "<TD><FONT COLOR=#0000FF>%s</FONT></TD>\n", (IS_NPC(wch)? "" : clan_table[wch->clan].who_name));
	html_colourconv( buf3, buf4, wch );
      fprintf(fp, buf3);



      buf6[0] = '\0';
      sprintf(buf6, "<TD><FONT COLOR=#006400>%s</FONT>", wch->pcdata->pretitle);
	html_colourconv( buf5, buf6, wch );
      fprintf(fp, buf5);
	fprintf(fp, "<FONT COLOR=#006400>%s</FONT>", wch->name);

      buf2[0] = '\0';
      sprintf(buf2, "<FONT COLOR=#006400>%s</FONT>", (IS_NPC(wch)? "" : wch->pcdata->title));
	html_colourconv( buf, buf2, wch );
      fprintf(fp, buf);
      fprintf(fp, "</TD></TR>\n");
 
    }
    }
    }  /*end if */
  }    /*end for */
  
  fprintf(fp, "</TABLE></CENTER>\n");
  sprintf(buf, "This file last updated at %s Mountain Time.\n", ((char *) ctime( &current_time )));
  fprintf(fp, buf);
  fprintf(fp, "</center></font>\n");

  fprintf(fp, "<br><br>\n");
  fprintf(fp, "<CENTER><P><A HREF=""./"" TARGET=""_top""><Font Size=+1>\n");
  fprintf(fp, "Return to main page</A> </P></CENTER></Font>\n");
  fprintf(fp, "</body>\n");
  fprintf(fp, "</html>\n");
  fclose( fp ); 
  fpReserve = fopen( NULL_FILE, "r" );
  } /*end if */ 
  
  return;
}/* end function */


int html_colour( char type, char *string )
{
    char	code[ 25 ];
    char	*p = '\0';

#ifdef DEBUG
        Debug ("html_colour");
#endif
        
    switch( type )
    {
	default:
	case '\0':
	    code[0] = '\0';
	    break;
	case ' ':
	    sprintf( code, " " );
	    break;
	case 'x':
	case '0':
	    sprintf( code, "<font color=""#006400"">" );
	    break;
	case 'b':
	case '4':
	    sprintf( code, "<font color=""#00008B"">" );
	    break;
	case 'c':
	case '6':
	    sprintf( code, "<font color=""#008B8B"">" );
	    break;
	case 'g':
	case '2':
	    sprintf( code, "<font color=""#006400"">" );
	    break;
	case 'm':
	case '5':
	    sprintf( code, "<font color=""#8B008B"">" );
	    break;
	case 'r':
	case '1':
	    sprintf( code, "<font color=""#8B0000"">" );
	    break;
	case 'w':
	case '7':
	    sprintf( code, "<font color=""#808080"">" );
	    break;
	case 'y':
	case '3':
	    sprintf( code, "<font color=""#808000"">" );
	    break;
	case 'B':
	case '$':
	    sprintf( code, "<font color=""#0000FF"">" );
	    break;
	case 'C':
	case '^':
	    sprintf( code, "<font color=""#OOFFFF"">" );
	    break;
	case 'G':
	case '@':
	    sprintf( code, "<font color=""#00FF00"">" );
	    break;
	case 'M':
	case '%':
	    sprintf( code, "<font color=""#FF00FF"">" );
	    break;
	case 'R':
	case '!':
	    sprintf( code, "<font color=""#FF0000"">" );
	    break;
	case 'W':
	case '&':
	    sprintf( code, "<font color=""#FFFFFF"">" );
	    break;
	case 'Y':
	case '#':
	    sprintf( code, "<font color=""#FFFF00"">" );
	    break;
	case 'D':
	case '8':
	case '*':
	    sprintf( code, "<font color=""#636363"">" );
	    break;
	case '{':
	    sprintf( code, "{" );
	    break;
    }

    p = code;
    while( *p != '\0' )
    {
	*string = *p++;
	*++string = '\0';
    }

    return( strlen( code ) );
}

void html_colourconv( char *buffer, const char *txt, CHAR_DATA *ch )
{
    const	char	*point;
		int	skip = 0;

#ifdef DEBUG
        Debug ("html_colourconv");
#endif
        
    for( point = txt ; *point ; point++ )
    {
	if( *point == '{' )
	{
	    point++;
	    if( *point == '\0' )
		point--;
	    else
	      skip = html_colour( *point, buffer );
	    while( skip-- > 0 )
		++buffer;
	    continue;
	}
	/* Following is put in to prevent adding HTML links to titles,
	   except for IMMS who know what they're doing and can be
	   punished if they screw it up! */
/*
	if( (*point == '<') && (!IS_IMMORTAL(ch)) )
	{
	    *buffer = '[';
	    *++buffer = '\0';
	    continue;
	}
	if( (*point == '>') && (!IS_IMMORTAL(ch)) )
	{
	    *buffer = ']';
	    *++buffer = '\0';
	    continue;
	}
*/ //taken out for hero levels
	*buffer = *point;
	*++buffer = '\0';
    }			
    *buffer = '\0';
    return;
}



