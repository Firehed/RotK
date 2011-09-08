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
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/*************************************************************************** 
*       ROT 1.4 is copyright 1996-1997 by Russ Walsh                       * 
*       By using this code, you have agreed to follow the terms of the     * 
*       ROT license, in the file doc/rot.license                           * 
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
#include "tables.h"
#include "interp.h"

/* command procedures needed heheheheh Wardrof Feb 4th
DECLARE_DO_FUN(do_awareness	);
DECLARE_DO_FUN(do_backstab	);
DECLARE_DO_FUN(do_emote		);
DECLARE_DO_FUN(do_berserk	);
DECLARE_DO_FUN(do_bash		);
DECLARE_DO_FUN(do_trip		);
DECLARE_DO_FUN(do_dirt		);
DECLARE_DO_FUN(do_flee		);
DECLARE_DO_FUN(do_kick		);
DECLARE_DO_FUN(do_disarm	);
DECLARE_DO_FUN(do_get		);
DECLARE_DO_FUN(do_recall	);
DECLARE_DO_FUN(do_yell		);
DECLARE_DO_FUN(do_sacrifice	);
DECLARE_DO_FUN(do_circle	);
DECLARE_DO_FUN(do_feed		);
DECLARE_DO_FUN(do_gouge		);
DECLARE_DO_FUN(do_vdpi		);
DECLARE_DO_FUN(do_vdtr		);
DECLARE_DO_FUN(do_vdth		);
DECLARE_DO_FUN(do_look		);
DECLARE_DO_FUN(do_impale	);
DECLARE_DO_FUN(do_deathblow	);
DECLARE_DO_FUN(do_lore		);
DECLARE_DO_FUN(do_killer	);
DECLARE_DO_FUN(do_bind      );
DECLARE_DO_FUN(do_range     ); muah haha!*/
/*
 * Local functions.
 */
void	check_assist	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_counter   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt ) );
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_shield_block     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    dam_message 	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                            int dt, bool immune ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim, 
			    int total_levels ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe_mock	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_voodood	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	make_corpse	args( ( CHAR_DATA *ch, CHAR_DATA *killer ) );
void	one_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary ) );
void	one_hit_mock args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary ) );
void    mob_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	raw_kill	args( ( CHAR_DATA *victim, CHAR_DATA *killer ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

//bool nopk=FALSE;

bool check_counter( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt)
{
        int chance;
        int dam_type;
        OBJ_DATA *wield;

        wield = get_eq_char(victim,WEAR_WIELD);

        if ( IS_NPC(victim))
        return FALSE;

        chance = get_skill(ch,gsn_counter) / 3;
        if ( !IS_NPC(ch))
        chance += ( victim->level - ch->level ) / 2;
        chance += 5 * (get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX));
        chance += get_weapon_skill(victim,get_weapon_sn(ch)) -
                        get_weapon_skill(ch,get_weapon_sn(victim));
        chance += (get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR) );

    if ( number_percent( ) >= chance )
        return FALSE;

        dt = gsn_counter;

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else 
            dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
    	if (wield != NULL)
    	    dam_type = attack_table[wield->value[3]].damage;
    	else
    	    dam_type = attack_table[ch->dam_type].damage;
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    act( "You reverse $n's attack and counter with your own!", ch, NULL, victim, TO_VICT    );
    act( "$N reverses your attack!", ch, NULL, victim, TO_CHAR    );

    damage(victim,ch,dam/2, gsn_counter , dam_type ,TRUE ); /* DAM MSG NUMBER!! */

    check_improve(victim,gsn_counter,TRUE,6);

    return TRUE;
}




/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	ch_next	= ch->next;

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

	/*
	 * Fun for the whole family!
	 */
	check_assist(ch,victim);
    
	if ( IS_NPC( ch ) )
	{
	    if ( HAS_TRIGGER( ch, TRIG_FIGHT ) )
		mp_percent_trigger( ch, victim, NULL, NULL, TRIG_FIGHT );
	    if ( HAS_TRIGGER( ch, TRIG_HPCNT ) )
		mp_hprct_trigger( ch, victim );
	}
    }

    return;
}

void do_awareness(CHAR_DATA *ch, char *argument) 
{ 
AFFECT_DATA af; 

if (IS_NPC(ch) || ch->level < skill_table[gsn_awareness].skill_level[ch->class] ) 
return; 


if ( is_affected(ch, gsn_awareness)) 
     { 
send_to_char("You are already aware.\n\r",ch); 
return; 
     } 

WAIT_STATE( ch, skill_table[gsn_awareness].beats ); 

if (number_percent() > ch->pcdata->learned[gsn_awareness]) 
     { 
send_to_char("You are aware of everything, you think.\n\r", ch); 
act("$n peers intently around themselves.", ch, NULL, NULL, TO_ROOM); 
return; 
     } 

af.where = TO_AFFECTS; 
af.type = gsn_awareness; 
af.level = ch->level; 
af.duration = ch->level; 
af.location = 0; 
af.modifier = ch->level / 8; 
af.bitvector = AFF_DETECT_INVIS; 
affect_to_char( ch, &af ); 

af.where = TO_AFFECTS; 
af.type = gsn_awareness; 
af.level = ch->level; 
af.duration = ch->level; 
af.location = 0; 
af.modifier = ch->level / 8; 
af.bitvector = AFF_DETECT_HIDDEN; 
affect_to_char( ch, &af ); 

af.where = TO_AFFECTS; 
af.type = gsn_awareness; 
af.level = ch->level; 
af.duration = ch->level; 
af.location = 0; 
af.modifier = ch->level / 8; 
af.bitvector = AFF_FARSIGHT; 
affect_to_char( ch, &af ); 
send_to_char("Your eyes glow as you become aware to everything.\n\r",ch); 
return; 
} 



/* for auto assisting */
void check_assist(CHAR_DATA *ch,CHAR_DATA *victim)
{
    CHAR_DATA *rch, *rch_next;

    for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
    {
	rch_next = rch->next_in_room;
	
	if (IS_AWAKE(rch) && rch->fighting == NULL)
	{

	    /* quick check for ASSIST_PLAYER */
	    if (!IS_NPC(ch) && IS_NPC(rch) 
	    && IS_SET(rch->off_flags,ASSIST_PLAYERS)
	    &&  rch->level + 6 > victim->level)
	    {
		do_emote(rch,"{Rscreams and attacks!{x");
		multi_hit(rch,victim,TYPE_UNDEFINED);
		continue;
	    }

	    /* PCs next */
	    if (!IS_NPC(ch) || IS_AFFECTED(ch,AFF_CHARM))
	    {
		if ( ( (!IS_NPC(rch) && IS_SET(rch->act,PLR_AUTOASSIST))
		||     IS_AFFECTED(rch,AFF_CHARM)) 
		&&   is_same_group(ch,rch) 
		&&   !is_safe(rch, victim))
		    multi_hit (rch,victim,TYPE_UNDEFINED);
		
		continue;
	    }
  	
	    /* now check the NPC cases */
	    
 	    if (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM))
	
	    {
		if ( (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALL))

		||   (IS_NPC(rch) && rch->group && rch->group == ch->group)

		||   (IS_NPC(rch) && rch->race == ch->race 
		   && IS_SET(rch->off_flags,ASSIST_RACE))

		||   (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALIGN)
		   &&   ((IS_GOOD(rch)    && IS_GOOD(ch))
		     ||  (IS_EVIL(rch)    && IS_EVIL(ch))
		     ||  (IS_NEUTRAL(rch) && IS_NEUTRAL(ch)))) 

		||   (rch->pIndexData == ch->pIndexData 
		   && IS_SET(rch->off_flags,ASSIST_VNUM)))

	   	{
		    CHAR_DATA *vch;
		    CHAR_DATA *target;
		    int number;

		    if (number_bits(1) == 0)
			continue;
		
		    target = NULL;
		    number = 0;
		    for (vch = ch->in_room->people; vch; vch = vch->next)
		    {
			if (can_see(rch,vch)
			&&  is_same_group(vch,victim)
			&&  number_range(0,number) == 0)
			{
			    target = vch;
			    number++;
			}
		    }

		    if (target != NULL)
		    {
			do_emote(rch,"{Rscreams and attacks!{x");
			multi_hit(rch,target,TYPE_UNDEFINED);
		    }
		}	
	    }
	}
    }
}


/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int     chance;

    /* decrement the wait */
    if (ch->desc == NULL)
	ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
	ch->daze = UMAX(0,ch->daze - PULSE_VIOLENCE); 


    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
	return;

    if (ch->stunned)
    {
	ch->stunned--;
	if (!ch->stunned)
	{
	    send_to_char( "You regain your equilibrium.\n\r", ch);
	    act( "$n regains $m equilibrium.", ch, NULL, NULL, TO_ROOM );
	}
        return;
    }

    if (IS_NPC(ch))
    {
	mob_hit(ch,victim,dt);
	return;
    }

    one_hit( ch, victim, dt, FALSE );

    if (get_eq_char (ch, WEAR_SECONDARY))
    {
	chance = (get_skill(ch,gsn_dual_wield)/3)*2;
	chance += 33;
	if ( number_percent( ) < chance )
	{
	    one_hit( ch, victim, dt, TRUE );
	    if ( get_skill(ch,gsn_dual_wield) != 0 && (!IS_NPC(ch)
	    && ch->level >= skill_table[gsn_dual_wield].skill_level[ch->class]))
	    {
		check_improve(ch,gsn_dual_wield,TRUE,1);
	    }
	}
        if ( ch->fighting != victim )
            return;
    }

    if (ch->fighting != victim)
	return;

    if (IS_AFFECTED(ch,AFF_HASTE))
	one_hit(ch,victim,dt, FALSE);

    if ( ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle)
	return;

    chance = get_skill(ch,gsn_second_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance /= 2;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch,gsn_second_attack,TRUE,5);
	if ( ch->fighting != victim )
	    return;
    } else {
	return;
    }

    chance = get_skill(ch,gsn_third_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance /= 2;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch,gsn_third_attack,TRUE,6);
	if ( ch->fighting != victim )
	    return;
    } else {
	return;
    }

    chance = get_skill(ch, gsn_fourth_attack)/2;

    if (IS_AFFECTED(ch, AFF_SLOW))
	chance /= 3;

    if (number_percent( ) < chance)
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch, gsn_fourth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    } else {
	return;
    }

    chance = get_skill(ch, gsn_fifth_attack)/2;

    if (IS_AFFECTED(ch, AFF_SLOW))
	chance = 0;

    if (number_percent( ) < chance)
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch, gsn_fifth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }

    return;
}

/* procedure for all mobile attacks */
void mob_hit (CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
    int chance,number;
    CHAR_DATA *vch, *vch_next;

    one_hit(ch,victim,dt, FALSE);

    if (ch->fighting != victim)
	return;

    if (ch->stunned)
	return;

    /* Area attack -- BALLS nasty! */
 
    if (IS_SET(ch->off_flags,OFF_AREA_ATTACK))
    {
	for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
	{
	    vch_next = vch->next;
	    if ((vch != victim && vch->fighting == ch))
		one_hit(ch,vch,dt, FALSE);
	}
    }

    if (ch->fighting != victim)
	return;

    if (get_eq_char (ch, WEAR_SECONDARY))
    {
	chance = (get_skill(ch,gsn_dual_wield)/3)*2;
	chance += 33;
	if ( number_percent( ) < chance )
	{
	    one_hit( ch, victim, dt, TRUE );
	}
        if ( ch->fighting != victim )
            return;
    }

    if (IS_AFFECTED(ch,AFF_HASTE) 
    ||  (IS_SET(ch->off_flags,OFF_FAST) && !IS_AFFECTED(ch,AFF_SLOW)))
	one_hit(ch,victim,dt, FALSE);

    if (ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle)
	return;

    chance = get_skill(ch,gsn_second_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
	chance /= 2;

    if (number_percent() < chance)
    {
	one_hit(ch,victim,dt, FALSE);
	if (ch->fighting != victim)
	    return;
	chance = get_skill(ch,gsn_third_attack)/2;

	if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
	    chance /= 2;

	if (number_percent() < chance)
	{
	    one_hit(ch,victim,dt, FALSE);
	    if (ch->fighting != victim)
		return;

	    chance = get_skill(ch, gsn_fourth_attack)/2;

	    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
		chance /= 3;

	    if (number_percent( ) < chance)
	    {
		one_hit(ch,victim,dt, FALSE);
		if (ch->fighting != victim)
		    return;

		chance = get_skill(ch, gsn_fifth_attack)/2;

		if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
		    chance = 0;

		if (number_percent( ) < chance)
		{
		    one_hit(ch,victim,dt, FALSE);
		    if (ch->fighting != victim)
			return;
		}
	    }
	}
    }

    /* oh boy!  Fun stuff! */

    if (ch->wait > 0)
	return;

    number = number_range(0,2);

    if (number == 1 && IS_SET(ch->act,ACT_MAGE))
    {
	/*  { mob_cast_mage(ch,victim); return; } */ ;
    }

    if (number == 2 && IS_SET(ch->act,ACT_CLERIC))
    {	
	/* { mob_cast_cleric(ch,victim); return; } */ ;
    }

    /* now for the skills */

    number = number_range(0,8);

    switch(number) 
    {
    case (0) :
	if (IS_SET(ch->off_flags,OFF_BASH))
	    do_bash(ch,"");
	break;

    case (1) :
	if (IS_SET(ch->off_flags,OFF_BERSERK) && !IS_AFFECTED(ch,AFF_BERSERK))
	    do_berserk(ch,"");
	break;


    case (2) :
	if (IS_SET(ch->off_flags,OFF_DISARM) 
	|| (get_weapon_sn(ch) != gsn_hand_to_hand 
	&& (IS_SET(ch->act,ACT_WARRIOR)
	||  IS_SET(ch->act,ACT_VAMPIRE)
   	||  IS_SET(ch->act,ACT_THIEF))))
	    do_disarm(ch,"");
	break;

    case (3) :
	if (IS_SET(ch->off_flags,OFF_KICK))
	    do_kick(ch,"");
	break;

    case (4) :
	if (IS_SET(ch->off_flags,OFF_KICK_DIRT))
	    do_dirt(ch,"");
	break;

    case (5) :
	if (IS_SET(ch->off_flags,OFF_TAIL))
	{
	    /* do_tail(ch,"") */ ;
	}
	break; 

    case (6) :
	if (IS_SET(ch->off_flags,OFF_TRIP))
	    do_trip(ch,"");
	break;

    case (7) :
	if (IS_SET(ch->off_flags,OFF_CRUSH))
	{
	    /* do_crush(ch,"") */ ;
	}
	break;
    case (8) :
	if (IS_SET(ch->off_flags,OFF_BACKSTAB))
	{
	    do_backstab(ch,"");
	}
    }
}
	

/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary )
{
    OBJ_DATA *wield;
    OBJ_DATA *quiver;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int sn,skill;
    int dam_type;
    bool result;

    sn = -1;


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
	return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     * if secondary == true, use the second weapon.
     */
    if ((!secondary) || (get_eq_char(ch,WEAR_SECONDARY)->item_type) == ITEM_QUIVER)
        wield = get_eq_char( ch, WEAR_WIELD );
    else
        wield = get_eq_char( ch, WEAR_SECONDARY );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else 
	    dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
    	if (wield != NULL)
    	    dam_type = attack_table[wield->value[3]].damage;
    	else
    	    dam_type = attack_table[ch->dam_type].damage;
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = 20 + get_weapon_skill(ch,sn);

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = 20;
	thac0_32 = -4;   /* as good as a thief */ 
	if (IS_SET(ch->act,ACT_VAMPIRE))
	    thac0_32 = -30;
        else if (IS_SET(ch->act,ACT_DRUID))
            thac0_32 = 0;
        else if (IS_SET(ch->act,ACT_RANGER))
            thac0_32 = -4;
	else if (IS_SET(ch->act,ACT_WARRIOR))
	    thac0_32 = -10;
	else if (IS_SET(ch->act,ACT_THIEF))
	    thac0_32 = -4;
	else if (IS_SET(ch->act,ACT_CLERIC))
	    thac0_32 = 2;
	else if (IS_SET(ch->act,ACT_MAGE))
	    thac0_32 = 6;
    }
    else
    {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0  = interpolate( ch->level, thac0_00, thac0_32 );

    if (thac0 < 0)
        thac0 = thac0/2;

    if (thac0 < -5)
        thac0 = -5 + (thac0 + 5) / 2;

    thac0 -= GET_HITROLL(ch) * skill/100;
    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab)
	thac0 -= 10 * (100 - get_skill(ch,gsn_backstab));

    if (dt == gsn_circle)
	thac0 -= 10 * (100 - get_skill(ch,gsn_circle));

     victim_ac = GET_AC(victim,DEFENCE)/10;

	
    if (victim_ac < -15)
	victim_ac = (victim_ac + 15) / 5 - 15;
     
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    if ( victim->position < POS_FIGHTING)
	victim_ac += 4;
 
    if (victim->position < POS_RESTING)
	victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt, dam_type, TRUE );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) && (!ch->pIndexData->new_format || wield == NULL))
	if (!ch->pIndexData->new_format)
	{
	    dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	    if ( wield != NULL )
	    	dam += dam / 2;
	}
	else
	    dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);
	
    else
    {
	if (sn != -1)
	    check_improve(ch,sn,TRUE,5);
	if ( wield != NULL )
	{
	    if (wield->clan)
	    {
		dam = dice(ch->level/3,3) * skill/100;
	    }

        else if (wield->value[0] != WEAPON_BOW)
	    {
		if (wield->pIndexData->new_format)
		    dam = dice(wield->value[1],wield->value[2]) * skill/100;
		else
	    	    dam = number_range( wield->value[1] * skill/100, 
				wield->value[2] * skill/100);
	    }

        else
        {
            quiver = get_eq_char(ch,WEAR_SECONDARY);
                if (quiver == NULL)
                {
                    send_to_char("You need to second wield a quiver!\n\r",ch);
                    return;
                }

                if (quiver->value[0] < 1)
                {
                    send_to_char("You're out of arrows!\n\r",ch);
                    return;
                }

                dam = dice((wield->value[1]+quiver->value[1]), (wield->value[2]+quiver->value[2]))* skill/100;

                quiver->value[0] -= 1; /* Lose 1 arrow */

//                if (class_table[ch->class].name != "Ranger"
//                 || class_table[ch->class].name != "Strider"
//                 || class_table[ch->class].name != "Pathfinder")
//                    dam *= 2/3;
//broken, no idea why yet
            }
	    if (get_eq_char(ch,WEAR_SHIELD) == NULL)  /* no shield = more */
		dam = dam * 11/10;

	    /* sharpness! */
	    if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
	    {
		int percent;

		if ((percent = number_percent()) <= (skill / 8))
		    dam = 2 * dam + (dam * 2 * percent / 100);
	    }
	}
	else
	    dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
    }

    /*
     * Bonuses.
     */
    if ( get_skill(ch,gsn_enhanced_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_enhanced_damage))
        {
            check_improve(ch,gsn_enhanced_damage,TRUE,6);
            dam += 2 * ( dam * diceroll/300);
        }
    }

    if ( get_skill(ch,gsn_critical_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_critical_damage))
        {
            check_improve(ch,gsn_critical_damage,TRUE,6);
            dam += 2.5 * ( dam * diceroll/300);
        }
    }

    if (get_skill(ch,gsn_piercing) > 0 && (get_eq_char(ch,WEAR_WIELD)->value[0] != WEAPON_BOW))
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_piercing))
        {
            check_improve(ch,gsn_piercing,TRUE,6);
            dam += 2.5 * ( dam * diceroll/300);
        }
    }



    if ( !IS_AWAKE(victim) )
	dam *= 2;
     else if (victim->position < POS_FIGHTING)
	dam = dam * 3 / 2;

    if (dt== gsn_backstab && IS_SHIELDED(ch,SHD_CAMO))
    dam *= 3;

    if ( dt == gsn_backstab && wield != NULL) 
   {
    	if ( wield->value[0] != 2 )
	    dam *= 2 + (ch->level / 10); 
	else 
	    dam *= 2 + (ch->level / 8);
    }
    if ( dt == gsn_circle && wield != NULL) 
    {
    	if ( wield->value[0] != 2 )
	    dam *= 2 + (ch->level / 15); 
	else 
	    dam *= 2 + (ch->level / 12);
    }
    dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;

    if ( dam <= 0 )
	dam = 1;


        if ( !check_counter( ch, victim, dam, dt ) )
        result = damage( ch, victim, dam, dt, dam_type, TRUE );



    else return;
    /* but do we have a funky weapon? */
    if (result && wield != NULL)
    { 
	int dam;

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON))
	{
	    int level;
	    AFFECT_DATA *poison, af;

	    if ((poison = affect_find(wield->affected,gsn_poison)) == NULL)
		level = wield->level;
	    else
		level = poison->level;
	
	    if (!saves_spell(level / 2,victim,DAM_POISON)) 
	    {
		send_to_char("{cYou feel {ypoison{c coursing through your veins.{x",
		    victim);
		act("$n is {ypoisoned{x by the venom on $p.",
		    victim,wield,NULL,TO_ROOM);

    		af.where     = TO_AFFECTS;
    		af.type      = gsn_poison;
    		af.level     = level * 3/4;
    		af.duration  = level / 2;
    		af.location  = APPLY_STR;
    		af.modifier  = -1;
    		af.bitvector = AFF_POISON;
    		affect_join( victim, &af );
	    }

	    /* weaken the poison if it's temporary */
	    if (poison != NULL)
	    {
	    	poison->level = UMAX(0,poison->level - 2);
	    	poison->duration = UMAX(0,poison->duration - 1);
	
	    	if (poison->level == 0 || poison->duration == 0)
		    act("The {ypoison{x on $p has worn off.",ch,wield,NULL,TO_CHAR);
	    }
 	}


    	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
	{
	    dam = number_range(1, wield->level / 5 + 1);
	    act("{k$p draws life from $n.{x",victim,wield,NULL,TO_ROOM);
	    act("{iYou feel $p drawing your life away.{x",
		victim,wield,NULL,TO_CHAR);
	    damage_old(ch,victim,dam,0,DAM_NEGATIVE,FALSE);
	    ch->alignment = UMAX(-1000,ch->alignment - 1);
	    if ( ch->pet != NULL )
		ch->pet->alignment = ch->alignment;
	    ch->hit += dam/2;
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
	{
	    dam = number_range(1,wield->level / 4 + 1);
	    act("{k$n is {rburned{k by $p.{x",victim,wield,NULL,TO_ROOM);
	    act("{i$p {rsears{i your flesh.{x",victim,wield,NULL,TO_CHAR);
	    fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_FIRE,FALSE);
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST))
	{
	    dam = number_range(1,wield->level / 6 + 2);
	    act("{k$p {cfreezes{k $n.{x",victim,wield,NULL,TO_ROOM);
	    act("{iThe {Ccold{i touch of $p surrounds you with {Cice.{x",
		victim,wield,NULL,TO_CHAR);
	    cold_effect(victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_COLD,FALSE);
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING))
	{
	    dam = number_range(1,wield->level/5 + 2);
	    act("{k$n is struck by {Ylightning{k from $p.{x",victim,wield,NULL,TO_ROOM);
	    act("{iYou are {Yshocked{i by $p.{x",victim,wield,NULL,TO_CHAR);
	    shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE);
	}
    }
    if (ch->fighting == victim);
    {
	if (result)
	{
	    if (IS_SHIELDED(victim, SHD_ICE))
	    {
		if (!IS_SHIELDED(ch, SHD_ICE))
		{
		    dt = skill_lookup("iceshield");
		    dam = number_range(5, 15);
		    damage_old(victim, ch, dam, dt, DAM_COLD, TRUE);
		}
	    }
	    if (IS_SHIELDED(victim, SHD_FIRE))
	    {
		if (!IS_SHIELDED(ch, SHD_FIRE))
		{
		    dt = skill_lookup("fireshield");
		    dam = number_range(10, 20);
		    damage_old(victim, ch, dam, dt, DAM_FIRE, TRUE);
		}
	    }
	    if (IS_SHIELDED(victim, SHD_SHOCK))
	    {
		if (!IS_SHIELDED(ch, SHD_SHOCK))
		{
		    dt = skill_lookup("shockshield");
		    dam = number_range(15, 25);
		    damage_old(victim, ch, dam, dt, DAM_LIGHTNING, TRUE);
		}
	    }

	   if (IS_SHIELDED(victim, SHD_ACID))
	    {
		if (!IS_SHIELDED(ch, SHD_ACID))
		{
		    dt = skill_lookup("acidshield");
		    dam = number_range(20, 30);
		    damage_old(victim, ch, dam, dt, DAM_ACID, TRUE);
		}
	    }
	    if (IS_SHIELDED(victim, SHD_WATER))
	    {
		if (!IS_SHIELDED(ch, SHD_WATER))
		{
		    dt = skill_lookup("watershield");
		    dam = number_range(20, 30);
		    damage_old(victim, ch, dam, dt, DAM_DROWNING, TRUE);
		}
	    }
	    if (IS_SHIELDED(victim, SHD_HOLY))
	    {
		if (!IS_SHIELDED(ch, SHD_HOLY))
		{
		    dt = skill_lookup("holyshield");
		    dam = number_range(20, 30);
		    damage_old(victim, ch, dam, dt, DAM_HOLY, TRUE);
		}
	    }
	    if (IS_SHIELDED(victim, SHD_GLASS))
	    {
		if (!IS_SHIELDED(ch, SHD_GLASS))
		{
		    dt = skill_lookup("glassshield");
		    dam = number_range(20, 30);
		    damage_old(victim, ch, dam, dt, DAM_SLASH, TRUE);
		}
	    }
	}
    }

    tail_chain( );
    return;
}

