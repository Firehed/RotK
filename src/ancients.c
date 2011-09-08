/*****************************************************************
* Spells for the new class of Realm of the Keepers, the Ancients *
* These spells were designed by Firehed, and Wardrof.  RotK can  *
* be found at mustang.tamarisk.ab.ca port 6900.  The IMP's       *
* E-mails are firehed13@hotmail.com (firehed), and Wardrof is at *
* kheez69@hotmail.com                                            *
******************************************************************/
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "tables.h"
#include "recycle.h"

extern char *target_name;
/*
 * Local Functions
 */
void    regen_update args( ( void ) );


/*
 * Imported functions
 */
void    dam_message 	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool immune ) );

void regen_update(void)
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    int hitgain;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {
         ch = d->character;
            if (IS_AFFECTED(ch, AFF_REGEN))
            {
                hitgain = ch->hit * .02;

                if (ch->hit <= 100)
                {
                ch->hit += 25;
                return;
                }
            ch->hit += hitgain;
            if (ch->hit >= ch->max_hit)
            ch->hit = ch->max_hit;
            return;
            }
        }
    }
    return;
}
void spell_ultima(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;
        send_to_char("A swirling mass of colors envelops the room!\n\r",ch);
        act( "$n calls down a swirling mass of colors!\n\r",ch,NULL,NULL,TO_ROOM);
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && !is_safe_spell(ch,vch,TRUE))
	    {
		if ( ( ch->fighting == NULL )
		&& ( !IS_NPC( ch ) ) )
		{
		    ch->attacker = TRUE;
		    vch->attacker = FALSE;
		}
   if (is_same_group(vch,ch) )
                {
                    continue;
                }

        /* Wanna know how THIS shit works?? */
          dam = dice(ch->level + ch->level/3, ch->level/3);
  damage( ch, vch, dam, sn, DAM_FIRE,TRUE );

          dam = dice(ch->level + ch->level/4, ch->level/3);
  damage( ch, vch, dam, sn, DAM_COLD,TRUE );

          dam = dice(ch->level + ch->level/5, ch->level/3);
  damage( ch, vch, dam, sn, DAM_LIGHTNING,TRUE );

          dam = dice(ch->level + ch->level/6, ch->level/3);
  damage( ch, vch, dam, sn, DAM_DROWNING,TRUE );

          dam = dice(ch->level + ch->level/7, ch->level/3);
  damage( ch, vch, dam, sn, DAM_ACID,TRUE );

          dam = dice(ch->level + ch->level/8, ch->level/3);
  damage( ch, vch, dam, sn, DAM_HOLY,TRUE );

          dam = dice(ch->level + ch->level/9, ch->level/3);
  damage( ch, vch, dam, sn, DAM_NEGATIVE,TRUE );

          dam = dice(ch->level + ch->level/10, ch->level/3);
  damage( ch, vch, dam, sn, DAM_SOUND,TRUE );
	    }
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "Somewhere off in the distance you see a huge mass of colors.\n\r", vch );
    }


    return;
}




void spell_inferno(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;

    send_to_char( "You call down a storm of fire!\n\r", ch );
    act( "$n calls down a storm of fire!\n\r", ch, NULL, NULL, TO_ROOM );

/*    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }*/
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && !is_safe_spell(ch,vch,TRUE))
	    {
		if ( ( ch->fighting == NULL )
		&& ( !IS_NPC( ch ) ) )
		{
		    ch->attacker = TRUE;
		    vch->attacker = FALSE;
		}
          dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_FIRE,TRUE );

    dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_FIRE,TRUE );

    dam = dice(ch->level/2, ch->level)*.9;

  damage( ch, vch, dam, sn, DAM_FIRE,TRUE );

    dam = dice(ch->level/2, ch->level)*.6;

  damage( ch, vch, dam, sn, DAM_FIRE,TRUE );
	    }
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "In the distace you can see the sky shoot forth fire.\n\r", vch );
    }


    return;
}

void spell_burn(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    send_to_char( "You call down a fire from the sky!\n\r", ch );
    act( "$n calls down a fire from the sky!\n\r", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_FIRE,TRUE );

    dam = dice(ch->level/2, ch->level)*.9;

  damage( ch, victim, dam, sn, DAM_FIRE,TRUE );

    dam = dice(ch->level/2, ch->level)*.6;

  damage( ch, victim, dam, sn, DAM_FIRE,TRUE );
    return;
}

