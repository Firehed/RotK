/* -1 MARRY MARRIAGE SPOUSE SPOUSETALK DIVORCE~
Ah, the beautiful ceremony that occurs when someone loves another so much
that they wish to spend the rest of their lives together.  Unfortuneately,
it turns sour sometimes.  Whether you're wanting to marry, or be divorced,
it must be performed by an Immortal.  With marriage, comes the ability
to communicate exclusively to your spouse via your own private talkline.

Marriage code written by Ryouga (ryouga@jessi.indstate.edu).
~
*/
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Gabrielle Taylor						   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/
/***************************************************************************
*	MARRY.C written by Ryouga for Vilaross Mud (baby.indstate.edu 4000)*
*	Please leave this and all other credit include in this package.    *
*	Email questions/comments to ryouga@jessi.indstate.edu		   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void do_marry( CHAR_DATA *ch, char *argument)
{

    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *husband;
    CHAR_DATA *wife;
    char buf[MAX_STRING_LENGTH];
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

   if ((ch->level >= LEVEL_IMMORTAL))
   { 
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: marry <char1> <char2>\n\r",ch);
        return;
    }
    if ( ( husband = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "The first person mentioned isn't playing.\n\r", ch );
        return;
    }
    
    if ( ( wife = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "The second person mentioned isn't playing.\n\r", ch);
	return;
    }
    
    if ( IS_NPC(husband) || IS_NPC(wife))
    {
     send_to_char("I don't think they want to be Married to the mob.\n\r", ch);
     return;
    }        
    
    if (!IS_SET(husband->act, PLR_CONSENT) || !IS_SET(wife->act, PLR_CONSENT))
    {
     send_to_char( "They do not give consent.\n\r", ch);
     return;
    }
    
    if (husband->spouse != NULL || wife->spouse != NULL)
    {
       send_to_char( "They are already married! \n\r", ch);
       return;
    }
   

    send_to_char( "You pronounce them man and wife!\n\r", ch);
    send_to_char( "You say the big 'I do.'\n\r", husband);
    send_to_char( "You say the big 'I do.'\n\r", wife);
        sprintf(buf, "%s", wife->name);
    husband->spouse = buf;
        sprintf(buf, "%s", husband->name);
    wife->spouse = buf;
    return;

   } 
   else
   {
    send_to_char( "You do not have marrying power.\n\r", ch);
    return;
   }
}

void do_divorce( CHAR_DATA *ch, char *argument)
{

    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *husband;
    CHAR_DATA *wife;
    char buf[MAX_STRING_LENGTH];
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

   if (ch->level >= LEVEL_IMMORTAL)
   { 
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: divorce <char1> <char2>\n\r",ch);
        return;
    }
    if ( ( husband = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "The first person mentioned isn't playing.\n\r", ch );
        return;
    }
    
    if ( ( wife = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "The second person mentioned isn't playing.\n\r", ch);
	return;
    }
    
    if ( IS_NPC(husband) || IS_NPC(wife))
    {
     send_to_char("I don't think they're Married to the Mob...\n\r", ch);
     return;
    }
            
    if (!IS_SET(husband->act, PLR_CONSENT) || !IS_SET(wife->act, PLR_CONSENT))
    {
     send_to_char( "They do not give consent.\n\r", ch);
     return;
    }
    
        sprintf(buf, "%s", wife->name);
    if (husband->spouse != buf)
    {
     send_to_char( "They aren't even married!!\n\r",ch);
     return;
    }
       
    send_to_char( "You hand them their papers.\n\r", ch);
    send_to_char( "Your divorce is final.\n\r", husband);
    send_to_char( "Your divorce is final.\n\r", wife);
    husband->spouse = NULL;
    wife->spouse = NULL;
    return;

   } 
   else
   {
    send_to_char( "You do not have divorcing power.\n\r", ch);
    return;
   }
}

void do_consent( CHAR_DATA *ch, char *argument )
{
   if (IS_NPC(ch))
    return;
    
   if ( IS_SET(ch->act, PLR_CONSENT) )
   {
    send_to_char( "You no longer give consent.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_CONSENT);
    return;
   }
                           
   send_to_char( "You now give consent to Married!\n\r", ch);
   SET_BIT(ch->act, PLR_CONSENT);
   return;
}


void do_spousetalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
     send_to_char("What do you wish to tell your other half?\n\r", ch);
     return;
    }
    else  /* message sent */
    {
       
      sprintf( buf, "You say to %s, '%s'\n\r", ch->spouse, argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             d->character->name == ch->spouse)
        {
          act_new( "$n says to you, '$t'", 
		   ch, argument, d->character, TO_VICT, POS_SLEEPING );
        }
        else
        {
         send_to_char("Your spouse is not here.\n\r", ch);
         return;
        }
      }
    }
}