/*
 * Mock hit one guy once.
 */
void one_hit_mock( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int sn,skill;
    int dam_type;
    bool result;

    sn = -1;


    /* just in case */
    if (ch == NULL || victim == NULL)
	return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     * if secondary == true, use the second weapon.
     */
    if (!secondary)
        wield = get_eq_char( ch, WEAR_WIELD );
    else
        wield = get_eq_char( ch, WEAR_SECONDARY );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else 
	    dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
    	if (wield != NULL)
    	    dam_type = attack_table[wield->value[3]].damage;
    	else
    	    dam_type = attack_table[ch->dam_type].damage;
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = 20 + get_weapon_skill(ch,sn);

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = 20;
	thac0_32 = -4;   /* as good as a thief */ 
        if (IS_SET(ch->act,ACT_VAMPIRE))
            thac0_32 = -30;
        else if (IS_SET(ch->act,ACT_DRUID))
            thac0_32 = 0;
        else if (IS_SET(ch->act,ACT_RANGER))
            thac0_32 = -4;
        else if (IS_SET(ch->act,ACT_WARRIOR))
	    thac0_32 = -10;
	else if (IS_SET(ch->act,ACT_THIEF))
	    thac0_32 = -4;
	else if (IS_SET(ch->act,ACT_CLERIC))
	    thac0_32 = 2;
	else if (IS_SET(ch->act,ACT_MAGE))
	    thac0_32 = 6;
    }
    else
    {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0  = interpolate( ch->level, thac0_00, thac0_32 );

    if (thac0 < 0)
        thac0 = thac0/2;

    if (thac0 < -5)
        thac0 = -5 + (thac0 + 5) / 2;

    thac0 -= GET_HITROLL(ch) * skill/100;
    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab)
	thac0 -= 10 * (100 - get_skill(ch,gsn_backstab));

    if (dt == gsn_circle)
	thac0 -= 10 * (100 - get_skill(ch,gsn_circle));

    if (dt == gsn_strike)
	thac0 -= 10 * (100 - get_skill(ch,gsn_strike));

    victim_ac = GET_AC(victim,DEFENCE)/10;

	
    if (victim_ac < -15)
	victim_ac = (victim_ac + 15) / 5 - 15;
     
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    if ( victim->position < POS_FIGHTING)
	victim_ac += 4;
 
    if (victim->position < POS_RESTING)
	victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage_mock( ch, victim, 0, dt, dam_type, TRUE );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) && (!ch->pIndexData->new_format || wield == NULL))
	if (!ch->pIndexData->new_format)
	{
	    dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	    if ( wield != NULL )
	    	dam += dam / 2;
	}
	else
	    dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);
	
    else
    {
	if (sn != -1)
	    check_improve(ch,sn,TRUE,5);
	if ( wield != NULL )
	{
	    if (wield->clan)
	    {
		dam = dice(ch->level/3,3) * skill/100;
	    } else
	    {
		if (wield->pIndexData->new_format)
		    dam = dice(wield->value[1],wield->value[2]) * skill/100;
		else
	    	    dam = number_range( wield->value[1] * skill/100, 
				wield->value[2] * skill/100);
	    }

	    if (get_eq_char(ch,WEAR_SHIELD) == NULL)  /* no shield = more */
		dam = dam * 11/10;

	    /* sharpness! */
	    if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
	    {
		int percent;

		if ((percent = number_percent()) <= (skill / 8))
		    dam = 2 * dam + (dam * 2 * percent / 100);
	    }
	}
	else
	    dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
    }

    /*
     * Bonuses.
     */
    if ( get_skill(ch,gsn_enhanced_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_enhanced_damage))
        {
            check_improve(ch,gsn_enhanced_damage,TRUE,6);
            dam += 2 * ( dam * diceroll/300);
        }
    }

    if ( get_skill(ch,gsn_critical_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_critical_damage))
        {
            check_improve(ch,gsn_critical_damage,TRUE,6);
            dam += 2.5 * ( dam * diceroll/300);
        }
    }

    if (get_skill(ch,gsn_piercing) > 0 && (get_eq_char(ch,WEAR_WIELD)->value[0] != WEAPON_BOW))
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_piercing))
        {
            check_improve(ch,gsn_piercing,TRUE,6);
            dam += 2.5 * ( dam * diceroll/300);
        }
    }


    if ( !IS_AWAKE(victim) )
	dam *= 2;
     else if (victim->position < POS_FIGHTING)
	dam = dam * 3 / 2;

    if ( dt == gsn_backstab && wield != NULL) 
    {
    	if ( wield->value[0] != 2 )
	    dam *= 2 + (ch->level / 10); 
	else 
	    dam *= 2 + (ch->level / 8);
    }
    if ( dt == gsn_circle && wield != NULL) 
    {
    	if ( wield->value[0] != 2 )
	    dam *= 2 + (ch->level / 15); 
	else 
	    dam *= 2 + (ch->level / 12);
    }
    dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;

    if ( dam <= 0 )
	dam = 1;

    result = damage_mock( ch, victim, dam, dt, dam_type, TRUE );
    
    tail_chain( );
    return;
}

/*
 * Inflict damage from a hit.
 */
bool damage(CHAR_DATA *ch,CHAR_DATA *victim,int dam,int dt,int dam_type,
	    bool show) 
{

    int damreduce, victim_ac, saves;
    OBJ_DATA *corpse;
    bool immune;

    if ( victim->position == POS_DEAD )
	return FALSE;

    /*
     * Stop up any residual loopholes.
     */
/*    if ( dam > 1200 && dt >= TYPE_HIT && !IS_IMMORTAL(ch))
    {
	bug( "Damage: %d: more than 1200 points!", dam );
	dam = 1200;
	if (!IS_IMMORTAL(ch))
	{
	    OBJ_DATA *obj;
	    obj = get_eq_char( ch, WEAR_WIELD );
	    send_to_char("{cYou {z{Breally{x{c shouldn't cheat.{x\n\r",ch);
	    if (obj != NULL)
	    	extract_obj(obj);
	}

    } */


    /* damage reduction */
    if ( dam > 35)
	dam = (dam - 35)/2 + 35;
    if ( dam > 80)
	dam = (dam - 80)/2 + 80; 



   
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return FALSE;

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
            {
		set_fighting( victim, ch );
           if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
		    mp_percent_trigger( victim, ch, NULL, NULL, TRIG_KILL );
	    }
	    if (victim->timer <= 4)
	    	victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     taken out by Russ! */
/*
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return FALSE;
	    }
*/
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );
    }

    /*
     * Inviso attacks ... not.
     */
    if ( IS_SHIELDED(ch, SHD_INVISIBLE) || IS_SHIELDED(ch, SHD_CAMO) )
    {
	affect_strip( ch, gsn_invis );
	affect_strip( ch, gsn_mass_invis );
    affect_strip( ch, gsn_camo    );
   	REMOVE_BIT( ch->shielded_by, SHD_INVISIBLE );
    REMOVE_BIT( ch->shielded_by, SHD_CAMO );
	act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim) 
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
	dam = 9 * dam / 10;