void spell_fire(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    send_to_char( "You call down a small fire from the sky!\n\r", ch );
    act( "$n calls down a small fire from the sky!\n\r", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }
    if (ch->level < 20)
        ch->mana += 100;

    dam = dice(ch->level/2, ch->level);
  damage( ch, victim, dam, sn, DAM_FIRE,TRUE );

    dam = dice(ch->level/2, ch->level)*.9;

  damage( ch, victim, dam, sn, DAM_FIRE,TRUE );
    return;
}

void spell_water(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int chance;
    int dam;

    chance = number_range( 1, 100);

    send_to_char( "You call a massive wave of water!\n\r", ch );
    act( "A massive wave of water flows over you!\n\r", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }
        if (ch->level < 20)
        ch->mana += 100;

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE );

    if (chance > 60)
    {
        return;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE );
    return;
}

void spell_flood(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int chance;
    int dam;

    chance = number_range( 1, 100);

    send_to_char( "You call a massive wave of water!\n\r", ch );
    act( "A massive wave of water flows over you!\n\r", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE );

    if (chance > 60)
    {
        return;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE );

    if (chance > 50)
    {
        return;
    }
    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_DROWNING,TRUE );
    return;
}


void spell_tsunami(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    int chance;
    int dam;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    chance = number_range( 1, 100);

    send_to_char( "You call a massive wave of water!\n\r", ch );
    act( "A massive wave of water flows over you!\n\r", ch, NULL, NULL, TO_ROOM );

/*    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }*/

      for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && !is_safe_spell(ch,vch,TRUE))
	    {
		if ( ( ch->fighting == NULL )
		&& ( !IS_NPC( ch ) ) )
		{
		    ch->attacker = TRUE;
		    vch->attacker = FALSE;
		}
          dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_DROWNING,TRUE );

    dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_DROWNING,TRUE );

    if (chance > 75)
    {
        return;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_DROWNING,TRUE );

    if (chance > 50)
    {
        return;
    }
    dam = dice(ch->level/2, ch->level);
       if (is_same_group(vch,ch) )
                {
                    continue;
                }


  damage( ch, vch, dam, sn, DAM_DROWNING,TRUE );

	    }
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "In the distace you can see a huge wave come in.\n\r", vch );
    }
    return;
}

void spell_blizzard(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;

    send_to_char( "You call down a storm of ice!\n\r", ch );
    act( "$n calls down a storm of ice!\n\r", ch, NULL, NULL, TO_ROOM );

/*    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }*/
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && !is_safe_spell(ch,vch,TRUE))
	    {
		if ( ( ch->fighting == NULL )
		&& ( !IS_NPC( ch ) ) )
		{
		    ch->attacker = TRUE;
		    vch->attacker = FALSE;
		}
   if (is_same_group(vch,ch) )
                {
                    continue;
                }

          dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_COLD,TRUE );

    dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_COLD,TRUE );

    dam = dice(ch->level/2, ch->level)*.9;

  damage( ch, vch, dam, sn, DAM_COLD,TRUE );

    dam = dice(ch->level/2, ch->level)*.6;

  damage( ch, vch, dam, sn, DAM_COLD,TRUE );
	    }
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "In the distace you can see ice swirling around.\n\r", vch );
    }


    return;
}

void spell_freeze(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    send_to_char( "You call down a block of ice from the sky to encase your foe!\n\r", ch );
    act( "$n calls down a block of ice from the sky to encase $s opponent!\n\r", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_COLD,TRUE );

    dam = dice(ch->level/2, ch->level)*.9;

  damage( ch, victim, dam, sn, DAM_COLD,TRUE );

    dam = dice(ch->level/2, ch->level)*.6;

  damage( ch, victim, dam, sn, DAM_COLD,TRUE );
    return;
}

void spell_ice(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    send_to_char( "You call down a small icicle from the sky to strike down your foe!\n\r", ch );
    act( "$n calls down a small icicle to strike down $s foe!\n\r", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }
         if (ch->level < 20)
        ch->mana += 100;

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_COLD,TRUE );

    dam = dice(ch->level/2, ch->level)*.9;

  damage( ch, victim, dam, sn, DAM_COLD,TRUE );
    return;
}

