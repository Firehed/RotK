#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif


#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "lookup.h"

//Joe:Lockers	Check if theres a locker in the room and if so return the obj data
OBJ_DATA *get_obj_locker_room( CHAR_DATA *ch, sh_int vnum )
{
    OBJ_DATA *obj;
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	if ( obj->pIndexData->vnum == vnum )
	    return obj;
    return NULL;
}

/*
 * Find an obj in player's inventory without checking for invis blind etc.... Used for lockers
 */
OBJ_DATA *get_locker( CHAR_DATA *ch)
{
//    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
		if (obj->pIndexData->vnum==LOCKER_VNUM)
			return obj;
    }

    return NULL;
}


void do_locker (CHAR_DATA *ch,char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	BUFFER *buf;
    BUFFER *list;

	OBJ_DATA *obj;
	OBJ_DATA *container;

	argument=one_argument(argument,arg1);
	argument=one_argument(argument,arg2);


    list=new_buf();

	if (ch->in_room->vnum!=LOCKER_ROOM)
	{
		send_to_char("Go the locker room!!\n\r",ch);
		return;
	}

	if ( ( container=get_locker(ch))==NULL)
	{
		send_to_char("Locker, what locker?\n\r",ch);
		return;
	}
	if (!str_cmp(arg1,"get"))
	{
		//do the locker get command


	  /* 'get obj container' */
		obj = get_obj_list( ch, arg2, container->contains );
	   if ( obj == NULL )
	   {
			act( "Your locker doesn't contain that.",ch, NULL, arg2, TO_CHAR );
			return;
		}
	   get_obj( ch, obj, container );
	}
	else if(!str_cmp(arg1,"put"))
	{
		//you guessed it, do the the locker put command
		
		//First get the object from inventory
		if ( ( obj = get_obj_carry( ch, arg2 ) ) == NULL )
		{
			send_to_char( "You do not have that item.\n\r", ch );
			return;
		}

		//Now check to see if meets conditions
		if ( obj->item_type==ITEM_CONTAINER)
		{
			send_to_char( "Containers are not allowed in lockers.\n\r", ch );
			return;
		}


		if ( !can_drop_obj( ch, obj ) )
		{
			send_to_char( "You can't let go of it.\n\r", ch );
			return;
		}

		//Put obj in locker
		obj_from_char( obj );
		obj_to_obj( obj, container );
		act( "$n puts $p in $s locker.", ch, obj, container, TO_ROOM );
	   act( "You put $p in your locker.", ch, obj, container, TO_CHAR );

	}
	else if(!str_cmp(arg1,"list"))
	{
		//do the locker list command
		send_to_char("Your locker contains:\n\r",ch);
		buf = show_list_to_char(container->contains ,ch,TRUE,TRUE);
        add_buf(list,buf_string(buf));
        free_buf(buf);
        page_to_char(buf_string(list), ch);
		return;
	}
	else
	{
		send_to_char("Try this syntax instead: \nlocker put *object*\nlocker get *object*\nlocker list\n",ch);

	}
	return;	
}