/*    if ( IS_WEAPON_STAT(get_eq_char(ch,WEAR_WIELD),WEAPON_TWO_HANDS))
    dam *= 1.5;*/

    if (IS_SET(victim->act,PLR_TWIT))
    dam *=2;

    if ( dam > 1 && IS_SHIELDED(victim, SHD_SANCTUARY) )
	dam /= 2;

    if ( dam > 3 && IS_SHIELDED(victim, SHD_IMPROVED_SANCTUARY) )
    dam /= 3;
    if ( dam > 1 && ((IS_SHIELDED(victim, SHD_PROTECT_EVIL) && IS_EVIL(ch) )
    ||		     (IS_SHIELDED(victim, SHD_PROTECT_GOOD) && IS_GOOD(ch) )))
	dam -= dam / 4;

    victim_ac = GET_AC(victim,DEFENCE);
    damreduce = victim_ac/109;
    saves = ch->saving_throw/22;

    if (dam_type != DAM_NONE || dam_type != DAM_BASH || dam_type != DAM_PIERCE || dam_type != DAM_SLASH)
        {
        if (saves < -50)
            saves = -50;
            saves *= -1;
        dam *= (100-saves);
        dam /= 100;
        }
    if (dam_type == DAM_NONE || dam_type == DAM_BASH || dam_type == DAM_PIERCE || dam_type == DAM_SLASH)
        {
        if (damreduce > 50)
            damreduce=50;
        dam *= (100-damreduce);
        dam /= 100;
        }
    immune = FALSE;

    /*
     * Check for parry, and dodge.
     */
    if ( dt >= TYPE_HIT && ch != victim)
    {
        if ( check_parry( ch, victim ) )
	    return FALSE;
	if ( check_dodge( ch, victim ) )
	    return FALSE;
	if ( check_shield_block(ch,victim))
	    return FALSE;

    }

    switch(check_immune(victim,dam_type))
    {
	case(IS_IMMUNE):
	    immune = TRUE;
	    dam = 0;
	    break;
	case(IS_RESISTANT):	
	    dam -= dam/3;
	    break;
	case(IS_VULNERABLE):
	    dam += dam/2;
	    break;
    }

    if (show)
    	dam_message( ch, victim, dam, dt, immune );

    if (dam == 0)
	return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );
    if (dt == gsn_feed)
    {
	ch->hit = UMIN(ch->hit+((dam/3)*2), ch->max_hit);
	update_pos( ch );
    }

    if (dt == gsn_death_bite)
    {
	ch->hit = UMIN(ch->hit+((dam/3)*2), ch->max_hit);
	update_pos( ch );
    }

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "{c$n is mortally wounded, and will die soon, if not aided.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "{cYou are mortally wounded, and will die soon, if not aided.{x\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "{c$n is incapacitated and will slowly die, if not aided.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "{cYou are incapacitated and will slowly {z{Rdie{x{c, if not aided.{x\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "{c$n is stunned, but will probably recover.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("{cYou are stunned, but will probably recover.{x\n\r",
	    victim );
	break;

    case POS_DEAD:
        if ((IS_NPC(victim)) && ( victim->die_descr != NULL && victim->die_descr[0] != '\0'))
	{
	    act( "{c$n $T{x", victim, 0, victim->die_descr, TO_ROOM );
	}
	else
	{
	    act( "{c$n is {CDEAD!!{x", victim, 0, 0, TO_ROOM );
	}
	send_to_char( "{cYou have been {RKILLED!!{x\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "{cThat really did {RHURT!{x\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "{cYou sure are {z{RBLEEDING!{x\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

     if (victim->position == POS_DEAD && nocorpse && !IS_NPC(victim))
    {
    stop_fighting(victim,TRUE);
    char_from_room(victim);
    char_to_room(victim,get_room_index(ROOM_VNUM_ALTAR));
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    affect_strip(victim,gsn_sleep);
    affect_strip(victim,gsn_curse);
    victim->move = victim->max_move;
    update_pos( victim );
    do_look(victim, "auto");
    }

    if (victim->position == POS_DEAD && !IS_NPC(victim) && !IS_NPC(ch)
    && (IS_SET(victim->in_room->room_flags,ROOM_ARENA)))
    {
    stop_fighting(victim,TRUE);
    char_from_room(victim);
    char_to_room(victim,get_room_index(ROOM_VNUM_ALTAR));
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    affect_strip(victim,gsn_sleep);
    affect_strip(victim,gsn_curse);
    victim->move = victim->max_move;
    update_pos( victim );
    do_look(victim, "auto");

    stop_fighting(ch,TRUE);
    char_from_room(ch);
    char_to_room(ch,get_room_index(ROOM_VNUM_TEMPLE));
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    affect_strip(ch,gsn_plague);
    affect_strip(ch,gsn_poison);
    affect_strip(ch,gsn_blindness);
    affect_strip(ch,gsn_sleep);
    affect_strip(ch,gsn_curse);
    ch->move = ch->max_move;
    update_pos( ch );
    do_look(ch, "auto");

    return TRUE;
    }
    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		ch->in_room->vnum );
	    log_string( log_buf );

	    /*
	     * Dying penalty:
	     * 2/3 way back to previous level.
	     */
	    if ( victim->exp > exp_per_level(victim,victim->pcdata->points) 
			       * victim->level )
	gain_exp( victim, (5 * (exp_per_level(victim,victim->pcdata->points)
			         * victim->level - victim->exp)/6) + 50 );
	}

        sprintf( log_buf, "%s got toasted by %s at %s [room %d]",
            (IS_NPC(victim) ? victim->short_descr : victim->name),
            (IS_NPC(ch) ? ch->short_descr : ch->name),
            ch->in_room->name, ch->in_room->vnum);
 
        if (IS_NPC(victim))
            wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
        else
            wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);

     	/*
	 * Death trigger
	 */
	if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DEATH) )
	{
	    victim->position = POS_STANDING;
	    mp_percent_trigger( victim, ch, NULL, NULL, TRIG_DEATH );
	}

	raw_kill( victim, ch );
        /* dump the flags */
        if (ch != victim && !IS_NPC(ch) && (!is_same_clan(ch,victim)
	|| clan_table[victim->clan].independent))
        {
            if (IS_SET(victim->act,PLR_TWIT))
                REMOVE_BIT(victim->act,PLR_TWIT);
        
	    if (IS_SET(victim->act,PLR_KILLER))
                REMOVE_BIT(victim->act,PLR_KILLER);
	}

        /* RT new auto commands */

	if ( !IS_NPC(ch) && IS_NPC(victim) )
	{
	    OBJ_DATA *coins;

	    corpse = get_obj_list( ch, "corpse", ch->in_room->contents ); 

	    if ( IS_SET(ch->act, PLR_AUTOLOOT) &&
		 corpse && corpse->contains) /* exists and not empty */
		do_get( ch, "all corpse" );

 	    if (IS_SET(ch->act,PLR_AUTOGOLD) &&
	        corpse && corpse->contains  && /* exists and not empty */
		!IS_SET(ch->act,PLR_AUTOLOOT))
		if ((coins = get_obj_list(ch,"gcash",corpse->contains))
		     != NULL)
	      	    do_get(ch, "all.gcash corpse");
            
	    if ( IS_SET(ch->act, PLR_AUTOSAC) )
       	{
              if ( IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains)
		return TRUE;  /* leave if corpse has treasure */
	      else
		do_sacrifice( ch, "corpse" );
        }
	}

	return TRUE;
    }

    if ( victim == ch )
	return TRUE;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return TRUE;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
	&&   victim->hit < victim->max_hit / 5) 
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait < PULSE_VIOLENCE / 2 )
	do_flee( victim, "" );

    tail_chain( );
    return TRUE;
}
/*
 * Show damage from a mock hit.
 */
bool damage_mock(CHAR_DATA *ch,CHAR_DATA *victim,int dam,int dt,int dam_type,
	    bool show) 
{
    long immdam;
    bool immune;
    char buf1[256], buf2[256], buf3[256];
    const char *attack;

    if ( victim->position == POS_DEAD )
	return FALSE;

    if ( dam > 35)
	dam = (dam - 35)/2 + 35;
    if ( dam > 80)
	dam = (dam - 80)/2 + 80; 
    if ( is_safe_mock( ch, victim ) )
	    return FALSE;
    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim) 
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
	dam = 9 * dam / 10;

    if ( dam > 1 && IS_SHIELDED(victim, SHD_SANCTUARY) )
	dam /= 2;

    if ( dam > 1 && ((IS_SHIELDED(victim, SHD_PROTECT_EVIL) && IS_EVIL(ch) )
    ||		     (IS_SHIELDED(victim, SHD_PROTECT_GOOD) && IS_GOOD(ch) )))
	dam -= dam / 4;

    immune = FALSE;


    switch(check_immune(victim,dam_type))
    {
	case(IS_IMMUNE):
	    immune = TRUE;
	    dam = 0;
	    break;
	case(IS_RESISTANT):	
	    dam -= dam/3;
	    break;
	case(IS_VULNERABLE):
	    dam += dam/2;
	    break;
    }

    if ( dt >= 0 && dt < MAX_SKILL )
        attack      = skill_table[dt].noun_damage;
    else if ( dt >= TYPE_HIT
    && dt <= TYPE_HIT + MAX_DAMAGE_MESSAGE)
        attack      = attack_table[dt - TYPE_HIT].noun;
    else
    {
        bug( "Dam_message: bad dt %d.", dt );
         dt  = TYPE_HIT;
        attack  = attack_table[0].name;
    }
    immdam = 0;
    if (ch->level == MAX_LEVEL)
    {
	immdam = dam*63;
    }
    if (ch == victim)
    {
	sprintf( buf1, "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {y$mself{g.{x",  attack, dam );
	sprintf( buf2, "{yYour {gmock {B%s{g would have done {R%d hp{g damage to {yyourself{g.{x",  attack, dam );
	act(buf1,ch,NULL,NULL,TO_ROOM);
	act(buf2,ch,NULL,NULL,TO_CHAR);
    }
    else if (ch->level < MAX_LEVEL)
    {
	sprintf( buf1, "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {y$N{g.{x",  attack, dam );
	sprintf( buf2, "{yYour {gmock {B%s{g would have done {R%d hp{g damage to {y$N{g.{x",  attack, dam );
	sprintf( buf3, "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {yyou{g.{x", attack, dam );
	act( buf1, ch, NULL, victim, TO_NOTVICT );
	act( buf2, ch, NULL, victim, TO_CHAR );
	act( buf3, ch, NULL, victim, TO_VICT );
    }
    else
    {
        sprintf( buf1, "{y$n's {gmock {B%s{g would have done {R%lu hp{g damage to {y$N{g.{x",  attack, immdam );
        sprintf( buf2, "{yYour {gmock {B%s{g would have done {R%lu hp{g damage to {y$N{g.{x",  attack, immdam );
        sprintf( buf3, "{y$n's {gmock {B%s{g would have done {R%lu hp{g damage to {yyou{g.{x", attack, immdam );
        act( buf1, ch, NULL, victim, TO_NOTVICT );
        act( buf2, ch, NULL, victim, TO_CHAR );
        act( buf3, ch, NULL, victim, TO_VICT );
    }

    tail_chain( );
    return TRUE;
}

/*
 * Inflict damage from a hit.
 */
bool damage_old( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int 
dam_type, bool show ) {

    OBJ_DATA *corpse;
    bool immune;
    int damreduce, victim_ac, saves;

    if ( victim->position == POS_DEAD )
	return FALSE;

    /*
     * Stop up any residual loopholes.
     */

/*    if ( dam > 1200 && dt >= TYPE_HIT && !IS_IMMORTAL(ch))
    {
        bug( "Damage: %d: more than 1200 points!", dam );
        dam = 1200;
        if (!IS_IMMORTAL(ch))
        {
            OBJ_DATA *obj;
            obj = get_eq_char( ch, WEAR_WIELD );
            send_to_char("{cYou {z{Greally{x{c shouldn't cheat.{x\n\r",ch);
            if (obj != NULL)
                extract_obj(obj);
        }
 
    } */

    
    /* damage reduction */
    if ( dam > 35)
	dam = (dam - 35)/2 + 35;
    if ( dam > 80)
	dam = (dam - 80)/2 + 80; 



   
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return FALSE;

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    if (victim->timer <= 4)
	    	victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return FALSE;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );
    }

    /*
     * Inviso attacks ... not.
     */
     if (IS_SHIELDED(ch,SHD_CAMO))
    dam *=2;

    if ( IS_SHIELDED(ch, SHD_INVISIBLE) || IS_SHIELDED(ch, SHD_CAMO) )
    {
	affect_strip( ch, gsn_invis );
	affect_strip( ch, gsn_mass_invis );
    affect_strip( ch, gsn_camo  );
	REMOVE_BIT( ch->shielded_by, SHD_INVISIBLE );
	REMOVE_BIT( ch->shielded_by, SHD_CAMO );
	act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim)
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
        dam = 9 * dam / 10;
 
    if ( dam > 1 && IS_SHIELDED(victim, SHD_SANCTUARY) )
	dam /= 2;

    if ( dam > 3 && IS_SHIELDED(victim, SHD_IMPROVED_SANCTUARY) )
    dam /= 3;
    if ( dam > 1 && ((IS_SHIELDED(victim, SHD_PROTECT_EVIL) && IS_EVIL(ch) )
    ||		     (IS_SHIELDED(victim, SHD_PROTECT_GOOD) && IS_GOOD(ch) )))
	dam -= dam / 4;

    victim_ac = GET_AC(victim,DEFENCE);
    damreduce = victim_ac/150;
    saves = ch->saving_throw/20;

    if (dam_type != DAM_NONE || dam_type != DAM_BASH || dam_type != DAM_PIERCE || dam_type != DAM_SLASH)
        {
        if (saves < -50)
            saves = -50;
            saves *= -1;
        dam *= (100-saves);
        dam /= 100;
        }

    if (dam_type == DAM_NONE || dam_type == DAM_BASH || dam_type == DAM_PIERCE || dam_type == DAM_SLASH)
        {
        if (damreduce > 50)
            damreduce=50;
        dam *= (100-damreduce);
        dam /= 100;
        }

    immune = FALSE;


    /*
     * Check for parry, and dodge.
     */
    if ( dt >= TYPE_HIT && ch != victim)
    {
        if ( check_parry( ch, victim ) )
	    return FALSE;
	if ( check_dodge( ch, victim ) )
	    return FALSE;
	if ( check_shield_block(ch,victim))
	    return FALSE;

    }

    switch(check_immune(victim,dam_type))
    {
	case(IS_IMMUNE):
	    immune = TRUE;
	    dam = 0;
	    break;
	case(IS_RESISTANT):	
	    dam -= dam/3;
	    break;
	case(IS_VULNERABLE):
	    dam += dam/2;
	    break;
    }

    if (show)
    	dam_message( ch, victim, dam, dt, immune );

    if (dam == 0)
	return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );
    if (dt == gsn_feed)
    {
	ch->hit = UMIN(ch->hit+((dam/3)*2), ch->max_hit);
	update_pos( ch );
    }

    if (dt == gsn_death_bite)
    {
	ch->hit = UMIN(ch->hit+((dam/3)*2), ch->max_hit);
	update_pos( ch );
    }

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "{c$n is mortally wounded, and will die soon, if not aided.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "{cYou are mortally wounded, and will die soon, if not aided.{x\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "{c$n is incapacitated and will slowly die, if not aided.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "{cYou are incapacitated and will slowly {Rdie{c, if not aided.{x\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "{c$n is stunned, but will probably recover.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("{cYou are stunned, but will probably recover.{x\n\r",
	    victim );
	break;

    case POS_DEAD:
        if ((IS_NPC(victim)) && ( victim->die_descr != NULL && victim->die_descr[0] != '\0'))
        { 
            act( "{c$n $T{x", victim, 0, victim->die_descr, TO_ROOM ); 
        } 
        else 
        {  
	    act( "{c$n is {CDEAD!!{x", victim, 0, 0, TO_ROOM );
	}
	send_to_char( "{cYou have been {RKILLED!!{x\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "{cThat really did {RHURT!{x\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "{cYou sure are {z{RBLEEDING!{x\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    if (victim->position == POS_DEAD && nocorpse && !IS_NPC(victim))
    {
    stop_fighting(victim,TRUE);
    char_from_room(victim);
    char_to_room(victim,get_room_index(ROOM_VNUM_ALTAR));
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    affect_strip(victim,gsn_sleep);
    affect_strip(victim,gsn_curse);
    victim->move = victim->max_move;
    update_pos( victim );
    do_look(victim, "auto");
    }

    if (victim->position == POS_DEAD && !IS_NPC(victim) && !IS_NPC(ch)
    && (IS_SET(victim->in_room->room_flags,ROOM_ARENA)))
    {
    stop_fighting(victim,TRUE);
    char_from_room(victim);
    char_to_room(victim,get_room_index(ROOM_VNUM_ALTAR));
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    affect_strip(victim,gsn_sleep);
    affect_strip(victim,gsn_curse);
    victim->move = victim->max_move;
    update_pos( victim );
    do_look(victim, "auto");

    stop_fighting(ch,TRUE);
    char_from_room(ch);
    char_to_room(ch,get_room_index(ROOM_VNUM_TEMPLE));
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    affect_strip(ch,gsn_plague);
    affect_strip(ch,gsn_poison);
    affect_strip(ch,gsn_blindness);
    affect_strip(ch,gsn_sleep);
    affect_strip(ch,gsn_curse);
    ch->move = ch->max_move;
    update_pos( ch );
    do_look(ch, "auto");

    return TRUE;
    }
    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string( log_buf );

	    /*
	     * Dying penalty:
	     * 2/3 way back to previous level.
	     */
	    if ( victim->exp > exp_per_level(victim,victim->pcdata->points) 
			       * victim->level )
	gain_exp( victim, (2 * (exp_per_level(victim,victim->pcdata->points)
			         * victim->level - victim->exp)/3) + 50 );
	}

        sprintf( log_buf, "%s got toasted by %s at %s [room %d]",
            (IS_NPC(victim) ? victim->short_descr : victim->name),
            (IS_NPC(ch) ? ch->short_descr : ch->name),
            ch->in_room->name, ch->in_room->vnum);
 
        if (IS_NPC(victim))
            wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
        else
            wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);

	raw_kill( victim, ch );
	/* dump the flags */
	if (ch != victim && !IS_NPC(ch) && (!is_same_clan(ch,victim)
	|| clan_table[victim->clan].independent))
	{
	    if (IS_SET(victim->act,PLR_TWIT))
	    	REMOVE_BIT(victim->act,PLR_TWIT);
	    if (IS_SET(victim->act,PLR_KILLER))
	    	REMOVE_BIT(victim->act,PLR_KILLER);

	}
        /* RT new auto commands */

	if ( !IS_NPC(ch) && IS_NPC(victim) )
	{
	    corpse = get_obj_list( ch, "corpse", ch->in_room->contents ); 

	    if ( IS_SET(ch->act, PLR_AUTOLOOT) &&
		 corpse && corpse->contains) /* exists and not empty */
		do_get( ch, "all corpse" );

 	    if (IS_SET(ch->act,PLR_AUTOGOLD) &&
	        corpse && corpse->contains  && /* exists and not empty */
		!IS_SET(ch->act,PLR_AUTOLOOT))
	      do_get(ch, "gold corpse");
            
	    if ( IS_SET(ch->act, PLR_AUTOSAC) )
       	{
              if ( IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains)
		return TRUE;  /* leave if corpse has treasure */
	      else
		do_sacrifice( ch, "corpse" );
        }
	}

	return TRUE;
    }

    if ( victim == ch )
	return TRUE;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return TRUE;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
	&&   victim->hit < victim->max_hit / 5) 
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait < PULSE_VIOLENCE / 2 )
	do_flee( victim, "" );

    tail_chain( );
    return TRUE;
}

bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (bloodbath)
    return FALSE;

    if ( victim->on != NULL && victim->on->pIndexData->vnum == 35)
    return TRUE;

    if ( ch->on != NULL && ch->on->pIndexData->vnum == 35)
    return TRUE;


    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (!IS_NPC(ch) && IS_IMMORTAL(ch))
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {

	/* safe room? */
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE) && !nosafe )
	{
	    send_to_char("Not in this room.\n\r",ch);
	    return TRUE;
	}
	if (victim->pIndexData->pShop != NULL)
	{
	    send_to_char("The shopkeeper wouldn't like that.\n\r",ch);
	    return TRUE;
	}

	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act,ACT_TRAIN)
	||  IS_SET(victim->act,ACT_PRACTICE)
	||  IS_SET(victim->act,ACT_IS_HEALER)
	||  IS_SET(victim->act,ACT_IS_CHANGER)
	||  IS_SET(victim->act,ACT_IS_SATAN)
	||  IS_SET(victim->act,ACT_IS_PRIEST))
	{
	    act("I don't think $G would approve.",ch,NULL,NULL,TO_CHAR);
	    return TRUE;
	}

	if (!IS_NPC(ch))
	{
	    /* no pets */
	    if (IS_SET(victim->act,ACT_PET))
	    {
		act("But $N looks so cute and cuddly...",
		    ch,NULL,victim,TO_CHAR);
		return TRUE;
	    }

	    /* no charmed creatures unless owner */
	    if (IS_AFFECTED(victim,AFF_CHARM) && ch != victim->master)
	    {
		send_to_char("You don't own that monster.\n\r",ch);
		return TRUE;
	    }
	}
    }
    /* killing players */
    else
    {
	/* NPC doing the killing */
	if (IS_NPC(ch))
	{
	    /* safe room check */
	    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE) && !nosafe)
	    {
		send_to_char("Not in this room.\n\r",ch);
		return TRUE;
	    }

	    /* charmed mobs and pets cannot attack players while owned */
	    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
	    &&  ch->master->fighting != victim)
	    {
		send_to_char("Players are your friends!\n\r",ch);
		return TRUE;
	    }
	}
	/* player doing the killing */
	else
	{
	    if (IS_SET(victim->act,PLR_TWIT) )
		return FALSE;

	    if (IS_SET(victim->act,PLR_KILLER) )
		return FALSE;

	    if (IS_SET(victim->in_room->room_flags,ROOM_ARENA))
		return FALSE;

	    if (((victim->level >19)
	    || ((victim->class >= MAX_CLASS/3)
	    && (victim->level > 14)))
	    && (is_voodood(ch, victim) ) )
		return FALSE;

            if (IS_SET(victim->in_room->room_flags,ROOM_SAFE) && !nosafe)
            {
                send_to_char("Not in this room.\n\r",ch);
                return TRUE;
            }
	    if (ch->on_quest)
	    {
		send_to_char("Not while you are on a quest.\n\r",ch);
		return TRUE;
	    }
	    if (victim->on_quest)
	    {
		send_to_char("They are on a quest, leave them alone.\n\r",ch);
		return TRUE;
	    }
	    /*if (!is_clan(ch))
	    {
		send_to_char("Join a clan if you want to fight players.\n\r",ch);
		return TRUE;
	    }

	    if (!is_pkill(ch))
	    {
		send_to_char("Your clan does not allow player fighting.\n\r",ch);
		return TRUE;
	    }

	    if (!is_clan(victim))
	    {
		send_to_char("They aren't in a clan, leave them alone.\n\r",ch);
		return TRUE;
	    }

	    if (!is_pkill(victim))
	    {
		send_to_char("They are in a no pkill clan, leave them alone.\n\r",ch);
		return TRUE;
	    } */

	    if (ch->level <= 25)
	    {
		send_to_char("Wait a while!\n\r",ch);
	    	return TRUE;
	    }

	    if (victim->level <= 25)
	    {
		send_to_char("Let them grow up a bit first!\n\r",ch);
		return TRUE;
	    }

        if (nopk)
        {
        send_to_char("There is currently no player killing.\n\r",ch);
        return TRUE;
        }

	    if (is_same_clan(ch,victim))
	    {
		send_to_char("You can't fight your own clan members.\n\r",ch);
		return TRUE;
	    }

/*	    if ( ch->pcdata->tier != victim->pcdata->tier )
	    {
        send_to_char("Pick on someone in your own tier!\n\r",ch);
        }*/
        if (ch->pcdata->tier > victim->pcdata->tier )
        {
        send_to_char("Pick on someone in a bigger tier than that!\n\r",ch);
        return TRUE;
        }

        if ((ch->pcdata->tier < victim->pcdata->tier) && (ch->level > victim->level+5) )
        {
        send_to_char("Pick on someone bigger than that!\n\r",ch);
        return TRUE;
        }


		if (ch->level > victim->level + (ch->level/5))
		{
		    send_to_char("Pick on someone your own size.\n\r",ch);
		    return TRUE;
		}
		/*if (ch->level < victim->level - 10)
		{
		    send_to_char("Pick on someone your own size.\n\r",ch);
		    return TRUE;
		}*/
	    } 
		/*else {
		send_to_char("Pick on someone in your own tier.\n\r",ch);
		return TRUE;
	    }*/

    }
    return FALSE;
}