void spell_lightning(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int chance;
    int dam;

    chance = number_range( 1, 100);

    send_to_char( "You call a small thunderbolt down from the heavens!\n\r", ch );
    act( "A small thuunderbolt illuminates the room as it strikes $N!\n\r", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }
         if (ch->level < 20)
        ch->mana += 100;

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_LIGHTNING,TRUE );

    if (chance > 60)
    {
        return;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_LIGHTNING,TRUE );
    return;
}

void spell_bolt(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int chance;
    int dam;

    chance = number_range( 1, 100);

    send_to_char( "You call a massive thunderbolt!\n\r", ch );
    act( "A huge thunderbolt illuminates the room as it strikes down $n's foe!\n\r", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_LIGHTNING,TRUE );

    if (chance > 60)
    {
        return;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_LIGHTNING,TRUE );

    if (chance > 50)
    {
        return;
    }
    dam = dice(ch->level/2, ch->level);

  damage( ch, victim, dam, sn, DAM_LIGHTNING,TRUE );
    return;
}

void spell_thunderstorm(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    int chance;
    int dam;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    chance = number_range( 1, 100);

    send_to_char( "You call an enormous thunderstorm to destroy those around you!\n\r", ch );
    act( "You are being shocked by the thunderstorm that $n has summoned!\n\r", ch, NULL, NULL, TO_ROOM );

/*    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }*/

      for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && !is_safe_spell(ch,vch,TRUE))
	    {
		if ( ( ch->fighting == NULL )
		&& ( !IS_NPC( ch ) ) )
		{
		    ch->attacker = TRUE;
		    vch->attacker = FALSE;
		}
   if (is_same_group(vch,ch) )
                {
                    continue;
                }

          dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_LIGHTNING,TRUE );

    dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_LIGHTNING,TRUE );

    if (chance > 75)
    {
        return;
    }

    dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_LIGHTNING,TRUE );

    if (chance > 50)
    {
        return;
    }
    dam = dice(ch->level/2, ch->level);

  damage( ch, vch, dam, sn, DAM_LIGHTNING,TRUE );

	    }
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "In the distace you can see a thunderstorm light up the surroundings.\n\r", vch );
    }
    return;
}

void spell_quatre(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
     CHAR_DATA *victim = (CHAR_DATA *) vo;
     int dam,manadrain;
     char buf[MSL];

     send_to_char("Oh No! A quarter of your life has been sucked out!\n\r",victim);
     dam=(victim->hit)/4;
     victim->hit*=.75;
        dam_message(ch,victim,dam,sn,FALSE);
     manadrain=ch->mana*.5;
     ch->mana*=.5;

     sprintf(buf,"{^%d{x points of mana have been drained while using quatre.\n\r",manadrain);
     send_to_char(buf,ch);
     return;
}

void spell_demi(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
     CHAR_DATA *victim = (CHAR_DATA *) vo;
     int dam,manadrain;
     char buf[MSL];

     send_to_char("Oh No! Half of your life has been sucked out!\n\r",victim);
        dam=(victim->hit)/2;
        victim->hit*=.5;
        dam_message(ch,victim,dam,sn,FALSE);
        manadrain=ch->mana*.75;
     ch->mana*=.25;
     sprintf(buf,"{^%d{x points of mana have been drained while using demi.\n\r",manadrain);
     send_to_char(buf,ch);
     return;
}

void spell_cure(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int healnum;
    int lev, levbase;

    levbase=ch->level/2;
    lev=ch->level*2;
    healnum = number_range( levbase, lev);
        victim->hit += healnum;

        if (victim->hit > victim->max_hit)
        {
        victim->hit = victim->max_hit;
        }
        if (ch==victim)
        {
        send_to_char("A wave of green envelops you, healing some of your wounds!\n\r",ch);
        return;
        }
        send_to_char("A wave of green envelops you, healing some of your wounds!\n\r",victim);
        act( "A wave of green envelops $N, and some of $s wounds appear to be healed.\n\r", ch, NULL, NULL, TO_NOTVICT );
    return;
}