bool is_safe_mock(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;
    if (!IS_NPC(ch) && IS_IMMORTAL(ch))
	return FALSE;
    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
    {
        send_to_char("Not in this room.\n\r",ch);
        return TRUE;
    }
    if (IS_NPC(victim))
    {
	send_to_char("{RYou can only use this on a player.{x\n\r",ch);
	return TRUE;
    }
    return FALSE;
}

bool is_voodood(CHAR_DATA *ch, CHAR_DATA *victim)
{
    OBJ_DATA *object;
    bool found;

    if ( ch->level > HERO )
	return FALSE;

    found = FALSE;
    for ( object = victim->carrying; object != NULL; object = object->next_content )
    {
	if (object->pIndexData->vnum == OBJ_VNUM_VOODOO)
	{
	    char arg[MAX_INPUT_LENGTH];

	    one_argument(object->name, arg);
	    if (!str_cmp(arg, ch->name))
	    {
		return TRUE;
	    }
	}
    }
    return FALSE;
}

bool is_safe_spell(CHAR_DATA *ch, CHAR_DATA *victim, bool area )
{
    if (bloodbath)
    return FALSE;

    if ( victim->on !=NULL &&  victim->on->pIndexData->vnum == 35)
    return TRUE;

    if ( ch->on !=NULL && ch->on->pIndexData->vnum == 35)
    return TRUE;



    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim == ch && area)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (!IS_NPC(ch) && IS_IMMORTAL(ch))
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {
	/* safe room? */
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE) && !nosafe )
	    return TRUE;

	if (victim->pIndexData->pShop != NULL)
	    return TRUE;

	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act,ACT_TRAIN)
	||  IS_SET(victim->act,ACT_PRACTICE)
	||  IS_SET(victim->act,ACT_IS_HEALER)
	||  IS_SET(victim->act,ACT_IS_CHANGER)
	||  IS_SET(victim->act,ACT_IS_SATAN)
	||  IS_SET(victim->act,ACT_IS_PRIEST))
	    return TRUE;

	if (!IS_NPC(ch))
	{
	    /* no pets */
	    if (IS_SET(victim->act,ACT_PET))
	   	return TRUE;

	    /* no charmed creatures unless owner */
	    if (IS_AFFECTED(victim,AFF_CHARM) && (area || ch != victim->master))
		return TRUE;

	    /* legal kill? -- cannot hit mob fighting non-group member */
	    if (victim->fighting != NULL && !is_same_group(ch,victim->fighting))
		return TRUE;
	}
	else
	{
	    /* area effect spells do not hit other mobs */
	    if (area && !is_same_group(victim,ch->fighting))
		return TRUE;
	}
    }
    /* killing players */
    else
    {
	if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL)
	    return TRUE;

	/* NPC doing the killing */
	if (IS_NPC(ch))
	{
	    /* charmed mobs and pets cannot attack players while owned */
	    if (((IS_AFFECTED(ch,AFF_CHARM)) & (ch->master != NULL))
	    &&  (ch->master->fighting != victim))
		return TRUE;
	
	    /* safe room? */
	    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE) && !nosafe)
		return TRUE;

	    /* legal kill? -- mobs only hit players grouped with opponent*/
	    if (ch->fighting != NULL && !is_same_group(ch->fighting,victim))
		return TRUE;
	}

	/* player doing the killing */
	else
	{
	    if (IS_SET(victim->act,PLR_TWIT) )
		return FALSE;


	    if (IS_SET(victim->act,PLR_KILLER) )
		return FALSE;


	    if (((victim->level > 19)
	    || ((victim->class >= MAX_CLASS/3)
	    && (victim->level > 14)))
	    && (is_voodood(ch, victim) ) )
		return FALSE;

	    if (!is_clan(ch))
		return TRUE;

	    if (!is_pkill(ch))
		return TRUE; 

            if (IS_SET(victim->in_room->room_flags,ROOM_SAFE) && !nosafe)
                return TRUE;

            if (ch->on_quest) 
                return TRUE; 

            if (victim->on_quest)
                return TRUE; 

	    if (!is_clan(victim))
		return TRUE;

	    if (!is_pkill(victim))
		return TRUE;  

	    if (is_same_clan(ch,victim))
		return TRUE;

	    if ( ( ( ch->class < MAX_CLASS/3 )
	    && ( victim->class < MAX_CLASS/3 ) )
	    || ( ( ch->class >= MAX_CLASS/3 )
	    && ( victim->class >= MAX_CLASS/3 ) ) )
	    {
		if (ch->level > victim->level + 10)
		{
		    return TRUE;
		}
		if (ch->level < victim->level - 10)
		{
		    return TRUE;
		}
	    } else {
		return TRUE;
	    }
	}
    }
    return FALSE;
}

/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    chance = get_skill(victim,gsn_parry) / 2;

    if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
    {
	if (IS_NPC(victim))
	    chance /= 2;
	else
	    return FALSE;
    }

    if (victim->stunned)
        return FALSE;

    if (!can_see(ch,victim))
	chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
	return FALSE;

    act( "{iYou parry $n's attack.{x",  ch, NULL, victim, TO_VICT    );
    act( "{h$N parries your attack.{x", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_parry,TRUE,6);
    return TRUE;
}

/*
 * Check for shield block.
 */
bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
        return FALSE;


    chance = get_skill(victim,gsn_shield_block) / 5 + 3;


    if ( get_eq_char( victim, WEAR_SHIELD ) == NULL )
        return FALSE;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    if (victim->stunned)
        return FALSE;

    act( "{iYou block $n's attack with your shield.{x",  ch, NULL, victim, 
TO_VICT    );
    act( "{h$N blocks your attack with a shield.{x", ch, NULL, victim, 
TO_CHAR    );
    check_improve(victim,gsn_shield_block,TRUE,6);
    return TRUE;
}


/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    chance = get_skill(victim,gsn_dodge) / 2;

    if (!can_see(victim,ch))
	chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    if (victim->stunned)
        return FALSE;

    act( "{iYou dodge $n's attack.{x", ch, NULL, victim, TO_VICT    );
    act( "{h$N dodges your attack.{x", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_dodge,TRUE,6);
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC(victim) && victim->hit < 1 )
    {
	victim->position = POS_DEAD;
	return;
    }

    if ( victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;
    ch->stunned = 0;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;
    char buf[MAX_STRING_LENGTH];

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= IS_NPC(fch) ? fch->default_pos : POS_STANDING;
	    fch->stunned	= 0;
	    update_pos( fch );
	    if (IS_SET(fch->comm,COMM_STORE))
		if(fch->tells)
		{
		    sprintf( buf, "You have {R%d{x tells waiting.\n\r", fch->tells );
		    send_to_char( buf, fch );
		    send_to_char("Type 'replay' to see tells.\n\r",fch);
		}
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch, CHAR_DATA *killer )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if ( IS_NPC(ch) )
    {
	if (IS_SET(ch->act, ACT_NO_BODY) )
	{
	    if (IS_SET(ch->act, ACT_NB_DROP) )
	    {
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
		    obj_next = obj->next_content;
		    obj_from_char( obj );
		    if (obj->item_type == ITEM_POTION)
			obj->timer = number_range(500,1000);
		    if (obj->item_type == ITEM_SCROLL)
			obj->timer = number_range(1000,2500);
		    if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH))
		    {
			obj->timer = number_range(5,10);
			REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
		    }
		    REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);

		    if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
			extract_obj( obj );
		    act("$p falls to the floor.",ch,obj,NULL,TO_ROOM);
		    obj_to_room(obj,ch->in_room);
		}
	    }
	    return;
	}
	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	corpse->timer	= number_range( 3, 6 );
	if ( ch->gold > 0 || ch->platinum > 0 )
	{
	    obj_to_obj( create_money( ch->platinum, ch->gold, ch->silver ), corpse );
	    ch->platinum = 0;
	    ch->gold = 0;
	    ch->silver = 0;
	}
	corpse->cost = 0;
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer	= number_range( 25, 40 );
	REMOVE_BIT(ch->act,PLR_CANLOOT);
	if (!is_clan(ch))
	{
	    corpse->owner = str_dup(ch->name);
	    corpse->killer = NULL;
	}
	else
	{
	    corpse->owner = str_dup(ch->name);
	    corpse->killer = str_dup(killer->name);
	    if (ch->platinum > 1 || ch->gold > 1 || ch->silver > 1)
	    {
		obj_to_obj(create_money(ch->platinum/2, ch->gold / 2, ch->silver/2), corpse);
		ch->platinum -= ch->platinum/2;
		ch->gold -= ch->gold/2;
		ch->silver -= ch->silver/2;
	    }
	}
		
	corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	bool floating = FALSE;

	obj_next = obj->next_content;
	if (obj->wear_loc == WEAR_FLOAT)
	    floating = TRUE;
	obj_from_char( obj );
	if (obj->item_type == ITEM_POTION)
	    obj->timer = number_range(500,1000);
	if (obj->item_type == ITEM_SCROLL)
	    obj->timer = number_range(1000,2500);
	if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH) && !floating)
	{
	    obj->timer = number_range(5,10);
	    REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
	}
	REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);
//Joe:Lockers	OK this determines if obj is chars's locker obj. If so drop it in room so it can be placed back on char
//		if (obj->pIndexData->vnum==LOCKER_VNUM)
//			obj_to_room(obj,ch->in_room );
// removed- the code sucks too much
	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    extract_obj( obj );
	else if (floating)
	{
	    if (IS_OBJ_STAT(obj,ITEM_ROT_DEATH)) /* get rid of it! */
	    { 
		if (obj->contains != NULL)
		{
		    OBJ_DATA *in, *in_next;

		    act("$p evaporates,scattering its contents.",
			ch,obj,NULL,TO_ROOM);
		    for (in = obj->contains; in != NULL; in = in_next)
		    {
			in_next = in->next_content;
			obj_from_obj(in);
			obj_to_room(in,ch->in_room);
		    }
		 }
		 else
		    act("$p evaporates.",
			ch,obj,NULL,TO_ROOM);
		 extract_obj(obj);
	    }
	    else
	    {
		act("$p falls to the floor.",ch,obj,NULL,TO_ROOM);
		obj_to_room(obj,ch->in_room);
	    }
	}
	else
	    obj_to_obj( obj, corpse );
    }

    obj_to_room( corpse, ch->in_room );
    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    msg = "You hear $n's death cry.";
    if (IS_NPC(ch))
    {
	if (!IS_SET(ch->act, ACT_NO_BODY) )
	{
	    switch ( number_bits(4))
	    {
	    case  0:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  1:
		msg  = "$n splatters blood on your armor.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  2:
		if (IS_SET(ch->parts,PART_GUTS))
		{
		    msg = "$n spills $s guts all over the floor.";
		    vnum = OBJ_VNUM_GUTS;
		}
		break;
	    case  3: 
		if (IS_SET(ch->parts,PART_HEAD))
		{
		    msg  = "$n's severed head plops on the ground.";
		    vnum = OBJ_VNUM_SEVERED_HEAD;
		}
		break;
	    case  4:
		if (IS_SET(ch->parts,PART_HEART))
		{
		    msg  = "$n's heart is torn from $s chest.";
		    vnum = OBJ_VNUM_TORN_HEART;
		}
		break;
	    case  5:
		if (IS_SET(ch->parts,PART_ARMS))
		{
		    msg  = "$n's arm is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_ARM;
		}
		break;
	    case  6:
		if (IS_SET(ch->parts,PART_LEGS))
		{
		    msg  = "$n's leg is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_LEG;				
		}
		break;
	    case 7:
		if (IS_SET(ch->parts,PART_BRAINS))
		{
		    msg = "$n's head is shattered, and $s brains splash all over you.";
		    vnum = OBJ_VNUM_BRAINS;
		}
		break;
	    case  8:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  9:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
	    }
	}
    } else if (ch->level > 19)
    {
	switch ( number_bits(4))
	{
	    case  0:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  1:
		msg  = "$n splatters blood on your armor.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  2:
		if (IS_SET(ch->parts,PART_GUTS))
		{
		    msg = "$n spills $s guts all over the floor.";
		    vnum = OBJ_VNUM_GUTS;
		}
		break;
	    case  3:
		if (IS_SET(ch->parts,PART_HEAD))
		{
		    msg  = "$n's severed head plops on the ground.";
		    vnum = OBJ_VNUM_SEVERED_HEAD;
		}
		break;
	    case  4:
		if (IS_SET(ch->parts,PART_HEART))
		{
		    msg  = "$n's heart is torn from $s chest.";
		    vnum = OBJ_VNUM_TORN_HEART;
		}
		break;
	    case  5:
		if (IS_SET(ch->parts,PART_ARMS))
		{
		    msg  = "$n's arm is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_ARM;
		}
		break;
	    case  6:
		if (IS_SET(ch->parts,PART_LEGS))
		{
		    msg  = "$n's leg is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_LEG;
		}
		break;
	    case 7:
		if (IS_SET(ch->parts,PART_BRAINS))
		{
		    msg = "$n's head is shattered, and $s brains splash all over you.";
		    vnum = OBJ_VNUM_BRAINS;
		}
		break;
	    case  8:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  9:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  10:
		if (IS_SET(ch->parts,PART_HEAD))
		{
		    msg  = "$n's severed head plops on the ground.";
		    vnum = OBJ_VNUM_SEVERED_HEAD;
		}
		break;
	    case  11:
		if (IS_SET(ch->parts,PART_HEART))
		{
		    msg  = "$n's heart is torn from $s chest.";
		    vnum = OBJ_VNUM_TORN_HEART;
		}
		break;
	    case  12:
		if (IS_SET(ch->parts,PART_ARMS))
		{
		    msg  = "$n's arm is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_ARM;
		}
		break;
	    case  13:
		if (IS_SET(ch->parts,PART_LEGS))
		{
		    msg  = "$n's leg is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_LEG;
		}
		break;
	    case 14:
		if (IS_SET(ch->parts,PART_BRAINS))
		{
		    msg = "$n's head is shattered, and $s brains splash all over you.";
		    vnum = OBJ_VNUM_BRAINS;
		}
	}
    }

    act( msg, ch, NULL, NULL, TO_ROOM );

    if (( vnum == 0 ) && !IS_SET(ch->act, ACT_NO_BODY) )
    {
	switch ( number_bits(4))
	{
	case  0:
		vnum = 0;
		break;
	case  1: 
		vnum = OBJ_VNUM_BLOOD;
		break;
	case  2: 							
		vnum = 0;
		break;
	case  3: 
		vnum = OBJ_VNUM_BLOOD;				
		break;
	case  4: 
		vnum = 0;				
		break;
	case  5: 
		vnum = OBJ_VNUM_BLOOD;				
		break;
	case  6: 
		vnum = 0;				
		break;
	case 7:
		vnum = OBJ_VNUM_BLOOD;
	}
    }

    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	obj->timer	= number_range( 4, 7 );
	if (!IS_NPC(ch))
	{
	    obj->timer = number_range( 12, 18 );
	}
	if (vnum == OBJ_VNUM_BLOOD)
	{
	    obj->timer = number_range( 1, 4 );
	}

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

        sprintf( buf, obj->name, name );
        free_string( obj->name );
        obj->name = str_dup( buf );

	if (obj->item_type == ITEM_FOOD)
	{
	    if (IS_SET(ch->form,FORM_POISON))
		obj->value[3] = 1;
	    else if (!IS_SET(ch->form,FORM_EDIBLE))
		obj->item_type = ITEM_TRASH;
	}

	if (IS_NPC(ch))
	{
	    obj->value[4] = 0;
	} else {
	    obj->value[4] = 1;
	}

	obj_to_room( obj, ch->in_room );
    }

    if ( IS_NPC(ch) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->u1.to_room != NULL
	&&   pexit->u1.to_room != was_in_room )
	{
	    ch->in_room = pexit->u1.to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *victim, CHAR_DATA *killer )
{
    int platgain;
    int questgain;
    int i;
    char buf[MAX_STRING_LENGTH];
//    OBJ_DATA *obj;
    death_cry( victim );
    stop_fighting( victim, TRUE );

    make_corpse( victim, killer );

//Joe:Lockers	Ok locker should have been dropped in room. Recover locker before char sent to death room
//	if ( (obj = get_obj_locker_room(victim,LOCKER_ROOM)) != NULL ) //ooops locker not in room. how did that happen?
//		obj_from_room(obj);//Get locker from room
//again, code sucks too much
    if (!IS_NPC(killer) && !IS_NPC(victim) && !IS_IMMORTAL(killer) && killer->name!=victim->name && !IS_SET(victim->in_room->room_flags,ROOM_ARENA))
    {
    	if (IS_SET(victim->act, PLR_KILLER) )
    	{
    	    send_to_char("Great Job On Killing A Killer!\n\r",killer);
            questgain=victim->level*5;
            platgain=victim->level*10;
            killer->platinum += platgain;
            killer->qps += questgain;
            sprintf(buf,"You gain {^%d{x qps and {^%d{x platinum.\n\r",questgain,platgain);
            send_to_char(buf,killer);
    	    victim->pcdata->pkdeaths ++;
    	    victim->pcdata->pkkills = 0;
    	    return;
    	}

        if (killer->attacker != TRUE)
        {
        	killer->pcdata->pkkills ++;
        }
    	victim->pcdata->pkdeaths ++;
        killer->pcdata->pktotal ++;
       	if (killer->pcdata->pkkills >= killer->pcdata->pkdeaths + 3 || victim->pcdata->pkdeaths >= victim->pcdata->pkkills + 30)
       	{
       	    do_killer(killer,"");
       	}
    }

    if ( IS_NPC(victim) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
	extract_char( victim, TRUE );
	return;
    }

    if (!IS_SET(victim->in_room->room_flags,ROOM_ARENA))
    {
    extract_char( victim, FALSE );
    while ( victim->affected )
	affect_remove( victim, victim->affected );
    victim->affected_by	= race_table[victim->race].aff;
    victim->shielded_by	= race_table[victim->race].shd;
    for (i = 0; i < 4; i++)
    	victim->armor[i]= 100;
    victim->position	= POS_RESTING;
    victim->hit		= UMAX( 1, victim->hit  );
    victim->mana	= UMAX( 1, victim->mana );
    victim->move	= UMAX( 1, victim->move );
/*  save_char_obj( victim ); we're stable enough to not need this :) */
//Joe:Lockers  Now char is in death room etc place locker back on him
//	if (obj->pIndexData->vnum==LOCKER_VNUM)
//		obj_to_char(obj,victim);
//you get the idea
    }
    else
      {
      char_from_room(victim);
      char_to_room(victim, (get_room_index(ROOM_VNUM_TEMPLE)));
      victim->position      = POS_RESTING;
      victim->hit           = victim->max_hit;
      victim->mana          = victim->max_mana;
      victim->move          = victim->max_move;
      do_look(victim, "auto");

      }
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;

        if (IS_SET(ch->act, PLR_QUESTOR)&&IS_NPC(victim))
        {
            if (ch->questmob == victim->pIndexData->vnum)
            {
                send_to_char("{MYou have almost completed your QUEST!{x\n\r",ch);
                send_to_char("Return to the questmaster before your time runs out!\n\r",ch);
                ch->questmob = -1;
            }
        }


    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( victim == ch )
	return;
    
    members = 0;
    group_levels = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
        {
	    members++;
	    group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
	}
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
	group_levels = ch->level ;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) || IS_NPC(gch))
	    continue;

/*	Taken out, add it back if you want it
	if ( gch->level - lch->level >= 5 )
	{
	    send_to_char( "You are too high for this group.\n\r", gch );
	    continue;
	}

	if ( gch->level - lch->level <= -5 )
	{
	    send_to_char( "You are too low for this group.\n\r", gch );
	    continue;
	}
*/

	xp = xp_compute( gch, victim, group_levels );  
	sprintf( buf, "{BYou receive {W%d{B experience points.{x\n\r", xp );
	send_to_char( buf, gch );
	gain_exp( gch, xp );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	    {
		act( "{cYou are {Wzapped{c by $p.{x", ch, obj, NULL, TO_CHAR );
		act( "$n is {Wzapped{x by $p.",   ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
	    }
	}
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels )
{
    int xp,base_exp;
    int align,level_range;
//    int change; (removed with align change, although align WILL affext xp)
    int time_per_level;

    level_range = victim->level - gch->level;
    if (!IS_NPC(gch))
    {
	if (gch->class >= MAX_CLASS/3)
	    level_range -= 5;
    }
 
    if (!IS_NPC(gch) && !IS_NPC(victim))
    {
	xp = victim->level*10;
	return xp;
    }
    /* compute the base exp */
    switch (level_range)
    {
 	default : 	base_exp =   0;		break;
	case -9 :	base_exp =   1;		break;
	case -8 :	base_exp =   2;		break;
	case -7 :	base_exp =   5;		break;
	case -6 : 	base_exp =   9;		break;
	case -5 :	base_exp =  11;		break;
	case -4 :	base_exp =  22;		break;
	case -3 :	base_exp =  33;		break;
	case -2 :	base_exp =  50;		break;
	case -1 :	base_exp =  66;		break;
	case  0 :	base_exp =  83;		break;
	case  1 :	base_exp =  99;		break;
	case  2 :	base_exp = 121;		break;
	case  3 :	base_exp = 143;		break;
	case  4 :	base_exp = 165;		break;
    } 
    
    if (level_range > 4)
	base_exp = 160 + 20 * (level_range - 4);

    /* do alignment computations */
    /* Following removed to whacky aligns, and it is very annoying
        to have your align constantly shifting */
    align = victim->alignment - gch->alignment;
    /*
    if (IS_SET(victim->act,ACT_NOALIGN))
    {
	// No change
    }

    else if (align > 500) // monster is more good than slayer
    {
	change = (align - 500) * base_exp / 500 * gch->level/total_levels; 
	change = UMAX(1,change);
        gch->alignment = UMAX(-1000,gch->alignment - change);
	if ( gch->pet != NULL )
	    gch->pet->alignment = gch->alignment;
    }

    else if (align < -500) // monster is more evil than slayer
    {
	change =  ( -1 * align - 500) * base_exp/500 * gch->level/total_levels;
	change = UMAX(1,change);
	gch->alignment = UMIN(1000,gch->alignment + change);
        if ( gch->pet != NULL )
            gch->pet->alignment = gch->alignment;
    }

    else // improve this someday
    {
	change =  gch->alignment * base_exp/500 * gch->level/total_levels;  
	gch->alignment -= change;
        if ( gch->pet != NULL )
            gch->pet->alignment = gch->alignment;
    }
    */
    /* calculate exp multiplier */
    if (IS_SET(victim->act,ACT_NOALIGN))
	xp = base_exp;

    else if (gch->alignment > 500)  /* for goodie two shoes */
    {
	if (victim->alignment < -750)
	    xp = (base_exp *4)/3;
   
 	else if (victim->alignment < -500)
	    xp = (base_exp * 5)/4;

        else if (victim->alignment < -250)
	    xp = (base_exp * 3)/4; 

        else if (victim->alignment > 750)
	    xp = base_exp / 4;

   	else if (victim->alignment > 500)
	    xp = base_exp / 2;

	else
	    xp = base_exp;
    }

    else if (gch->alignment < -500) /* for baddies */
    {
	if (victim->alignment > 750)
	    xp = (base_exp * 5)/4;
	
  	else if (victim->alignment > 500)
	    xp = (base_exp * 11)/10; 

   	else if (victim->alignment < -750)
	    xp = base_exp/2;

	else if (victim->alignment < -500)
	    xp = (base_exp * 3)/4;

	else if (victim->alignment < -250)
	    xp = (base_exp * 9)/10;

	else
	    xp = base_exp;
    }

    else if (gch->alignment > 200)  /* a little good */
    {

	if (victim->alignment < -500)
	    xp = (base_exp * 6)/5;

 	else if (victim->alignment > 750)
	    xp = base_exp/2;

	else if (victim->alignment > 0)
	    xp = (base_exp * 3)/4; 
	
	else
	    xp = base_exp;
    }

    else if (gch->alignment < -200) /* a little bad */
    {
	if (victim->alignment > 500)
	    xp = (base_exp * 6)/5;
 
	else if (victim->alignment < -750)
	    xp = base_exp/2;

	else if (victim->alignment < 0)
	    xp = (base_exp * 3)/4;

	else
	    xp = base_exp;
    }

    else /* neutral */
    {

	if (victim->alignment > 500 || victim->alignment < -500)
	    xp = (base_exp * 4)/3;

	else if (victim->alignment < 200 && victim->alignment > -200)
	    xp = base_exp/2;

 	else
	    xp = base_exp;
    }
    //New exp code for "smoother" exping
    xp= 15*xp/gch->level;


    /* more exp at the low levels
    if (gch->level < 11)
    	xp = 15 * xp / (gch->level + 4);

     less at high
       LEVEL 61!!!!!!- And I thought it was a bug...well shows how
         often I actually look at the coding... *Lowered for 3rd Tier*
    if (gch->level > 60 )
	xp =  15 * xp / (gch->level - 15 );  Formerly 25, change back
                                         if you want (the GCH one) */

    /* reduce for playing time */

    
    {
	/* compute quarter-hours per level */
	time_per_level = 4 *
			 (gch->played + (int) (current_time - gch->logon))/3600
			 / gch->level;

	time_per_level = URANGE(2,time_per_level,12);
	if (gch->level < 25)  /* make it a curve */
	    time_per_level = UMAX(time_per_level,(25 - gch->level));
/*
 *	xp = xp * time_per_level / 12;
 *      More exp for slower levels?? How bootleg.. (Removed in RoT original code)
 */
    }
    xp = xp*.75;
   
    /* randomize the rewards */
    xp = number_range (xp * 3/4, xp * 5/4);

    /* adjust for grouping */
    xp = xp * gch->level/( UMAX(1,total_levels -1) );

    if (xp>=5000)
    xp=5000;

    if (double_exp)
    xp *= 2;

    return xp;
}


void dam_message( CHAR_DATA *ch, CHAR_DATA *victim,int dam,int dt,bool immune )
{
    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;

    if (ch == NULL || victim == NULL)
	return;

	  if ( dam ==   0 ) { vs = "{$miss{0";	vp = "{$misses{0";		}
    else if ( dam <=   6 ) { vs = "{1scr{!a{1tch{0";	vp = "{1scr{!atc{1hes{0";	}
    else if ( dam <=  10 ) { vs = "{2g{@r{^a{@z{2e{0";	vp = "{2g{@r{^az{@e{2s{0";		}
    else if ( dam <=  15 ) { vs = "{5h{Mi{5t{0";	vp = "{5h{Mit{5s{0";		}
    else if ( dam <=  20 ) { vs = "{*i{7n{&j{7u{*r{7e{0";	vp = "{*i{7n{&j{7u{*r{7e{&s{0";		}
    else if ( dam <=  35 ) { vs = "{4w{$o{5u{$n{4d{0";	vp = "{4w{$o{5un{$d{4s{0";		}
    else if ( dam <=  50 ) { vs = "{3m{#au{3l{0";       vp = "{3m{#a{^u{#l{3s{0";		}
    else if ( dam <=  65 ) { vs = "{^D{&e{7c{&i{7m{&a{7t{^E{0";	vp = "{^D{&e{7c{&i{7m{&a{7t{&e{^S{0";	}
    else if ( dam <=  90 ) { vs = "{^s{$m{Mi{$t{^e{0";	vp = "{^s{$m{Mit{$e{^s{)";	}
    else if ( dam <=  120 ) { vs = "{1m{!ai{1m{0";	vp = "{1m{!ai{1ms{0";		}
    else if ( dam <=  150 ) { vs = "{^:I:{$M{6U{$T{6I{$L{6A{$T{6E{^:I:{0";
                             vp = "{^:I:{$M{6U{$T{6I{$L{6A{$T{6E{$S{^:I:{0";	}
    else if ( dam <=  200 ) { vs = "{1+{!X{1+{MD{^I{MS{^E{MM{^B{MO{^W{ME{^L{1+{!X{1+{0";
                             vp = "{1+{!X{1+{MD{^I{MS{^E{MM{^B{MO{^W{ME{^L{MS{1+{!X{1+{0";	}
    else if ( dam <=  250 ) { vs = "{7--{&>{1D{!I{5S{1M{!E{5M{1B{!E{5R{&<{7--{0";
                             vp = "{7--{&>{1D{!I{5S{1M{!E{5M{1B{!E{5R{1S{&<{7--{0";	}
    else if ( dam <=  300 ) { vs = "{!>{$X{!<>{$X{!<{^M{#AS{MSA{#CR{^E{!>{$X{!<>{$X{!<{0";	
                             vp = "{!>{$X{!<>{$X{!<{^M{#AS{MSA{#CR{^ES{!>{$X{!<>{$X{!<{0";	}
    else if ( dam <=  400 ) { vs = "{&){7>{*-{#M{3A{#N{3G{#L{3E{*-{7<{&({0";	
                              vp = "{&){7>{*-{#M{3A{#N{3G{#L{3E{#S{*-{7<{&({0";		}
    else if ( dam <=  550 ) { vs = "{**** {7SA{&U{7TE {****{0";
			     vp = "{**** {7SA{&UT{7ES {****{0";			}
    else if ( dam <=  700 ) { vs = "{#<{3*{#> {3D{7E{&V{7A{3S{#T{3A{7T{&E {#<{3*{#>{0";
			     vp = "{#<{3*{#> {3D{7E{&V{7A{3S{#T{3A{7T{&E{7S {#<{3*{#>{0";			}
    else if ( dam <= 800)  { vs = "{1={!={1= {^O{6B{^L{6I{^T{6E{^R{6A{^T{6E {1={!={1={0";
			     vp = "{1={!={1= {^O{6B{^L{6I{^T{6E{^R{6A{^T{6E{^S {1={!={1={0";		}
    else if ( dam <= 900)  { vs = "{!-{@<{#({$A{^N{!N{@I{#H{$I{^L{!A{@T{#E{#){@>{!-{0";
			     vp = "{!-{@<{#({$A{^N{!N{@I{#H{$I{^L{!A{@T{#E{$S{#){@>{!-{0";		}
    else if ( dam <= 1000)  { vs = "{6<>{^-{6<>{1E{!R{2A{@D{1I{!C{2A{@T{1E{6<>{^-{6<>{0";
			     vp = "{6<>{^-{6<>{1E{!R{2A{@D{1I{!C{2A{@T{1E{!S{6<>{^-{6<>{0";			}
    else if ( dam <= 1750)  { vs = "{2do {$U{^N{4S{6P{$E{^A{4K{6A{$B{^L{4E {#things {2to{0";
			     vp = "{2does {$U{^N{4S{6P{$E{^A{4K{6A{$B{^L{4E {#things {2to{0";		}
    else if ( dam <= 15000) { vs = "do {@-({zX{0{@)- {!GODLY DAMAGE{@ -({zX{0{@)-{0 to";
			     vp = "does {@-({zX{0{@)- {!GODLY DAMAGE{@ -({zX{0{@)-{0 to";		}
    else if ( dam <= 1000000){ vs = "do {@-({z{#X{0{@)- {$TRULY {#GODLY {$DAMAGE{@ -({z{#X{0{@)-{0 to";
			     vp = "does {@-({z{#X{0{@)- {$TRULY {#GODLY {$DAMAGE{@ -({z{#X{0{@)-{0 to";		}
    else		    { vs = "do {z{%MUD-CRASHING DAMAGE{0 to";
                             vp = "does {z{%MUD-CRASHING DAMAGE{0 to";  }

    punct   = (dam <= 4) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
	if (ch  == victim)
	{
	    sprintf( buf1, "{k$n %s $melf%c {^({$%d{^){x",vp,punct, dam );
	    sprintf( buf2, "{hYou %s yourself%c {#>{^%d{#<{x",vs,punct, dam );
	}
	else
	{
	    sprintf( buf1, "{k$n %s $N%c {^({$%d{^){x",  vp, punct, dam );
	    sprintf( buf2, "{hYou %s $N%c {#[{^%d{#]{x", vs, punct, dam );
	    sprintf( buf3, "{i$n %s you%c {$>{!%d{$<{x", vp, punct, dam );
	}
    }
    else
    {
	if ( dt >= 0 && dt < MAX_SKILL )
	    attack	= skill_table[dt].noun_damage;
	else if ( dt >= TYPE_HIT
	&& dt < TYPE_HIT + MAX_DAMAGE_MESSAGE) 
	    attack	= attack_table[dt - TYPE_HIT].noun;
	else
	{
	    bug( "Dam_message: bad dt %d.", dt );
	    dt  = TYPE_HIT;
	    attack  = attack_table[0].name;
	}

	if (immune)
	{
	    if (ch == victim)
	    {
		sprintf(buf1,"{k$n is unaffected by $s own %s.{x",attack);
		sprintf(buf2,"{hLuckily, you are immune to that.{x");
	    } 
	    else
	    {
	    	sprintf(buf1,"{k$N is unaffected by $n's %s!{x",attack);
	    	sprintf(buf2,"{h$N is unaffected by your %s!{x",attack);
	    	sprintf(buf3,"{i$n's %s is powerless against you.{x",attack);
	    }
	}
	else
	{
	    if (ch == victim)
	    {
		sprintf( buf1, "{k$n's %s %s $m%c {^({$%d{^){x",attack,vp,punct,dam);
		sprintf( buf2, "{hYour %s %s you%c {#>{^%d{#<{0",attack,vp,punct,dam);
	    }
	    else
	    {
	    	sprintf( buf1, "{k$n's %s %s $N%c {^({$%d{^){x",  attack, vp, punct, dam );
	    	sprintf( buf2, "{hYour %s %s $N%c {#[{^%d{#]{x",  attack, vp, punct, dam );
	    	sprintf( buf3, "{i$n's %s %s you%c {$>{!%d{$<{x", attack, vp, punct, dam );
	    }
	}
    }

    if (ch == victim)
    {
	act(buf1,ch,NULL,NULL,TO_ROOM);
	act(buf2,ch,NULL,NULL,TO_CHAR);
    }
    else
    {
    	act( buf1, ch, NULL, victim, TO_NOTVICT );
    	act( buf2, ch, NULL, victim, TO_CHAR );
    	act( buf3, ch, NULL, victim, TO_VICT );
    }

    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    OBJ_DATA *second;
    bool secondtf;

    secondtf = FALSE;

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
        send_to_char("They aren't using a weapon!\n\r",ch);
	return;
    }

    if ( ( get_eq_char(victim,WEAR_SECONDARY)) != NULL )
        secondtf = TRUE;


    if ( IS_OBJ_STAT(obj,ITEM_NOREMOVE))
    {
	act("{j$S weapon won't budge!{x",ch,NULL,victim,TO_CHAR);
	act("{j$n tries to disarm you, but your weapon won't budge!{x",
	    ch,NULL,victim,TO_VICT);
	act("{k$n tries to disarm $N, but fails.{x",ch,NULL,victim,TO_NOTVICT);
	return;
    }

    act( "{j$n DISARMS you and sends your weapon flying!{x", 
	 ch, NULL, victim, TO_VICT    );
    act( "{jYou disarm $N!{x",  ch, NULL, victim, TO_CHAR    );
    act( "{k$n disarms $N!{x",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_OBJ_STAT(obj,ITEM_NODROP) || IS_OBJ_STAT(obj,ITEM_INVENTORY) )
	obj_to_char( obj, victim );
    else
    {
	obj_to_room( obj, victim->in_room );
	if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
	    get_obj(victim,obj,NULL);
    }
     if ( secondtf == TRUE && ((second = get_eq_char(victim,WEAR_SECONDARY))->item_type != ITEM_QUIVER))
    {
        unequip_char( victim, second );
        equip_char( victim, second, WEAR_WIELD );
        send_to_char("Your secondary weapon is now your primary weapon.\n\r",victim);
    }
    return;
}