void spell_cura(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int healnum;
    int lev,level2, healnum2;

    lev=ch->level*6;
    level2=ch->level*4;
    healnum = number_range(level2, lev);
    healnum2= number_range(healnum/2, healnum*2);
        victim->hit += healnum2;

        if (victim->hit > victim->max_hit)
        {
        victim->hit = victim->max_hit;
        }
        if (ch==victim)
        {
        send_to_char("A wave of green envelops you, healing a lot of your wounds!\n\r",ch);
        return;
        }
        send_to_char("A wave of green envelops you, healing a lot of your wounds!\n\r",victim);
        act( "A wave of green envelops $N, and a lot of $s wounds appear to be healed.\n\r", ch, NULL, NULL, TO_NOTVICT );
    return;
}


void spell_curaga(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int healnum;
    int lev, level2,healnum2;

    lev=ch->level*12;
    level2=ch->level*8;
    healnum = number_range(level2, lev);
    healnum2= number_range(healnum, healnum*3);
        victim->hit += healnum2;

        if (victim->hit > victim->max_hit)
        {
        victim->hit = victim->max_hit;
        }
        if (ch==victim)
        {
        send_to_char("A wave of green envelops you, healing tons of your wounds!\n\r",ch);
        return;
        }
        send_to_char("A wave of green envelops you, healing tons of your wounds!\n\r",victim);
        act( "A wave of green envelops $N, and tons of $s wounds appear to be healed.\n\r", ch, NULL, NULL, TO_NOTVICT );
    return;

}

void spell_fulllife(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int diff;

    diff=(victim->max_hit - victim->hit);
        if (ch->mana < diff)
        {
        send_to_char("You don't have enough mana to cast this spell!\n\r",ch);
        return;
        }

        victim->hit += diff;
        ch->mana -= diff;
        if (ch==victim)
        {
        send_to_char("An enormous wave of green envelops you, healing all of your wounds!\n\r",ch);
        return;
        }
        send_to_char("An enormous wave of green envelops you, healing all of your wounds!\n\r",victim);
        act( "A wave of green envelops $N, and all of $s wounds are healed.\n\r", ch, NULL, NULL, TO_NOTVICT );
    return;

}

void spell_regen( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int manaloss;

    manaloss = ch->level * 5;
    if (ch->mana < manaloss)
        {
        send_to_char("But you don't have enough mana to cast this spell!\n\r",ch);
        return;
        }
    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You are already regenerating!\n\r",ch);
	else
	  act("$N is already regenerating!",ch,NULL,victim,TO_CHAR);
	return;
    }
        ch->mana -= manaloss;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/50;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_REGEN;
    affect_to_char( victim, &af );
    act( "$n starts flashing green as $s wounds start continually healing.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You are starting to regenerate!\n\r", victim );
    return;
}

void spell_fire_sword(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    OBJ_DATA *sword;
    AFFECT_DATA *pAf;

    sword = create_object( get_obj_index( OBJ_VNUM_FIRE_SWORD ), 0 );
    sword->timer = level / 2;

    pAf             =   new_affect();
    pAf->location   =   APPLY_HITROLL;
    pAf->modifier   =   (ch->level / 3);
    pAf->where	    =   TO_OBJECT;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =	sword->level;
    pAf->next       =   sword->affected;
    sword->affected  =   pAf;
    sword->level     =   level;

    pAf             =   new_affect();
    pAf->location   =   APPLY_DAMROLL;
    pAf->modifier   =   (ch->level / 3);
    pAf->where	    =   TO_OBJECT;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =	sword->level;
    pAf->next       =   sword->affected;
    sword->affected  =   pAf;
    sword->level     =   level;

    pAf             =   new_affect();
    pAf->location   =   APPLY_HIT;
    pAf->modifier   =   (ch->level * 2);
    pAf->where	    =   TO_OBJECT;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =	sword->level;
    pAf->next       =   sword->affected;
    sword->affected  =   pAf;
    sword->level     =   level;

    pAf             =   new_affect();
    pAf->location   =   APPLY_MANA;
    pAf->modifier   =   (ch->level );
    pAf->where	    =   TO_OBJECT;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =	sword->level;
    pAf->next       =   sword->affected;
    sword->affected  =   pAf;
    sword->level     =   level;

    sword->value[1] = level / 5 - 3;
    sword->value[2] = level / 5 - 4;
    obj_to_char(sword, ch);
    send_to_char("You called your fire sword.\n\r",ch);
    send_to_char("From calling it you feel weak, perhaps you should rest.\n\r",ch);
    act( "$n calls for $s fire sword.", ch, sword, NULL, TO_ROOM );
    ch->hit   *= .75;
    return;
}