void do_berserk( CHAR_DATA *ch, char *argument)
{
    int chance, hp_percent;

    if ((chance = get_skill(ch,gsn_berserk)) == 0
    ||  (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BERSERK))
    ||  (!IS_NPC(ch)
    &&   ch->level < skill_table[gsn_berserk].skill_level[ch->class]))
    {
	send_to_char("{hYou turn {rred{h in the face, but nothing happens.{x\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_BERSERK) || is_affected(ch,gsn_berserk)
    ||  is_affected(ch,skill_lookup("frenzy")))
    {
	send_to_char("{hYou get a little madder.{x\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_CALM))
    {
	send_to_char("{hYou're feeling too mellow to berserk.{x\n\r",ch);
	return;
    }

    if (ch->mana < 50)
    {
	send_to_char("{hYou can't get up enough energy.{x\n\r",ch);
	return;
    }

    /* modifiers */

    /* fighting */
    if (ch->position == POS_FIGHTING)
	chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit/ch->max_hit;
    chance += 25 - hp_percent/2;

    if (number_percent() < chance)
    {
	AFFECT_DATA af;

	WAIT_STATE(ch,PULSE_VIOLENCE);
	ch->mana -= 50;
	ch->move /= 2;

	/* heal a little damage */
	ch->hit += ch->level * 2;
	ch->hit = UMIN(ch->hit,ch->max_hit);

	send_to_char("{hYour pulse races as you are consumed by {rrage!{x\n\r",ch);
	act("{k$n gets a {cw{gi{rl{yd{k look in $s eyes.{x",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_berserk,TRUE,2);

	af.where	= TO_AFFECTS;
	af.type		= gsn_berserk;
	af.level	= ch->level;
	af.duration	= number_fuzzy(ch->level / 8);
	af.modifier	= UMAX(1,ch->level/5);
	af.bitvector 	= AFF_BERSERK;

	af.location	= APPLY_HITROLL;
	affect_to_char(ch,&af);

	af.location	= APPLY_DAMROLL;
	affect_to_char(ch,&af);

	af.modifier	= UMAX(10,10 * (ch->level/5));
	af.location	= APPLY_AC;
	affect_to_char(ch,&af);
    }

    else
    {
	WAIT_STATE(ch,3 * PULSE_VIOLENCE);
	ch->mana -= 25;
	ch->move /= 2;

	send_to_char("{hYour pulse speeds up, but nothing happens.{x\n\r",ch);
	check_improve(ch,gsn_berserk,FALSE,2);
    }
}

void do_voodoo( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *doll;
 
    if (IS_NPC(ch))
        return;
 
    doll = get_eq_char(ch,WEAR_HOLD);
    if  (doll == NULL || (doll->pIndexData->vnum != OBJ_VNUM_VOODOO))
    {
        send_to_char("You are not holding a voodoo doll.\n\r",ch);
        return;
    }

    one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char("Syntax: voodoo <action>\n\r",ch);
	send_to_char("Actions: pin trip throw\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"pin"))
    {
	do_vdpi(ch,doll->name);
	return;
    }

    if (!str_cmp(arg,"trip"))
    {
	do_vdtr(ch,doll->name);
	return;
    }

    if (!str_cmp(arg,"throw"))
    {
	do_vdth(ch,doll->name);
	return;
    }

    do_voodoo(ch,"");
}

void do_vdpi( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    bool found = FALSE;

    argument = one_argument( argument, arg1 );

    for (d = descriptor_list; d != NULL; d = d->next)
    {
	CHAR_DATA *wch;

	if (d->connected != CON_PLAYING || !can_see(ch,d->character))
	    continue;

	wch = ( d->original != NULL ) ? d->original : d->character;

	if (!can_see(ch,wch))
	    continue;

	if (!str_cmp(arg1,wch->name) && !found)
	{
	    if (IS_NPC(wch))
		continue;

	    if (IS_IMMORTAL(wch) && (wch->level > ch->level))
	    {
		send_to_char( "That's not a good idea.\n\r", ch);
		return;
	    }

	    if ((wch->level < 20) && !IS_IMMORTAL(ch))
	    {
		send_to_char( "They are a little too young for that.\n\r",ch);
		return;
	    }

	    if (IS_SHIELDED(wch,SHD_PROTECT_VOODOO))
	    {
		send_to_char( "They are still realing from a previous voodoo.\n\r",ch);
		return;
	    }

	    found = TRUE;

	    send_to_char( "You stick a pin into your voodoo doll.\n\r",ch);
	    act( "$n sticks a pin into a voodoo doll.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "{RYou double over with a sudden pain in your gut!{x\n\r",wch);
	    act( "$n suddenly doubles over with a look of extreme pain!",wch, NULL,NULL,TO_ROOM);
	    af.where      = TO_SHIELDS;
	    af.type       = skill_lookup("protection voodoo");
	    af.level      = wch->level;
	    af.duration   = 1;
	    af.location   = APPLY_NONE;
	    af.modifier   = 0;
	    af.bitvector  = SHD_PROTECT_VOODOO;
	    affect_to_char(wch,&af);
	    return;
	}
    }
    send_to_char("Your victim doesn't seem to be in the realm.\n\r",ch);
    return;
}

void do_vdtr( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    bool found = FALSE; 
 
    argument = one_argument( argument, arg1 );
 
    for (d = descriptor_list; d != NULL; d = d->next)
    { 
	CHAR_DATA *wch; 
 
	if (d->connected != CON_PLAYING || !can_see(ch,d->character)) 
	    continue; 
 
	wch = ( d->original != NULL ) ? d->original : d->character; 
 
	if (!can_see(ch,wch)) 
	    continue; 
 
	if (!str_cmp(arg1,wch->name) && !found) 
	{ 
	    if (IS_NPC(wch)) 
		continue; 
 
	    if (IS_IMMORTAL(wch) && (wch->level > ch->level)) 
	    {         
		send_to_char( "That's not a good idea.\n\r", ch); 
		return; 
	    } 
 
	    if ((wch->level < 20) && !IS_IMMORTAL(ch))
	    {
		send_to_char( "They are a little too young for that.\n\r",ch);
		return;
	    }

	    if (IS_SHIELDED(wch,SHD_PROTECT_VOODOO))
	    {
                send_to_char( "They are still realing from a previous voodoo.\n\r",ch);
		return;
	    }

	    found = TRUE;
 
	    send_to_char( "You slam your voodoo doll against the ground.\n\r",ch); 
	    act( "$n slams a voodoo doll against the ground.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "{RYour feet slide out from under you!{x\n\r",wch);
	    send_to_char( "{RYou hit the ground face first!{x\n\r",wch);
	    act( "$n trips over $s own feet, and does a nose dive into the ground!",wch, NULL,NULL,TO_ROOM);
            af.where      = TO_SHIELDS;
            af.type       = skill_lookup("protection voodoo");
            af.level      = wch->level;
            af.duration   = 1;
            af.location   = APPLY_NONE;
            af.modifier   = 0;
            af.bitvector  = SHD_PROTECT_VOODOO;
            affect_to_char(wch,&af);
	    return;
	}
    }
    send_to_char("Your victim doesn't seem to be in the realm.\n\r",ch);
    return;
}

void do_vdth( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    bool found = FALSE; 
    int attempt;
 
    argument = one_argument( argument, arg1 );
 
    for (d = descriptor_list; d != NULL; d = d->next)
    { 
	CHAR_DATA *wch; 
 
	if (d->connected != CON_PLAYING || !can_see(ch,d->character)) 
	    continue; 
 
	wch = ( d->original != NULL ) ? d->original : d->character; 
 
	if (!can_see(ch,wch)) 
	    continue; 
 
	if (!str_cmp(arg1,wch->name) && !found) 
	{ 
	    if (IS_NPC(wch)) 
		continue; 
 
	    if (IS_IMMORTAL(wch) && (wch->level > ch->level)) 
	    {         
		send_to_char( "That's not a good idea.\n\r", ch); 
		return; 
	    } 
 
	    if ((wch->level < 20) && !IS_IMMORTAL(ch))
	    {
		send_to_char("They are a little too young for that.\n\r",ch);
		return;
	    }

	    if (IS_SHIELDED(wch,SHD_PROTECT_VOODOO))
	    {
                send_to_char( "They are still realing from a previous voodoo.\n\r",ch);
		return;
	    }

	    found = TRUE;
 
	    send_to_char( "You toss your voodoo doll into the air.\n\r",ch); 
	    act( "$n tosses a voodoo doll into the air.", ch, NULL, NULL, TO_ROOM );
            af.where      = TO_SHIELDS;
            af.type       = skill_lookup("protection voodoo");
            af.level      = wch->level;
            af.duration   = 1;
            af.location   = APPLY_NONE;
            af.modifier   = 0;
            af.bitvector  = SHD_PROTECT_VOODOO;
            affect_to_char(wch,&af);
	    if ((wch->fighting != NULL) || (number_percent() < 25))
	    {
		send_to_char( "{RA sudden gust of wind throws you through the air!{x\n\r",wch);
		send_to_char( "{RYou slam face first into the nearest wall!{x\n\r",wch);
		act( "A sudden gust of wind picks up $n and throws $m into a wall!",wch, NULL,NULL,TO_ROOM);
		return;
	    }
	    wch->position = POS_STANDING;
	    was_in = wch->in_room;
	    for ( attempt = 0; attempt < 6; attempt++ )
	    {
		EXIT_DATA *pexit;
		int door;

		door = number_door( );
		if ( ( pexit = was_in->exit[door] ) == 0
		||   pexit->u1.to_room == NULL
		||   IS_SET(pexit->exit_info, EX_CLOSED)
		|| ( IS_NPC(wch)
		&&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
		    continue;

		move_char( wch, door, FALSE, TRUE );
		if ( ( now_in = wch->in_room ) == was_in )
		    continue;

		wch->in_room = was_in;
		sprintf(buf, "A sudden gust of wind picks up $n and throws $m to the %s.", dir_name[door]);
		act( buf, wch, NULL, NULL, TO_ROOM );
		send_to_char( "{RA sudden gust of wind throws you through the air!{x\n\r",wch);
		wch->in_room = now_in;
		act( "$n sails into the room and slams face first into a wall!",wch,NULL,NULL,TO_ROOM);
		do_look( wch, "auto" );
		send_to_char( "{RYou slam face first into the nearest wall!{x\n\r",wch);
		return;
	    }
	    send_to_char( "{RA sudden gust of wind throws you through the air!{x\n\r",wch);
            send_to_char( "{RYou slam face first into the nearest wall!{x\n\r",wch);
            act( "A sudden gust of wind picks up $n and throws $m into a wall!",wch, NULL,NULL,TO_ROOM);
            return;
	}
    }
    send_to_char("Your victim doesn't seem to be in the realm.\n\r",ch);
    return;
}

void do_bash( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);
 
    if ( (chance = get_skill(ch,gsn_bash)) == 0
    ||	 (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BASH))
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_bash].skill_level[ch->class]))
    {	
	send_to_char("Bashing? What's that?\n\r",ch);
	return;
    }
 
    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't fighting anyone!\n\r",ch);
	    return;
	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (victim->position < POS_FIGHTING)
    {
	act("You'll have to let $M get back up first.",ch,NULL,victim,TO_CHAR);
	return;
    } 

    if (victim == ch)
    {
	send_to_char("You try to bash your brains out, but fail.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if ( IS_NPC(victim) && 
	victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is your friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( !can_see( ch, victim ) )
    {
	send_to_char("You get a running start, and slam right into a wall.\n\r",ch);
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;

    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 15;
    else
	chance += (ch->size - victim->size) * 10; 


    /* stats */
    chance += get_curr_stat(ch,STAT_STR);
    chance -= (get_curr_stat(victim,STAT_DEX) * 4)/3;
    chance -= GET_AC(victim,DEFENCE) /25;
    /* speed */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
        chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
        chance -= 30;

    /* level */
    chance += (ch->level - victim->level);

    if (!IS_NPC(victim) 
	&& chance < get_skill(victim,gsn_dodge) )
    {	/*
        act("{i$n tries to bash you, but you dodge it.{x",ch,NULL,victim,TO_VICT);
        act("{h$N dodges your bash, you fall flat on your face.{x",ch,NULL,victim,TO_CHAR);
        WAIT_STATE(ch,skill_table[gsn_bash].beats);
        return;*/
	chance -= 3 * (get_skill(victim,gsn_dodge) - chance);
    }

    /* now the attack */
    if (number_percent() < chance )
    {
    
	act("{i$n sends you sprawling with a powerful bash!{x",
		ch,NULL,victim,TO_VICT);
	act("{hYou slam into $N, and send $M flying!{x",ch,NULL,victim,TO_CHAR);
	act("{k$n sends $N sprawling with a powerful bash.{x",
		ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_bash,TRUE,1);

	DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
	WAIT_STATE(ch,skill_table[gsn_bash].beats);
	victim->position = POS_RESTING;
	damage(ch,victim,number_range(2,2 + 2 * ch->size + chance/20),gsn_bash,
	    DAM_BASH,FALSE);
	chance = (get_skill(ch,gsn_stun)/5);
	if (number_percent() < chance )
	{
	    chance = (get_skill(ch,gsn_stun)/5);
	    if (number_percent() < chance )
	    {
		victim->stunned = 2;
	    } else {
		victim->stunned = 1;
	    }
	    act("{iYou are stunned, and have trouble getting back up!{x",
		ch,NULL,victim,TO_VICT);
	    act("{h$N is stunned by your bash!{x",ch,NULL,victim,TO_CHAR);
	    act("{k$N is having trouble getting back up.{x",
		ch,NULL,victim,TO_NOTVICT);
	    check_improve(ch,gsn_stun,TRUE,1);
	}
    }
    else
    {
	damage(ch,victim,0,gsn_bash,DAM_BASH,FALSE);
	act("{hYou fall flat on your face!{x",
	    ch,NULL,victim,TO_CHAR);
	act("{k$n falls flat on $s face.{x",
	    ch,NULL,victim,TO_NOTVICT);
	act("{iYou evade $n's bash, causing $m to fall flat on $s face.{x",
	    ch,NULL,victim,TO_VICT);
	check_improve(ch,gsn_bash,FALSE,1);
	ch->position = POS_RESTING;
	WAIT_STATE(ch,skill_table[gsn_bash].beats * 3/2); 
    }
}

void do_dirt( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_dirt)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK_DIRT))
    ||   (!IS_NPC(ch)
    &&    ch->level < skill_table[gsn_dirt].skill_level[ch->class]))
    {
	send_to_char("{hYou get your feet dirty.{x\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't in combat!\n\r",ch);
	    return;
	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(victim,AFF_BLIND))
    {
	act("{h$E's already been blinded.{x",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("Very funny.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_DEX);

    /* speed  */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;

    /* terrain */

    switch(ch->in_room->sector_type)
    {
	case(SECT_INSIDE):		chance -= 20;	break;
	case(SECT_CITY):		chance -= 10;	break;
	case(SECT_FIELD):		chance +=  5;	break;
	case(SECT_FOREST):				break;
	case(SECT_HILLS):				break;
	case(SECT_MOUNTAIN):		chance -= 10;	break;
	case(SECT_WATER_SWIM):		chance  =  0;	break;
	case(SECT_WATER_NOSWIM):	chance  =  0;	break;
	case(SECT_AIR):			chance  =  0;  	break;
	case(SECT_DESERT):		chance += 10;   break;
    }

    if (chance == 0)
    {
	send_to_char("{hThere isn't any dirt to kick.{x\n\r",ch);
	return;
    }

    /* now the attack */
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	act("{k$n is blinded by the dirt in $s eyes!{x",victim,NULL,NULL,TO_ROOM);
	act("{i$n kicks dirt in your eyes!{x",ch,NULL,victim,TO_VICT);
        damage(ch,victim,number_range(2,5),gsn_dirt,DAM_NONE,FALSE);
	send_to_char("{DYou can't see a thing!{x\n\r",victim);
	check_improve(ch,gsn_dirt,TRUE,2);
	WAIT_STATE(ch,skill_table[gsn_dirt].beats);

	af.where	= TO_AFFECTS;
	af.type 	= gsn_dirt;
	af.level 	= ch->level;
	af.duration	= 0;
	af.location	= APPLY_HITROLL;
	af.modifier	= -4;
	af.bitvector 	= AFF_BLIND;

	affect_to_char(victim,&af);
    }
    else
    {
	damage(ch,victim,0,gsn_dirt,DAM_NONE,TRUE);
	check_improve(ch,gsn_dirt,FALSE,2);
	WAIT_STATE(ch,skill_table[gsn_dirt].beats);
    }
}

void do_gouge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_gouge)) == 0
    ||   (!IS_NPC(ch)
    &&    ch->level < skill_table[gsn_gouge].skill_level[ch->class]))
    {
	send_to_char("Gouge?  What's that?{x\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't in combat!\n\r",ch);
	    return;
	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(victim,AFF_BLIND))
    {
	act("{h$E's already been blinded.{x",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("Very funny.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_DEX);

    /* speed  */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;

    /* now the attack */
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	act("{k$n is blinded by a poke in the eyes!{x",victim,NULL,NULL,TO_ROOM);
	act("{i$n gouges at your eyes!{x",ch,NULL,victim,TO_VICT);
        damage(ch,victim,number_range(2,5),gsn_gouge,DAM_NONE,FALSE);
	send_to_char("{DYou see nothing but stars!{x\n\r",victim);
	check_improve(ch,gsn_gouge,TRUE,2);
	WAIT_STATE(ch,skill_table[gsn_gouge].beats);

	af.where	= TO_AFFECTS;
	af.type 	= gsn_gouge;
	af.level 	= ch->level;
	af.duration	= 0;
	af.location	= APPLY_HITROLL;
	af.modifier	= -4;
	af.bitvector 	= AFF_BLIND;

	affect_to_char(victim,&af);
    }
    else
    {
	damage(ch,victim,0,gsn_gouge,DAM_NONE,TRUE);
	check_improve(ch,gsn_gouge,FALSE,2);
	WAIT_STATE(ch,skill_table[gsn_gouge].beats);
    }
}

void do_trip( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_trip)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TRIP))
    ||   (!IS_NPC(ch) 
	  && ch->level < skill_table[gsn_trip].skill_level[ch->class]))
    {
	send_to_char("Tripping?  What's that?\n\r",ch);
	return;
    }


    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't fighting anyone!\n\r",ch);
	    return;
 	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
	send_to_char("Kill stealing is not permitted.\n\r",ch);
	return;
    }
    
    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(victim,AFF_FLYING))
    {
	act("{h$S feet aren't on the ground.{x",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim->position < POS_FIGHTING)
    {
	act("{h$N is already down.{c",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("{hYou fall flat on your face!{x\n\r",ch);
	WAIT_STATE(ch,2 * skill_table[gsn_trip].beats);
	act("{k$n trips over $s own feet!{x",ch,NULL,NULL,TO_ROOM);
	return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("$N is your beloved master.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    /* modifiers */

    /* size */
    if (ch->size < victim->size)
        chance += (ch->size - victim->size) * 10;  /* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= get_curr_stat(victim,STAT_DEX) * 3 / 2;

    /* speed */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 20;

    /* level */
    chance += (ch->level - victim->level) * 2;


    /* now the attack */
    if (number_percent() < chance)
    {
	act("{i$n trips you and you go down!{x",ch,NULL,victim,TO_VICT);
	act("{hYou trip $N and $N goes down!{x",ch,NULL,victim,TO_CHAR);
	act("{k$n trips $N, sending $M to the ground.{x",ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_trip,TRUE,1);

	DAZE_STATE(victim,2 * PULSE_VIOLENCE);
        WAIT_STATE(ch,skill_table[gsn_trip].beats);
	victim->position = POS_RESTING;
	damage(ch,victim,number_range(2, 2 +  2 * victim->size),gsn_trip,
	    DAM_BASH,TRUE);
    }
    else
    {
	damage(ch,victim,0,gsn_trip,DAM_BASH,TRUE);
	WAIT_STATE(ch,skill_table[gsn_trip].beats*2/3);
	check_improve(ch,gsn_trip,FALSE,1);
    } 
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wield;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "{hYou hit yourself.  {z{COuch!{x\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( (wield = get_eq_char(ch, WEAR_WIELD))== NULL && IS_IMMORTAL(ch))
    {
    ROOM_INDEX_DATA *pRoomIndex;

    send_to_char("Trying to crash the MUD, eh? You need to die!\n\r",ch);
    /* Hell, lets have RotK wizslap them!! */
    pRoomIndex = get_random_room(ch);
    act( "Realm of the Keepers slaps you, sending you reeling through time and space!", ch, NULL, victim, TO_CHAR);
    act( "Realm of the Keepers slaps $N, sending $M reeling through time and space!", ch, NULL, victim, TO_NOTVICT );
    char_from_room( ch );
    char_to_room( ch, pRoomIndex );
    act( "$n crashes to the ground!", victim, NULL, NULL, TO_ROOM );
    do_look(ch, "auto");
    return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( !IS_NPC(victim) )
    {
        if ( !IS_SET(victim->act, PLR_TWIT) )
        {
            send_to_char( "You must MURDER a player.\n\r", ch );
            return;
        }
        if ( !IS_SET(victim->act, PLR_KILLER) )
        {
            send_to_char( "You must MURDER a player.\n\r", ch );
            return;
        }
    }
    if ( victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

void do_mock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mock hit whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( is_safe_mock( ch, victim ) )
	return;

    if ( victim->fighting != NULL)
    {
        send_to_char("{gThis player is busy at the moment.{x\n\r",ch);
        return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "{gYou've already got your hands full!{x\n\r", ch );
	return;
    }

    one_hit_mock( ch, victim, TYPE_UNDEFINED, FALSE );

    return;
}

void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to {RMURDER{x, spell it out.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if (IS_NPC(ch))
	return;

    if (IS_AFFECTED(ch,AFF_CHARM))
	return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    if (IS_NPC(ch))
	sprintf(buf, "Help! I am being attacked by %s!",ch->short_descr);
    else
    	sprintf( buf, "Help!  I am being attacked by %s!", ch->name );
    do_yell( victim, buf );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if (arg[0] == '\0')
    {
        send_to_char("Backstab whom?\n\r",ch);
        return;
    }

    if (ch->fighting != NULL)
    {
	send_to_char("{hYou're facing the wrong end.{x\n\r",ch);
	return;
    }
 
    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
	send_to_char( "{hYou need to wield a primary weapon to backstab.{x\n\r", ch );
	return;
    }

    if ( obj->value[0] != WEAPON_DAGGER
      && obj->value[0] != WEAPON_SWORD
      && obj->value[0] != WEAPON_AXE)
    {
        send_to_char("You need a sharp weapon to backstab.\n\r",ch);
        return;
    }

    if ( victim->hit < victim->max_hit / 3)
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( number_percent( ) < get_skill(ch,gsn_backstab)
    || ( get_skill(ch,gsn_backstab) >= 2 && !IS_AWAKE(victim) ) )
    {
	check_improve(ch,gsn_backstab,TRUE,1);
	multi_hit( ch, victim, gsn_backstab );
    }
    else
    {
	check_improve(ch,gsn_backstab,FALSE,1);
	damage( ch, victim, 0, gsn_backstab,DAM_NONE,TRUE);
    }

    return;
}

void do_circle( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
  
    if ( get_skill(ch,gsn_circle) == 0
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_circle].skill_level[ch->class]))
    {	
	send_to_char("Circle? What's that?\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
 
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
        send_to_char( "You need to wield a primary weapon to circle.\n\r", ch );
        return;
    }

    if ( victim->hit < victim->max_hit / 6)
    {
	act( "$N is hurt and suspicious ... you can't sneak around.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }
  
    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( !can_see( ch, victim ) )
    {
	send_to_char("You stumble blindly into a wall.\n\r",ch);
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_circle].beats );
    if ( number_percent( ) < get_skill(ch,gsn_circle)
    || ( get_skill(ch,gsn_circle) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_circle,TRUE,1);
	act( "{i$n circles around behind you.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou circle around $N.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n circles around behind $N.{x", ch, NULL, victim, TO_NOTVICT);
        multi_hit( ch, victim, gsn_circle );
    }
    else
    {
        check_improve(ch,gsn_circle,FALSE,1);
	act( "{i$n tries to circle around you.{x", ch, NULL, victim, TO_VICT);
	act( "{h$N circles with you.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n tries to circle around $N.{x", ch, NULL, victim, TO_NOTVICT);
        damage( ch, victim, 0, gsn_circle,DAM_NONE,TRUE);
    }
 
    return;
}

void do_feed( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam;
  
    if ( get_skill(ch,gsn_feed) == 0
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_feed].skill_level[ch->class]))
    {	
	send_to_char("Feed? What's that?\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char("You are not fighting anyone.\n\r", ch);
        return;
    }

   
    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }
  
    WAIT_STATE( ch, skill_table[gsn_feed].beats );
    if ( number_percent( ) < get_skill(ch,gsn_feed)*5/6
    || ( get_skill(ch,gsn_feed) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_feed,TRUE,1);
	act( "{i$n bites you.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou bite $N.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n bites $N.{x", ch, NULL, victim, TO_NOTVICT);
	dam=dice( ch->level, ch->damroll/5 );
        damage( ch, victim, dam, gsn_feed,DAM_NEGATIVE,TRUE);
    }
    else
    {
        check_improve(ch,gsn_feed,FALSE,1);
	act( "{i$n tries to bite you, but hits only air.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou chomp a mouthfull of air.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n tries to bite $N.{x", ch, NULL, victim, TO_NOTVICT);
        damage( ch, victim, 0, gsn_feed,DAM_NEGATIVE,TRUE);
    }
 
    return;
}



void do_death_bite( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam,roll;
    roll=number_range(1,110);
  
    if ( get_skill(ch,gsn_death_bite) == 0
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_death_bite].skill_level[ch->class]))
    {	
	send_to_char("Death bite? What's that?\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char("You are not fighting anyone.\n\r", ch);
        return;
    }

   
    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }
  
    WAIT_STATE( ch, skill_table[gsn_death_bite].beats );
    if ( roll < ch->level )
    {
        check_improve(ch,gsn_death_bite,TRUE,1);
	act( "{i$n bites you.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou bite $N.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n bites $N.{x", ch, NULL, victim, TO_NOTVICT);
	dam=dice( ch->level, ch->damroll/4 );
        damage( ch, victim, dam, gsn_death_bite,DAM_NEGATIVE,TRUE);
    }
    else
    {
        check_improve(ch,gsn_death_bite,FALSE,1);
	act( "{i$n tries to bite you, but hits only air.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou chomp a mouthfull of air.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n tries to bite $N.{x", ch, NULL, victim, TO_NOTVICT);
        damage( ch, victim, 0, gsn_death_bite,DAM_NEGATIVE,TRUE);
    }
 
    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
        if ( ch->position == POS_FIGHTING )
            ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	||   pexit->u1.to_room == NULL
	||   IS_SET(pexit->exit_info, EX_CLOSED)
	||   number_range(0,ch->daze) != 0
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;

	move_char( ch, door, FALSE, FALSE );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has {Yfled{x!", ch, NULL, NULL, TO_ROOM );
        if ( !IS_NPC(ch) )
        {
            send_to_char( "{BYou {Yflee{B from combat!{x\n\r", ch );
            if( ( (ch->class == 2) || (ch->class == (MAX_CLASS/3)+1) )
            && (number_percent() < 3*(ch->level/2) ) )
            {
                if ( IS_NPC(victim) || ch->attacker == FALSE)
                {
                    send_to_char( "You {Ysnuck away{x safely.\n\r", ch);
                } else
                {    
                    send_to_char( "You feel something singe your butt on the way out.\n\r", ch);                                               
                    act( "$n is nearly {Yzapped{x in the butt by a lightning bolt from above!", ch, NULL, NULL, TO_ROOM );
                    ch->hit -= (ch->hit/8);
                }
            } else
            {    
                if ( !IS_NPC(victim) && ch->attacker == TRUE)
                {
                    send_to_char( "The {RWrath of Thoth {YZAPS{x your butt on the way out!\n\r", ch);
                    act( "$n is {Yzapped{x in the butt by a lightning bolt from above!", ch, NULL, NULL, TO_ROOM );
                    ch->hit -= (ch->hit/4);
                }
                send_to_char( "You lost 10 exp.\n\r", ch);
                gain_exp( ch, -10 );
            }
        }    
	ch->in_room = now_in;
	stop_fighting( ch, TRUE );
	return;
    }

    send_to_char( "{z{RYour escape was blocked!!!{x\n\r", ch );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about {Yfleeing{x instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( IS_NPC(fch) && !is_same_group(ch,victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( number_percent( ) > get_skill(ch,gsn_rescue))
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	check_improve(ch,gsn_rescue,FALSE,1);
	return;
    }

    act( "{yYou rescue $N!{x",  ch, NULL, victim, TO_CHAR    );
    act( "{y$n rescues you!{x", ch, NULL, victim, TO_VICT    );
    act( "{y$n rescues $N!{x",  ch, NULL, victim, TO_NOTVICT );
    check_improve(ch,gsn_rescue,TRUE,1);

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    set_fighting( ch, fch );
    set_fighting( fch, ch );
    return;
}


void do_thunderkick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_thunderkick].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK))
	return;

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }


    dam = number_range( 1, ch->level*2 );
	damage(ch,victim,number_range( dam, (ch->level*1.5) ), gsn_thunderkick,DAM_BASH,TRUE);
    dam = number_range( 1, ch->level*2 );
	damage(ch,victim,number_range( dam, (ch->level*1.5) ), gsn_thunderkick,DAM_BASH,TRUE);
    dam = number_range( 1, ch->level*2 );
	damage(ch,victim,number_range( dam, (ch->level*1.5) ), gsn_thunderkick,DAM_BASH,TRUE);
    dam = number_range( 1, ch->level*2 );
	damage(ch,victim,number_range( dam, (ch->level*1.5) ), gsn_thunderkick,DAM_BASH,TRUE);
    dam = number_range( 1, ch->level*2 );
	damage(ch,victim,number_range( dam, (ch->level*1.5) ), gsn_thunderkick,DAM_BASH,TRUE);
    WAIT_STATE( ch, skill_table[gsn_thunderkick].beats );


    return;
}





void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_kick].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK))
	return;

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    dam = number_range( 1, ch->level );
    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( get_skill(ch,gsn_kick) > number_percent())
    {
	damage(ch,victim,number_range( dam, (ch->level*1.5) ), gsn_kick,DAM_BASH,TRUE);
	check_improve(ch,gsn_kick,TRUE,1);
    }
    else
    {
	damage( ch, victim, 0, gsn_kick,DAM_BASH,TRUE);
	check_improve(ch,gsn_kick,FALSE,1);
    }
    return;
}


void do_impale( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *wield;
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_impale].skill_level[ch->class] )
    {
	send_to_char(
	    "You Don't Know Impale!.\n\r", ch );
	return;
    }

/*    if ( !can_see(ch,victim))
    {
        send_to_char("You bounce off the walls in utter blindness.\n\r",ch);
        return;
    } changed to: (cause it fucked up the mud)*/

    if (IS_AFFECTED(ch,AFF_BLIND))
    {
    send_to_char("You bounce off the walls in utter blindness.\n\r",ch);
    return;
    }
    if ((wield=get_eq_char(ch, WEAR_WIELD))==NULL)
    {
	send_to_char("Try a weapon.\n\r",ch);
	return;
    }

    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_IMPALE))
	return;

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    dam = dice(ch->level,ch->damroll/3);
    WAIT_STATE( ch, skill_table[gsn_impale].beats );
    if ( get_skill(ch,gsn_impale) > number_percent())
    {
	damage(ch,victim,dice( ch->level, (ch->damroll/3) ),
gsn_impale,DAM_BASH,TRUE);
	check_improve(ch,gsn_impale,TRUE,1);
    }
    else
    {
	damage( ch, victim, 0, gsn_impale,DAM_BASH,TRUE);
	check_improve(ch,gsn_impale,FALSE,1);
    }
    return;
}

void do_deathblow( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_deathblow].skill_level[ch->class] )
    {
	send_to_char(
	    "You Don't Know Deathblow!.\n\r", ch );
	return;
    }

    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DEATHBLOW))
	return;

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    dam = number_range( (ch->level*4), (ch->level*15)+(ch->damroll*15) );
    WAIT_STATE( ch, skill_table[gsn_deathblow].beats );
    if ( get_skill(ch,gsn_deathblow) > number_percent())
    {
	damage(ch,victim,number_range( dam, (ch->level*1.5) ), 
gsn_deathblow,DAM_BASH,TRUE);
	check_improve(ch,gsn_deathblow,TRUE,1);
    }
    else
    {
	damage( ch, victim, 0, gsn_deathblow,DAM_BASH,TRUE);
	check_improve(ch,gsn_deathblow,FALSE,1);
    }
    return;
}




void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance,hth,ch_weapon,vict_weapon,ch_vict_weapon;

    hth = 0;

    if ((chance = get_skill(ch,gsn_disarm)) == 0)
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL 
    &&   ((hth = get_skill(ch,gsn_hand_to_hand)) == 0
    ||    (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))))
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
	send_to_char( "{hYour opponent is not wielding a weapon.{x\n\r", ch );
	return;
    }

    /* find weapon skills */
    ch_weapon = get_weapon_skill(ch,get_weapon_sn(ch));
    vict_weapon = get_weapon_skill(victim,get_weapon_sn(victim));
    ch_vict_weapon = get_weapon_skill(ch,get_weapon_sn(victim));

    /* modifiers */

    /* skill */
    if ( get_eq_char(ch,WEAR_WIELD) == NULL)
	chance = chance * hth/150;
    else
	chance = chance * ch_weapon/100;

    chance += (ch_vict_weapon/2 - vict_weapon) / 2; 

    /* dex vs. strength */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_STR);

    /* level */
    chance += (ch->level - victim->level) * 2;

    chance /= 2;
 
    /* and now the attack */
    if (number_percent() < chance)
    {
	if (((chance = get_skill(victim,gsn_grip)) == 0)
	||   (!IS_NPC(victim)
	&&    victim->level < skill_table[gsn_grip].skill_level[victim->class]))
	{
	    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
	    disarm( ch, victim );
	    check_improve(ch,gsn_disarm,TRUE,1);
	    return;
	}
	if (number_percent() > (chance/5)*4)
	{
	    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
	    disarm( ch, victim );
	    check_improve(ch,gsn_disarm,TRUE,1);
	    check_improve(victim,gsn_grip,FALSE,1);
	    return;
	}
	check_improve(victim,gsn_grip,TRUE,1);
    }
    WAIT_STATE(ch,skill_table[gsn_disarm].beats);
    act("{hYou fail to disarm $N.{x",ch,NULL,victim,TO_CHAR);
    act("{i$n tries to disarm you, but fails.{x",ch,NULL,victim,TO_VICT);
    act("{k$n tries to disarm $N, but fails.{x",ch,NULL,victim,TO_NOTVICT);
    check_improve(ch,gsn_disarm,FALSE,1);
    return;
}

void do_surrender( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    if ( (mob = ch->fighting) == NULL )
    {
	send_to_char( "But you're not fighting!\n\r", ch );
	return;
    }
    act( "You surrender to $N!", ch, NULL, mob, TO_CHAR );
    act( "$n surrenders to you!", ch, NULL, mob, TO_VICT );
    act( "$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT );
    stop_fighting( ch, TRUE );
 
    if ( !IS_NPC( ch ) && IS_NPC( mob ) 
    &&   ( !HAS_TRIGGER( mob, TRIG_SURR ) 
        || !mp_percent_trigger( mob, ch, NULL, NULL, TRIG_SURR ) ) )
    {
	act( "$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR );
	multi_hit( mob, ch, TYPE_UNDEFINED );
    }
}

void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to {RSLAY{x, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && victim->level >= get_trust(ch) )
    {
	send_to_char( "{hYou failed.{x\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) || get_trust(ch) >= GOD )
    { 
	act( "{hYou summon up Mr. Howard who sits on $N!{x",  ch, NULL, victim, TO_CHAR    );
	act( "{h$n summons Mr. Howard, who sits on you!{x", ch, NULL, victim, TO_VICT    );
	act( "{h$n summons Mr. Howard, who sits on $N!{x",  ch, NULL, victim, TO_NOTVICT );
	act( "{h$N is DEAD!{x",  ch, NULL, victim, TO_CHAR    );
	act( "{hYou are DEAD!{x",  ch, NULL, victim, TO_VICT    );
	act( "{h$N is DEAD!{x",  ch, NULL, victim, TO_NOTVICT    );
	raw_kill( victim, ch );
    }
    else
    {
	act( "{i$N wields a sword called '{z{RGodSlayer{i'!{x",  ch, NULL, victim, TO_CHAR    );
	act( "{hYou wield a sword called '{z{RGodSlayer{h'!{x", ch, NULL, victim, TO_VICT    );
	act( "{k$N wields a sword called '{z{RGodSlayer{k'!{x",  ch, NULL, victim, TO_NOTVICT );
        act("{i$N's slice takes off your left arm!{x",ch,NULL,victim,TO_CHAR);
        act("{hYour slice takes off $n's left arm!{x",ch,NULL,victim,TO_VICT);
        act("{k$N's slice takes off $n's left arm!{x",ch,NULL,victim,TO_NOTVICT);
        act("{i$N's slice takes off your right arm!{x",ch,NULL,victim,TO_CHAR);
        act("{hYour slice takes off $n's right arm!{x",ch,NULL,victim,TO_VICT);
        act("{k$N's slice takes off $n's right arm!{x",ch,NULL,victim,TO_NOTVICT);
        act("{i$N's slice cuts off both of your legs!{x",ch,NULL,victim,TO_CHAR);
        act("{hYour slice cuts off both of $n's legs!{x",ch,NULL,victim,TO_VICT);
        act("{k$N's slice cuts off both of $n's legs!{x",ch,NULL,victim,TO_NOTVICT);
        act("{i$N's slice beheads you!{x",ch,NULL,victim,TO_CHAR);
        act("{hYour slice beheads $n!{x",ch,NULL,victim,TO_VICT);
        act("{k$N's slice beheads $n!{x",ch,NULL,victim,TO_NOTVICT);
        act("{iYou are DEAD!!!{x",ch,NULL,victim,TO_CHAR);
	act("{h$n is DEAD!!!{x",ch,NULL,victim,TO_VICT);
	act("{k$n is DEAD!!!{x", ch,NULL,victim,TO_NOTVICT);
	act("A sword called '{z{RGodSlayer{x' vanishes.",ch,NULL,victim,TO_VICT);
	act("A sword called '{z{RGodSlayer{x' vanishes.",ch,NULL,victim,TO_NOTVICT);
	raw_kill( ch, victim );
    }
    return;
}


void do_blind_rage( CHAR_DATA *ch, char *argument )
{

   CHAR_DATA *pChar;
   CHAR_DATA *pChar_next;
   OBJ_DATA *wield;
   int dam;
   
   if (    !IS_NPC( ch ) 
        && ch->level < skill_table[gsn_blind_rage].skill_level[ch->class] )
   {
      send_to_char( "You don't know how to do that...\n\r", ch );
      return;
   }

   if (ch->stunned)
   {
       send_to_char("You're still a little woozy.\n\r",ch);
       return;
   }

   if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a weapon first...\n\r", ch );
      return;
   }
   
   act( "In a blind rage $n attacks everyone in the room!", ch, wield, NULL, TO_ROOM );
   act( "In a blind rage you attack everything in the room!",  ch, wield, NULL, TO_CHAR );
   
    for ( pChar = char_list; pChar != NULL; pChar = pChar_next )
    {
        pChar_next        = pChar->next;
        if ( pChar->in_room == NULL )
            continue;
        if ( pChar->in_room == ch->in_room )
        {
            if ( pChar != ch && !is_safe_spell(ch,pChar,TRUE))
            {
                if ( ( ch->fighting == NULL )
                && ( !IS_NPC( ch ) )
                && ( !IS_NPC( pChar ) ) )
                {
                    ch->attacker = TRUE;
                    pChar->attacker = FALSE;
                }

                if (is_same_group(pChar,ch) )
                {
                    continue;
                }
         

        act( "$n turns towards YOU!", ch, NULL, pChar, TO_VICT    );
	check_improve(ch,gsn_blind_rage,TRUE,4);
	dam = dice (ch->damroll/1.5, ch->level );
        damage( ch, pChar, dam, gsn_blind_rage,DAM_SLASH,TRUE);
      }
   }
   
   WAIT_STATE( ch, skill_table[gsn_blind_rage].beats );
   
   }

   return;
}


void do_whirlwind( CHAR_DATA *ch, char *argument )
{

   CHAR_DATA *pChar;
   CHAR_DATA *pChar_next;
   OBJ_DATA *wield;
   OBJ_DATA *second;
   int dam;
   
   if (    !IS_NPC( ch ) 
        && ch->level < skill_table[gsn_whirlwind].skill_level[ch->class] )
   {
      send_to_char( "You don't know how to do that...\n\r", ch );
      return;
   }

   if (ch->stunned)
   {
       send_to_char("You're still a little woozy.\n\r",ch);
       return;
   }

   if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a sharp weapon first...\n\r", ch );
      return;
   }
   
   if (wield->value[0] != WEAPON_SWORD && wield->value[0] != WEAPON_DAGGER
    && wield->value[0] != WEAPON_AXE)
   {
      send_to_char( "You need to wield a sharp weapon first...\n\r", ch );
      return;
   }

   act( "In a whirlwind $n attacks everyone in the room!", ch, wield, NULL, TO_ROOM );
   act( "In a whirlwind you attack everything in the room!",  ch, wield, NULL, TO_CHAR );
   
    for ( pChar = char_list; pChar != NULL; pChar = pChar_next )
    {
        pChar_next        = pChar->next;
        if ( pChar->in_room == NULL )
            continue;
        if ( pChar->in_room == ch->in_room )
        {
            if ( pChar != ch && !is_safe_spell(ch,pChar,TRUE))
            {
                if ( ( ch->fighting == NULL )
                && ( !IS_NPC( ch ) )
                && ( !IS_NPC( pChar ) ) )
                {
                    ch->attacker = TRUE;
                    pChar->attacker = FALSE;
                }

                if (is_same_group(pChar,ch) )
                {
                    continue;
                }
         

        act( "$n turns towards YOU!", ch, NULL, pChar, TO_VICT    );
	check_improve(ch,gsn_whirlwind,TRUE,4);
	dam = dice (ch->damroll/1.5, ch->level )*.5;
        damage( ch, pChar, dam, gsn_whirlwind,DAM_SLASH,TRUE);
        if ( (second=get_eq_char(ch, WEAR_SECONDARY)) != NULL)
        {
        dam = dice (ch->damroll/1.5, ch->level )*.5;
        damage( ch, pChar, dam, gsn_whirlwind,DAM_SLASH,TRUE);
        }
      }
   }
   
   WAIT_STATE( ch, skill_table[gsn_whirlwind].beats );
   
   }

   return;
}



void do_multishot( CHAR_DATA *ch, char *argument )
{

   CHAR_DATA *pChar;
   CHAR_DATA *pChar_next;
   OBJ_DATA *wield;
   OBJ_DATA *quiver;
   int dam;
   
   if (    !IS_NPC( ch ) 
        && ch->level < skill_table[gsn_multishot].skill_level[ch->class] )
   {
      send_to_char( "You don't know how to do that...\n\r", ch );
      return;
   }

   if (ch->stunned)
   {
       send_to_char("You're still a little woozy.\n\r",ch);
       return;
   }

   if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a bow first...\n\r", ch );
      return;
   }
   if ( wield->value[0] != WEAPON_BOW)
   {
        send_to_char("You need to wield a bow first...\n\r",ch);
        return;
    }

    quiver = get_eq_char(ch, WEAR_SECONDARY);
   if (quiver == NULL)
    {
        send_to_char("You need to second wield a quiver.\n\r",ch);
        return;
    }

    if (quiver->value[0] < 1)
    {
        send_to_char("You need to refill your quiver!\n\r",ch);
        return;
    }

    quiver->value[0] -= 1;
   act( "$n splits up an arrow and attacks everyone in the room!", ch, wield, NULL, TO_ROOM );
   act( "You split up an arrow and attack everyone in the room!",  ch, wield, NULL, TO_CHAR );
   
    for ( pChar = char_list; pChar != NULL; pChar = pChar_next )
    {
        pChar_next        = pChar->next;
        if ( pChar->in_room == NULL )
            continue;
        if ( pChar->in_room == ch->in_room )
        {
            if ( pChar != ch && !is_safe_spell(ch,pChar,TRUE))
            {
                if ( ( ch->fighting == NULL )
                && ( !IS_NPC( ch ) )
                && ( !IS_NPC( pChar ) ) )
                {
                    ch->attacker = TRUE;
                    pChar->attacker = FALSE;
                }

                if (is_same_group(pChar,ch) )
                {
                    continue;
                }
         

        act( "$n turns towards YOU!", ch, NULL, pChar, TO_VICT    );
	check_improve(ch,gsn_multishot,TRUE,4);
	dam = dice (ch->damroll/1.5, ch->level );
        damage( ch, pChar, dam, gsn_multishot,DAM_SLASH,TRUE);
      }
   }
   
   WAIT_STATE( ch, skill_table[gsn_multishot].beats );
   
   }

   return;
}



void do_rub(CHAR_DATA *ch, char *argument)
{
if (IS_NPC( ch ))
return;

   if (ch->stunned)
   {
       send_to_char("You're still a little woozy.\n\r",ch);
       return;
   }
     if ( number_range(1,100 ) < get_skill(ch,gsn_rub))
    {
	affect_strip(ch,skill_lookup("fire breath"));
	affect_strip(ch,gsn_gouge);
	affect_strip(ch,gsn_dirt);
	send_to_char("You rub your eyes.\n\r",ch);
        act( "$n rubs $s eyes.\n\r", ch, NULL, NULL, TO_ROOM );
        check_improve(ch,gsn_rub,TRUE,4);
    }

    else
    {
	send_to_char("You rub your eyes, but you still can't see!\n\r",ch);
        check_improve(ch,gsn_rub,FALSE,4);
    }
//	check_improve(ch,gsn_rub,TRUE,4);
   return;
} 

void do_bind( CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int hpheal;
    int chance;

    one_argument(argument,arg);

    if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if ( victim->hit >= victim->max_hit)
    {
	send_to_char("They are just fine without your help.\n\r",ch);
	return;
    }
    if (ch==victim)
    {
        send_to_char("You cannot bind yourself.\n\r",ch);
        return;
    }

    chance = number_range( 1, 100);
    if (chance > get_skill(ch,gsn_bind))
    {
	send_to_char("You failed.\n\r",ch);
	check_improve(ch,gsn_bind,FALSE,2);
   WAIT_STATE( ch, skill_table[gsn_bind].beats );
	return;
    }
    act("$n kneels down and begins to bandage $N.\n\r",ch,NULL,victim,TO_NOTVICT);
    act("$n kneels down and touches bandages to your wounds.\n\r",ch,NULL,victim,TO_VICT);
    act("You apply bandages to $N's wounds.\n\r",ch,NULL,victim,TO_CHAR);
    hpheal=victim->max_hit/10;
    victim->hit += hpheal;
     if (victim->hit >= victim->max_hit)
    {
    victim->hit = victim->max_hit;
    return;
    }
   WAIT_STATE( ch, skill_table[gsn_bind].beats );
    check_improve(ch,gsn_bind,FALSE,2);
    return;
}

void do_holy_fury(CHAR_DATA *ch, char *argument)
{
AFFECT_DATA af;

if ( get_skill(ch,gsn_holy_fury) == 0
|| (!IS_NPC(ch)
&& ch->level < skill_table[gsn_holy_fury].skill_level[ch->class]))
{ 
send_to_char("You do not know how to do that..\n\r",ch);
return;
}

if (IS_AFFECTED(ch,AFF_CALM))
{
send_to_char("{hYou're feeling to mellow.{x\n\r",ch);
return;
}

if ( is_affected(ch, gsn_holy_fury))
{
send_to_char("You are already filled with divine wrath.\n\r",ch);
return;
} 

WAIT_STATE( ch, skill_table[gsn_holy_fury].beats );

if (number_percent() > ch->pcdata->learned[gsn_holy_fury])
{
act("$n {6becomes filled with {#d{2i{7vi{2n{#e {!f{1u{!r{1y{x!", ch, NULL, NULL, TO_ROOM);
check_improve(ch,gsn_holy_fury,TRUE,2);

} 

af.where = TO_AFFECTS;
af.type = gsn_holy_fury;
af.level = ch->level;
af.duration = ch->level / 5;
af.location = APPLY_AC;
af.modifier = ch->level;
af.bitvector = 0;
affect_to_char( ch, &af ); 

af.where = TO_AFFECTS;
af.type = gsn_holy_fury;
af.level = ch->level;
af.duration = ch->level / 5;
af.location = APPLY_HITROLL;
af.modifier = ch->level / 3;
af.bitvector = 0;
affect_to_char( ch, &af ); 

af.where = TO_AFFECTS;
af.type = gsn_holy_fury;
af.level = ch->level;
af.duration = ch->level / 5;
af.location = APPLY_DAMROLL;
af.modifier = ch->level / 3;
af.bitvector = 0;
affect_to_char( ch, &af ); 

send_to_char("You become filled with {3ri{2g{7h{#t{7e{2o{3us {!f{1u{!r{1y{x.\n\r",ch);

return;
}

void do_knee( CHAR_DATA *ch, char *argument )
{
CHAR_DATA *victim;
int dam;

if ( !IS_NPC(ch)
&& ch->level < skill_table[gsn_knee].skill_level[ch->class] )
{
send_to_char(
"You better leave the martial arts to fighters.\n\r", ch );
return;
}

if ( ( victim = ch->fighting ) == NULL )
{
send_to_char( "You aren't fighting anyone.\n\r", ch );
return;
}

if (ch->stunned)
{
send_to_char("You're still a little woozy.\n\r",ch);
return;
}

WAIT_STATE( ch, skill_table[gsn_knee].beats );
if ( get_skill(ch,gsn_knee) > number_percent())
{
check_improve(ch,gsn_knee,TRUE,1);
act( "{i$n grabs your head and slams their knee into your face.{x", ch, NULL, victim, TO_VICT);
act( "{hYou grab $N and slam your knee into their face.{x", ch, NULL, victim, TO_CHAR);
act( "{k$n grabs $N and slams $s knee into their face.{x", ch, NULL, victim, TO_NOTVICT);
dam = dice ( (ch->level), ( 25 ));
damage( ch, victim, dam, gsn_knee,DAM_BASH,TRUE);
}
else
{
check_improve(ch,gsn_knee,FALSE,1);
act( "{i$n tries to knee you and misses.{x", ch, NULL, victim, TO_VICT);
act( "{hYou try to knee $N and miss.{x", ch, NULL, victim, TO_CHAR);
act( "{k$n tries to knee $N and misses.{x", ch, NULL, victim, TO_NOTVICT);
damage( ch, victim, 0, gsn_knee,DAM_BASH,TRUE);
}
return;
}

void do_strike( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance;
  
    if ( get_skill(ch,gsn_strike) == 0
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_strike].skill_level[ch->class]))
    {	
	send_to_char("Strike? What's that?\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
 
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
        send_to_char( "You need to wield a primary weapon to strike.\n\r", ch );
        return;
    }
  
    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }


    WAIT_STATE( ch, skill_table[gsn_strike].beats );
    if ( number_percent( ) < get_skill(ch,gsn_strike)
    || ( get_skill(ch,gsn_strike) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_strike,TRUE,1);
	act( "{i$n strikes you in a vital area.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou strike $N in a vital place.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n strikes $N in a vital area.{x", ch, NULL, victim, TO_NOTVICT);
        multi_hit( ch, victim, gsn_strike );
        if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL)
        {
            return;
        }
        if ( IS_OBJ_STAT(obj,ITEM_NOREMOVE))
        {
	    return;
        }
	chance = (get_skill(ch,gsn_disarm)/2);
	if (number_percent() <= chance )
	{
	    chance = (get_skill(ch,gsn_disarm)/2);
	    if (number_percent() <= chance )

	    act("{7Unable to block $n's strike, your weapon tears free!{x",
		ch,NULL,victim,TO_VICT);
	    act("{7$N's weapon breaks free from your strike!{x",ch,NULL,victim,TO_CHAR);
	    check_improve(ch,gsn_disarm,TRUE,1);
            obj_from_char( obj );
            if ( IS_OBJ_STAT(obj,ITEM_NODROP) || IS_OBJ_STAT(obj,ITEM_INVENTORY ) )
        	obj_to_char( obj, victim );
            else
            {
	        obj_to_room( obj, victim->in_room );
	        if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
	        get_obj(victim,obj,NULL);
            }
        }
    
    }
    else
    {
        check_improve(ch,gsn_strike,FALSE,1);
	act( "{i$n tries to strike you in a vital area.{x", ch, NULL, victim, TO_VICT);
	act( "{h$N dodges your strike.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n tries to strike $N in a vital area.{x", ch, NULL, victim, TO_NOTVICT);
        damage( ch, victim, 0, gsn_strike,DAM_NONE,TRUE);
    }
 
    return;
}

void do_range( CHAR_DATA *ch, char *argument )
{
     char buf[MAX_STRING_LENGTH];
     int rangemax, rangemin;
     rangemin=((ch->level*4)/5);
     rangemax=((ch->level*5)/4);
     if (rangemin > 101)
     rangemin=101;
     if (rangemax > 101)
     rangemax=101;
     if (IS_NPC(ch))
     {
     send_to_char("NPCs can be toasted by anyone!\n\r",ch);
     return;
     }
     if (ch->level <= 25)
     {
     send_to_char("You don't need to worry about PK yet!\n\r",ch);
     return;
     }
     if ( IS_IMMORTAL(ch))
     {
     send_to_char("No, Just No, but if you must...\n\r",ch);
     sprintf(buf,"{!You can attack people equal or greater than {^%d{x.\n\r",rangemin);
     send_to_char(buf,ch);
     sprintf(buf,"{!You should watch out for people up to level {^%d{x.\n\r",rangemax);
     send_to_char(buf,ch);

     return;
     }
     else
     {
     sprintf(buf,"{!You can attack people equal or greater than {^%d{x.\n\r",rangemin);
     send_to_char(buf,ch);
     sprintf(buf,"{!You should watch out for people up to level {^%d{x.\n\r",rangemax);
     send_to_char(buf,ch);
     return;
     }
     }

void do_catchup(CHAR_DATA *ch, char *argument)
{
        ch->pcdata->last_note = current_time;
        ch->pcdata->last_idea = current_time;
	ch->pcdata->last_news = current_time;
	ch->pcdata->last_changes = current_time;
	ch->pcdata->last_weddings = current_time;
        ch->pcdata->last_rpnote = current_time;
        send_to_char("You are caught up on your notes, RPnotes, ideas, news, changes, ", ch);
    if (IS_IMMORTAL(ch))
    {
	ch->pcdata->last_penalty = current_time;
        send_to_char("penalties, ", ch);
    }
        send_to_char("and weddings.\n\r", ch);
    return;
}

void do_pklist(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA *d;
   char buf[MSL];

   send_to_char("{cPlayer        {RPKills  {rPK Deaths  {wPKs for KILLER{x\n\r",ch);

   for (d=descriptor_list; d != NULL; d = d->next)
   {

	if(d->connected == CON_PLAYING)
	{
	CHAR_DATA *wch;
 	if (d->connected != CON_PLAYING || !can_see(ch,d->character))
	    continue;
	
	wch = ( d->original != NULL ) ? d->original : d->character;
 	if (!can_see(ch,wch))
	    continue;
        if ((wch->level >= LEVEL_IMMORTAL) && (ch->level >= LEVEL_IMMORTAL))
        {
	   sprintf(buf, "{M*{c%-11s  {R%3d     {r%3d        {w%2d{x\n\r",
	   wch->name,
	   wch->pcdata->pktotal,
	   wch->pcdata->pkdeaths,
	   wch->pcdata->pkkills);
           send_to_char(buf,ch);
        continue;
        }
        if ((wch->level >= LEVEL_IMMORTAL) && (ch->level < LEVEL_IMMORTAL))
            continue;

	   sprintf(buf, "{c%-12s  {R%3d     {r%3d        {w%2d{x\n\r",
	   wch->name,
	   wch->pcdata->pktotal,
	   wch->pcdata->pkdeaths,
	   wch->pcdata->pkkills);
           send_to_char(buf,ch);
	}
   }
   return;
}
