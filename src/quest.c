/****************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com    *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this   *
*  code is allowed provided you add a credit line to the effect of:         *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest     *
*  of the standard diku/rom credits. If you use this or a modified version  *
*  of this code, let me know via email: moongate@moongate.ams.com. Further  *
*  updates will be posted to the rom mailing list. If you'd like to get     *
*  the latest version of quest.c, please send a request to the above add-   *
*  ress. Quest Code v2.03. Please do not remove this notice from this file. *
****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

DECLARE_DO_FUN( do_say		);
DECLARE_DO_FUN( do_token	);
DECLARE_DO_FUN( do_lore		);

/* Object vnums for Quest Rewards */
/*
#define QUEST_ITEM1 3330
#define QUEST_ITEM2 8323
#define QUEST_ITEM3 8324
#define QUEST_ITEM4 17543
#define QUEST_ITEM5 8325
#define QUEST_ITEM6 3331
#define QUEST_ITEM7 3398
#define QUEST_ITEM8 450
#define QUEST_ITEM9 451
#define QUEST_ITEM10 454
#define QUEST_ITEM11 16397
#define QUEST_ITEM12 456
#define QUEST_ITEM13 245
*/
#define QUEST_ITEM1 21251
#define QUEST_ITEM2 21252
#define QUEST_ITEM3 21253
#define QUEST_ITEM4 21254
#define QUEST_ITEM5 21255
#define QUEST_ITEM6 21256
#define QUEST_ITEM7 21257
#define QUEST_ITEM8 21258
#define QUEST_ITEM9 21259
#define QUEST_ITEM10 21260
#define QUEST_ITEM11 21261
#define QUEST_ITEM12 21262
#define QUEST_ITEM13 21263
#define QUEST_ITEM14 21264
#define QUEST_ITEM15 21265
#define QUEST_ITEM16 21266
#define QUEST_ITEM17 21267
#define QUEST_ITEM18 21268

/* Coming soon (?)
 * #define QUEST_ITEM19 21269
 * #define QUEST_ITEM20 21270
 * #define QUEST_ITEM21 21271
 * #define QUEST_ITEM22 21272
 * #define QUEST_ITEM23 21273
 * #define QUEST_ITEM24 21274
 * #define QUEST_ITEM25 21275
 * #define QUEST_ITEM26 21276
 * #define QUEST_ITEM27 21277
 * #define QUEST_ITEM28 21278
 * #define QUEST_ITEM29 21279
 * #define QUEST_ITEM30 21280
 */

/*
 * Get these
 */
#define QUEST_OBJQUEST1 8326
#define QUEST_OBJQUEST2 8327
#define QUEST_OBJQUEST3 8328

/* Local functions */

void generate_quest     args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void quest_update       args(( void ));
bool quest_level_diff   args(( int clevel, int mlevel));
bool chance             args(( int num ));
ROOM_INDEX_DATA         *find_location( CHAR_DATA *ch, char *arg );

/* CHANCE function. I use this everywhere in my code, very handy :> */

bool chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}

/* The main quest function */

void do_qwest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questman;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    AFFECT_DATA *paf;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
        send_to_char("QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY QUIT.\n\r",ch);
        send_to_char("For more information, type 'HELP QUEST'.\n\r",ch);
        return;
    }
    if (!strcmp(arg1, "info"))
    {
        if (IS_SET(ch->act, PLR_QUESTOR))
        {
            if (ch->questmob == -1 && ch->questgiver->short_descr != NULL)
            {
                sprintf(buf, "{MYour quest is ALMOST complete!{x\n\rGet back to %s before your time runs out!\n\r",ch->questgiver->short_descr);
                send_to_char(buf, ch);
            }
            else if (ch->questobj > 0)
            {
                questinfoobj = get_obj_index(ch->questobj);
                if (questinfoobj != NULL)
                {
                    sprintf(buf, "You are on a quest to recover the fabled %s!\n\r",questinfoobj->name);
                    send_to_char(buf, ch);
                }
                else send_to_char("You aren't currently on a quest.\n\r",ch);
                return;
            }
            else if (ch->questmob > 0)
            {
                questinfo = get_mob_index(ch->questmob);
                if (questinfo != NULL)
                {
                    sprintf(buf, "You are on a quest to slay the dreaded %s!\n\r",questinfo->short_descr);
                    send_to_char(buf, ch);
                }
                else send_to_char("You aren't currently on a quest.\n\r",ch);
                return;
            }
        }
        else
            send_to_char("You aren't currently on a quest.\n\r",ch);
        return;
    }


    if (!strcmp(arg1, "points"))
    {
        sprintf(buf, "You have %ld quest points.\n\r",ch->qps);
        send_to_char(buf, ch);
        return;
    }


    else if (!strcmp(arg1, "time"))
    {
        if (!IS_SET(ch->act, PLR_QUESTOR))
        {
            send_to_char("You aren't currently on a quest.\n\r",ch);
            if (ch->nextquest > 1)
            {
                sprintf(buf, "There are %d minutes remaining until you can go on another quest.\n\r",ch->nextquest);
                send_to_char(buf, ch);
            }
            else if (ch->nextquest == 1)
            {
                sprintf(buf, "There is less than a minute remaining until you can go on another quest.\n\r");
                send_to_char(buf, ch);
            }
        }
        else if (ch->countdown > 0)
        {
            sprintf(buf, "Time left for current quest: %d\n\r",ch->countdown);
            send_to_char(buf, ch);
        }
        return;
    }

/* Checks for a character in the room with spec_questmaster set. This special
   procedure must be defined in special.c. You could instead use an
   ACT_QUESTMASTER flag instead of a special procedure. */

    for ( questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room )
    {
        if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup( "spec_questmaster" )) break;
    }

    if (questman == NULL || questman->spec_fun != spec_lookup( "spec_questmaster" ))
    {
        send_to_char("You can't do that here.\n\r",ch);
        return;
    }

    if ( questman->fighting != NULL)
    {
        send_to_char("Wait until the fighting stops.\n\r",ch);
        return;
    }

    ch->questgiver = questman;

/* And, of course, you will need to change the following lines for YOUR
   quest item information. Quest items on Moongate are unbalanced, very
   very nice items, and no one has one yet, because it takes awhile to
   build up quest points :> Make the item worth their while. */

    if (!strcmp(arg1, "list"))
    {
        act( "$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM);
        act ("You ask $N for a list of quest items.",ch, NULL, questman, TO_CHAR);
        sprintf(buf, "Current Quest Items available for Purchase:\n\r\
{w[{cCost{w]{x          {w[{cName{w]{x                          {R[{rLvl{R]{x\n\r\
{c15000{x QPs.......'{MA{5m{!u{1l{#e{3t {2of {^P{6e{^r{6m{^a{6n{^e{6n{^t {&S{7a{*n{7c{&t{7u{8a{7r{&y{0' {![{1101{!]{0\n\r\
{c10000{x QPs.......'{1P{!o{1l{!e{1a{!r{1m{0 of {^D{&e{^s{&t{^i{&n{^y{0' 		{![{1101{!]{0\n\r\
{c9250{x  QPs.......'the {RS{rk{wu{Dl{wl{rs{Rp{rl{wi{Dt{wt{ri{Rn{rg {wM{Da{wc{re{x'       {![{1101{!]{x\n\r\
{c9000{x  QPs.......'{#W{3a{#r{3d{#r{3o{#f{3'{Ys {!S{5hort {!S{5word{0' 	{R[{r101{R]{0\n\r\
{c9000{x  QPs.......'{yW{wi{Dn{wd{yf{wo{Dr{wc{ye{x'                     {R[{r101{R]{x\n\r\
{c8750{x  QPs.......'{cFla{wil o{bf {gt{yh{re {GE{Bl{Ye{Wm{Re{Dn{Mt{Cs{x'         {![{1101{!]{x\n\r\
{c8500{x  QPs.......'{CSt{caf{wf O{Df S{wtr{ciki{Cng{x'             {![{1101{!]{x\n\r\
{c8000{x  QPs.......'{1D{2a{3gg{2e{1r {6o{7f {6t{7h{6e {^W{&in{^d'            {![{1101{!]{0\n\r\
{c7500{x  QPs.......'{^Cr{7y{&s{7t{^al {^R{&in{^g{0' 			{![{1101{!]{0\n\r\
{c7000{x  QPs.......'{1A{3x{1e {3o{1f {3t{1h{3e {1A{3n{1c{3i{1e{3n{1t{3s{0'		{![{1101{!]{0\n\r\
{c6500{x  QPs.......'{6The {&A{7rmor {2of {!F{Yi{rr{ye{ch{Be{bd{0' 		{![{1101{!]{0\n\r\
{c5000{x  QPs.......'{WSt{win{Dg{cin{Dg {wWh{Wip'                 {![{1101{!]{x\n\r\
{c4500{x  QPs.......'{MA{5m{!u{1l{&e{7t {*of {7D{&e{1t{!e{5c{Mt{2i{@o{^n{0' 		{![{1101{!]{0\n\r\
{c3500{x  QPs.......'a D{2e{@a{2d{@l{2y {$H{4u{$n{4t{$i{4n{$g {#H{3a{#w{3k{0' 	{![{1101{!]{0\n\r\
{c3000{x  QPs.......'{3L{1e{3a{1t{3h{1e{3r {7Boots{0'			{![{195{!]{0\n\r\
{c2750{x  QPs.......'{MM{^a{Ms{^k{0 of {!T{@y{#m{$e'			{![{195{!]{0\n\r\
{c2500{x  QPs.......'{!H{1a{3n{#d{3f{1u{!l {4of {^Icy {#F{3l{1a{!m{3e{#s{0'		{![{145{!]{0\n\r\
{c50{x    QPs.......'{$Spe{!llu{^p P{&ill{0' 			{![{1any{!]{0\n\r\
{c2000{x  QPs.......Forgive Your Wrong Doings	{![{1any{!]{0\n\r\
{c500{x   QPs.......100 Platinum pieces 		{![{1any{!]{0\n\r\
{c400{x   QPs.......1 Practice 			{![{1any{!]{0\n\r\
To buy an item, type 'QUEST BUY <item>'.\n\r");
        send_to_char(buf, ch);
        return;
    }

    else if (!strcmp(arg1, "buy"))
    {
        if (arg2[0] == '\0')
        {
            send_to_char("To buy an item, type 'QUEST BUY <item>'.\n\r",ch);
            return;
        }
        if (is_name(arg2, "ring crystal"))
        {
            if (ch->qps >= 7500)
            {
                ch->qps -= 7500;
                obj = create_object(get_obj_index(QUEST_ITEM1),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

	else if (is_name(arg2, "mask tyme"))
        {
            if (ch->qps >= 2750)
            {
                ch->qps -= 2750;
                obj = create_object(get_obj_index(QUEST_ITEM10),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

	else if (is_name(arg2, "staff striking"))
        {
            if (ch->qps >= 8500)
            {
                ch->qps -= 8500;
                obj = create_object(get_obj_index(QUEST_ITEM14),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }
	else if (is_name(arg2, "skull splitting skullsplitting mace"))
        {
            if (ch->qps >= 9250)
            {
                ch->qps -= 9250;
                obj = create_object(get_obj_index(QUEST_ITEM16),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }
	else if (is_name(arg2, "flail elements"))
        {
            if (ch->qps >= 8750)
            {
                ch->qps -= 8750;
                obj = create_object(get_obj_index(QUEST_ITEM17),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

	else if (is_name(arg2, "stinging whip"))
        {
            if (ch->qps >= 5000)
            {
                ch->qps -= 5000;
                obj = create_object(get_obj_index(QUEST_ITEM15),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

        else if (is_name(arg2, "short sword wardrof"))
        {
            if (ch->qps >= 9000)
            {
                ch->qps -= 9000;
                obj = create_object(get_obj_index(QUEST_ITEM2),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

        else if (is_name(arg2, "windforce wind force"))
        {
            if (ch->qps >= 9000)
            {
                ch->qps -= 9000;
                obj = create_object(get_obj_index(QUEST_ITEM18),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

	else if (is_name(arg2, "light handful icy flames"))
        {
            if (ch->qps >= 2500)
            {
                ch->qps -= 2500;
                obj = create_object(get_obj_index(QUEST_ITEM11),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }
	else if (is_name(arg2, "dagger wind"))
        {
            if (ch->qps >= 8000)
            {
                ch->qps -= 8000;
                obj = create_object(get_obj_index(QUEST_ITEM13),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

	else if (is_name(arg2, "amulet permanent sanctuary"))
        {
            if (ch->qps >= 15000)
            {
                ch->qps -= 15000;
                obj = create_object(get_obj_index(QUEST_ITEM7),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }
        else if (is_name(arg2, "detection"))
        {
            if (ch->qps >= 4500)
            {
                ch->qps -= 4500;
                obj = create_object(get_obj_index(QUEST_ITEM3),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

        else if (is_name(arg2, "armor firehed"))
        {
            if (ch->qps >= 6500)
            {
                ch->qps -= 6500;
                obj = create_object(get_obj_index(QUEST_ITEM4),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }
	else if (is_name(arg2, "spellup pill"))
        {
            if (ch->qps >= 50)
            {
                ch->qps -= 50;
                obj = create_object(get_obj_index(QUEST_ITEM5),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

	else if (is_name(arg2, "axe ancients"))
        {
            if (ch->qps >= 7000)
            {
                ch->qps -= 7000;
                obj = create_object(get_obj_index(QUEST_ITEM9),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }


        else if (is_name(arg2, "deadly hawk helmet hunter"))
        {
            if (ch->qps >= 3500)
            {
                ch->qps -= 3500;
                obj = create_object(get_obj_index(QUEST_ITEM6),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

        else if (is_name(arg2, "polearm destiny"))
        {
            if (ch->qps >= 10000)
            {
                ch->qps -= 10000;
                obj = create_object(get_obj_index(QUEST_ITEM8),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }

	else if (is_name(arg2, "leather boots"))
        {
            if (ch->qps >= 3000)
            {
                ch->qps -= 3000;
                obj = create_object(get_obj_index(QUEST_ITEM12),ch->level);
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }


        else if (is_name(arg2, "practices pracs prac practice"))
        {
            if (ch->qps >= 400)
            {
                ch->qps -= 400;
                ch->practice += 1;
                act( "$N gives 1 practice to $n.", ch, NULL, questman, TO_ROOM );
                act( "$N gives you 1 practice.",   ch, NULL, questman, TO_CHAR );
                return;
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }




        else if (is_name(arg2, "forgive"))
        {
            if (ch->qps >= 2000)
            {
                ch->qps -= 2000;
                ch->pcdata->pkkills -= 1;
                act( "$N clears up some of $n's debt to society.", ch, NULL, questman, TO_ROOM );
                act( "$N clears up some of your debt to society.",   ch, NULL, questman, TO_CHAR );
                return;
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }





        else if (is_name(arg2, "100 plat platinum gold money gp"))
        {
            if (ch->qps >= 500)
            {
                ch->qps -= 500;
                ch->pcdata->bank += 100;
                act( "$N gives 100 Platinum pieces to $n.", ch, NULL, questman, TO_ROOM );
                act( "$N has 100 platinum transfered from $s Swiss account to your balance.",   ch, NULL, questman, TO_CHAR );
                return;
            }
            else
            {
                sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
                do_say(questman,buf);
                return;
            }
        }
        else
        {
            sprintf(buf, "I don't have that item, %s.",ch->name);
            do_say(questman, buf);
        }
        if (obj != NULL)
        {
            act( "$N gives $p to $n.", ch, obj, questman, TO_ROOM );
            act( "$N gives you $p.",   ch, obj, questman, TO_CHAR );
            obj_to_char(obj, ch);
        }
        return;
    }
    else if (!strcmp(arg1, "request"))
    {
        act( "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
        act ("You ask $N for a quest.",ch, NULL, questman, TO_CHAR);
        if (IS_SET(ch->act, PLR_QUESTOR))
        {
            sprintf(buf, "But you're already on a quest!");
            do_say(questman, buf);
            return;
        }
        if (ch->nextquest > 0)
        {
            sprintf(buf, "You're very brave, %s, but let someone else have a chance.",ch->name);
            do_say(questman, buf);
            sprintf(buf, "Come back later.");
            do_say(questman, buf);
            return;
        }

        sprintf(buf, "Thank you, brave %s!",ch->name);
        do_say(questman, buf);
        ch->questmob = 0;
        ch->questobj = 0;

        generate_quest(ch, questman);

        if (ch->questmob > 0 || ch->questobj > 0)
        {
            ch->countdown = number_range(10,30);
            SET_BIT(ch->act, PLR_QUESTOR);
            sprintf(buf, "You have %d minutes to complete this quest.",ch->countdown);
            do_say(questman, buf);
            sprintf(buf, "May the gods go with you!");
            do_say(questman, buf);
        }
        return;
    }


    else if (!strcmp(arg1, "identify"))
{
    if (is_name(arg2, "ring crystal"))
        obj = create_object(get_obj_index(QUEST_ITEM1),ch->level);
    else if (is_name(arg2, "mask tyme"))
        obj = create_object(get_obj_index(QUEST_ITEM10),ch->level);
    else if (is_name(arg2, "staff striking"))
        obj = create_object(get_obj_index(QUEST_ITEM14),ch->level);
    else if (is_name(arg2, "skull splitting skullsplitting mace"))
        obj = create_object(get_obj_index(QUEST_ITEM16),ch->level);
    else if (is_name(arg2, "flail elements"))
        obj = create_object(get_obj_index(QUEST_ITEM17),ch->level);
    else if (is_name(arg2, "stinging whip"))
        obj = create_object(get_obj_index(QUEST_ITEM15),ch->level);
    else if (is_name(arg2, "short sword wardrof"))
        obj = create_object(get_obj_index(QUEST_ITEM2),ch->level);
    else if (is_name(arg2, "windforce force"))
        obj = create_object(get_obj_index(QUEST_ITEM18),ch->level);
    else if (is_name(arg2, "light handful icy flames"))
        obj = create_object(get_obj_index(QUEST_ITEM11),ch->level);
    else if (is_name(arg2, "dagger wind"))
        obj = create_object(get_obj_index(QUEST_ITEM13),ch->level);
    else if (is_name(arg2, "amulet permanent sanctuary"))
        obj = create_object(get_obj_index(QUEST_ITEM7),ch->level);
    else if(is_name(arg2, "detection"))
        obj = create_object(get_obj_index(QUEST_ITEM3),ch->level);
    else if (is_name(arg2, "armor firehed"))
        obj = create_object(get_obj_index(QUEST_ITEM4),ch->level);
    else if (is_name(arg2, "spellup pill"))
        obj = create_object(get_obj_index(QUEST_ITEM5),ch->level);
    else if (is_name(arg2, "axe ancients"))
        obj = create_object(get_obj_index(QUEST_ITEM9),ch->level);
    else if (is_name(arg2, "deadly hawk helmet hunter"))
        obj = create_object(get_obj_index(QUEST_ITEM6),ch->level);
    else if (is_name(arg2, "polearm destiny"))
        obj = create_object(get_obj_index(QUEST_ITEM8),ch->level);
    else if (is_name(arg2, "leather boots"))
        obj = create_object(get_obj_index(QUEST_ITEM12),ch->level);
    else
    {
        send_to_char("That is not a valid name.\n\r",ch);
        return;
    }
    sprintf( buf, "Name(s): %s\n\r",
	obj->name );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s\n\rLong description: %s\n\r",
        obj->short_descr, obj->description );
    send_to_char( buf, ch );

    sprintf( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
        wear_bit_name(obj->wear_flags), extra_bit_name( obj->extra_flags ) );
    send_to_char( buf, ch );

/*    sprintf( buf, "Weight: %d/%d/%d (10th pounds)\n\r",
        obj->weight / 10);
    send_to_char( buf, ch ); */

    sprintf( buf, "Level: %d  Cost: %d\n\r",
        obj->level, obj->cost);
    send_to_char( buf, ch );

    /* now give out vital statistics as per identify */

    switch ( obj->item_type )
    {
        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
            sprintf( buf, "Level %d spells of:", obj->value[0] );
            send_to_char( buf, ch );

            if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
            {
                send_to_char( " '", ch );
                send_to_char( skill_table[obj->value[1]].name, ch );
                send_to_char( "'", ch );
            }

            if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
            {
                send_to_char( " '", ch );
                send_to_char( skill_table[obj->value[2]].name, ch );
                send_to_char( "'", ch );
            }

            if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
            {
                send_to_char( " '", ch );
                send_to_char( skill_table[obj->value[3]].name, ch );
                send_to_char( "'", ch );
            }

            if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
            {
                send_to_char(" '",ch);
                send_to_char(skill_table[obj->value[4]].name,ch);
                send_to_char("'",ch);
            }

            send_to_char( ".\n\r", ch );
        break;

	case ITEM_TOKEN:
	    sprintf( buf, "Worth{M %d{x Quest Points!", obj->value[0] );
		send_to_char( buf, ch );
		break;

        case ITEM_WAND:
        case ITEM_STAFF:
            sprintf( buf, "Has %d(%d) charges of level %d",
                obj->value[1], obj->value[2], obj->value[0] );
            send_to_char( buf, ch );

            if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
            {
                send_to_char( " '", ch );
                send_to_char( skill_table[obj->value[3]].name, ch );
                send_to_char( "'", ch );
            }

            send_to_char( ".\n\r", ch );
        break;

        case ITEM_DRINK_CON:
            sprintf(buf,"It holds %s-colored %s.\n\r",
                liq_table[obj->value[2]].liq_color,
                liq_table[obj->value[2]].liq_name);
            send_to_char(buf,ch);
            break;


        case ITEM_WEAPON:
            send_to_char("Weapon type is ",ch);
            switch (obj->value[0])
            {
                case(WEAPON_EXOTIC):
                    send_to_char("exotic\n\r",ch);
                    break;
                case(WEAPON_SWORD):
                    send_to_char("sword\n\r",ch);
                    break;
                case(WEAPON_DAGGER):
                    send_to_char("dagger\n\r",ch);
                    break;
                case(WEAPON_STAFF):
                    send_to_char("spear/staff\n\r",ch);
                    break;
                case(WEAPON_MACE):
                    send_to_char("mace/club\n\r",ch);
                    break;
                case(WEAPON_AXE):
                    send_to_char("axe\n\r",ch);
                    break;
                case(WEAPON_FLAIL):
                    send_to_char("flail\n\r",ch);
                    break;
                case(WEAPON_WHIP):
                    send_to_char("whip\n\r",ch);
                    break;
                case(WEAPON_POLEARM):
                    send_to_char("polearm\n\r",ch);
                    break;
                case(WEAPON_BOW):
                    send_to_char("bow\n\r",ch);
                    break;
                default:
                    send_to_char("unknown\n\r",ch);
                    break;
            }
            if (obj->pIndexData->new_format)
                sprintf(buf,"Damage is %dd%d (average %d)\n\r",
                    obj->value[1],obj->value[2],
                    (1 + obj->value[2]) * obj->value[1] / 2);
            else
                sprintf( buf, "Damage is %d to %d (average %d)\n\r",
                    obj->value[1], obj->value[2],
                    ( obj->value[1] + obj->value[2] ) / 2 );
            send_to_char( buf, ch );

            sprintf(buf,"Damage type is %s.\n\r",
                (obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE) ?
                    attack_table[obj->value[3]].noun : "undefined");
            send_to_char(buf,ch);

            if (obj->value[4])  /* weapon flags */
            {
                sprintf(buf,"Weapons flags: %s\n\r",
                    weapon_bit_name(obj->value[4]));
                send_to_char(buf,ch);
            }
        break;

        case ITEM_ARMOR:
            sprintf( buf,
            "Defense is %d.\n\r",
                obj->value[0]);
            send_to_char( buf, ch );
        break;

        case ITEM_QUIVER:
            sprintf( buf,
            "Adds bonuses of %dd%d, and holds %d/%d arrows\n\r",
                obj->value[1], obj->value[2], obj->value[0], obj->value[3] );
            send_to_char( buf, ch );
        break;

        case ITEM_CONTAINER:
            sprintf(buf,"Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
                obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
            send_to_char(buf,ch);
            if (obj->value[4] != 100)
            {
                sprintf(buf,"Weight multiplier: %d%%\n\r",
                    obj->value[4]);
                send_to_char(buf,ch);
            }
        break;
    }


    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
        EXTRA_DESCR_DATA *ed;

        send_to_char( "Extra description keywords: '", ch );

        for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
        {
            send_to_char( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char( " ", ch );
        }

        for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
        {
            send_to_char( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char( " ", ch );
        }

        send_to_char( "'\n\r", ch );
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf, "Affects %s by %d, level %d",
            affect_loc_name( paf->location ), paf->modifier,paf->level );
        send_to_char(buf,ch);
        if ( paf->duration > -1)
            sprintf(buf,", %d hours.\n\r",paf->duration);
        else
            sprintf(buf,".\n\r");
        send_to_char( buf, ch );
        if (paf->bitvector)
        {
            switch(paf->where)
            {
                case TO_AFFECTS:
                    sprintf(buf,"Adds %s affect.\n",
                        affect_bit_name(paf->bitvector));
                    break;
                case TO_WEAPON:
                    sprintf(buf,"Adds %s weapon flags.\n",
                        weapon_bit_name(paf->bitvector));
                    break;
                case TO_OBJECT:
                    sprintf(buf,"Adds %s object flag.\n",
                        extra_bit_name(paf->bitvector));
                    break;
                case TO_IMMUNE:
                    sprintf(buf,"Adds immunity to %s.\n",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_RESIST:
                    sprintf(buf,"Adds resistance to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_VULN:
                    sprintf(buf,"Adds vulnerability to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
		case TO_SHIELDS:
		    sprintf(buf,"Adds shield %s.\n\r",
			shield_bit_name(paf->bitvector));
		    break;
                default:
                    sprintf(buf,"Unknown bit %d: %d\n\r",
                        paf->where,paf->bitvector);
                    break;
            }
            send_to_char(buf,ch);
        }
    }

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf, "Affects %s by %d, level %d.\n\r",
            affect_loc_name( paf->location ), paf->modifier,paf->level );
        send_to_char( buf, ch );
        if (paf->bitvector)
        {
            switch(paf->where)
            {
                case TO_AFFECTS:
                    sprintf(buf,"Adds %s affect.\n",
                        affect_bit_name(paf->bitvector));
                    break;
                case TO_OBJECT:
                    sprintf(buf,"Adds %s object flag.\n",
                        extra_bit_name(paf->bitvector));
                    break;
                case TO_IMMUNE:
                    sprintf(buf,"Adds immunity to %s.\n",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_RESIST:
                    sprintf(buf,"Adds resistance to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_VULN:
                    sprintf(buf,"Adds vulnerability to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                 case TO_SHIELDS:
                    sprintf(buf,"Adds %s shield.\n",
                        shield_bit_name(paf->bitvector));
                    break;
		default:
                    sprintf(buf,"Unknown bit %d: %d\n\r",
                        paf->where,paf->bitvector);
                    break;
            }
            send_to_char(buf,ch);
        }
    }

    extract_obj(obj);
    return;
}


    else if (!strcmp(arg1, "complete"))
    {
        act( "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM);
        act ("You inform $N you have completed $s quest.",ch, NULL, questman, TO_CHAR);
        if (ch->questgiver != questman)
        {
            sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
            do_say(questman,buf);
            return;
        }

        if (IS_SET(ch->act, PLR_QUESTOR))
        {
            if (ch->questmob == -1 && ch->countdown > 0)
            {
                int reward, pointreward, nexttime;

                reward = number_range(20,60);
                pointreward = (ch->level/number_range(5,15))*(ch->pcdata->tier + 1); //(first tier is '0' etc)
                if (pointreward < 10)
                pointreward = 10;

                sprintf(buf, "Congratulations on completing your quest!");
                do_say(questman,buf);
                sprintf(buf,"As a reward, I am giving you %d quest points, and %d platinum.",pointreward,reward);
                do_say(questman,buf);
                if (chance(3))
                {
                    send_to_char("You gain a practice!\n\r",ch);
                    ch->practice ++;
                }
                nexttime = 0;
                REMOVE_BIT(ch->act, PLR_QUESTOR);
                ch->questgiver = NULL;
                ch->countdown = 0;
                ch->questmob = 0;
                ch->questobj = 0;
                ch->nextquest = nexttime;
                ch->platinum += reward;
                ch->qps += pointreward;

                return;
            }
            else if (ch->questobj > 0 && ch->countdown > 0)
            {
                bool obj_found = FALSE;

                for (obj = ch->carrying; obj != NULL; obj= obj_next)
                {
                    obj_next = obj->next_content;

                    if (obj != NULL && obj->pIndexData->vnum == ch->questobj)
                    {
                        obj_found = TRUE;
                        break;
                    }
                }
                if (obj_found == TRUE)
                {
                    int reward, pointreward, pracreward, nexttime;

                    reward = number_range(25,65);
                    pointreward = (ch->level/number_range(5,15))*(ch->pcdata->tier + 1); //(tier 1 is '0' etc)
                    if (pointreward < 10)
                    pointreward = 10;

                    act("You hand $p to $N.",ch, obj, questman, TO_CHAR);
                    act("$n hands $p to $N.",ch, obj, questman, TO_ROOM);

                    sprintf(buf, "Congratulations on completing your quest!");
                    do_say(questman,buf);
                    sprintf(buf,"As a reward, I am giving you %d quest points, and %d platinum.",pointreward,reward);
                    do_say(questman,buf);
                    if (chance(5))
                    {
                        pracreward = number_range(1,2);
                        sprintf(buf, "You gain %d practice(s)!\n\r",pracreward);
                        send_to_char(buf, ch);
                        ch->practice += pracreward;
                    }
                    nexttime = 0;
                    REMOVE_BIT(ch->act, PLR_QUESTOR);
                    ch->questgiver = NULL;
                    ch->countdown = 0;
                    ch->questmob = 0;
                    ch->questobj = 0;
                    ch->nextquest = nexttime;
                    ch->platinum += reward;
                    ch->qps += pointreward;
                    extract_obj(obj);
                    return;
                }
                else
                {
                    sprintf(buf, "You haven't completed the quest yet, but there is still time!");
                    do_say(questman, buf);
                    return;
                }
                return;
            }
            else if ((ch->questmob > 0 || ch->questobj > 0) && ch->countdown >0)
            {
                sprintf(buf, "You haven't completed the quest yet, but there is still time!");
                do_say(questman, buf);
                return;
            }
        }
        if (ch->nextquest > 0)
            sprintf(buf,"But you didn't complete your quest in time!");
        else sprintf(buf, "You have to REQUEST a quest first, %s.",ch->name);
        do_say(questman, buf);
        return;
    }

    else if (!strcmp(arg1, "quit"))
    {
        act("$n informs $N $e wishes to quit $s quest.", ch, NULL,questman,TO_ROOM);
	act ("You inform $N you wish to quit $s quest.",ch, NULL, questman,TO_CHAR);
	if (ch->questgiver != questman)
	{
	    sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
	    do_say(questman,buf);
	    return;
	}

if (IS_SET(ch->act,PLR_QUESTOR))
	{
            REMOVE_BIT(ch->act,PLR_QUESTOR);
            ch->questgiver = NULL;
            ch->countdown = 0;
            ch->questmob = 0;
	    ch->questobj = 0;
            ch->nextquest = 30; 
	    sprintf(buf, "Your quest is over, but for your cowardly behavior, you may not quest again for 30 minutes.");
	    do_say(questman,buf);
		    return;
        }
	else
	{
	send_to_char("You aren't on a quest!",ch);
	return;
	}
    }
    send_to_char("QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY QUIT.\n\r",ch);
    send_to_char("For more information, type 'HELP QUEST'.\n\r",ch);
    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *questitem;
    char buf [MAX_STRING_LENGTH];

    /*  Randomly selects a mob from the world mob list. If you don't
        want a mob to be selected, make sure it is immune to summon.
        Or, you could add a new mob flag called ACT_NOQUEST. The mob
        is selected for both mob and obj quests, even tho in the obj
        quest the mob is not used. This is done to assure the level
        of difficulty for the area isn't too great for the player. */

    for (victim = char_list; victim != NULL; victim = victim->next)
    {
        if (!IS_NPC(victim)) continue;

        if (quest_level_diff(ch->level, victim->level) == TRUE
            && !IS_SET(victim->imm_flags, IMM_SUMMON)
            && victim->pIndexData != NULL
            && victim->pIndexData->pShop == NULL
            && !IS_SET(victim->act, ACT_PET)
            && !IS_SET(victim->affected_by, AFF_CHARM)
	    && !IS_SET(victim->in_room->room_flags, ROOM_NOQUEST)
	    && !IS_SET(victim->in_room->room_flags, ROOM_SAFE)
            && chance(15)) break;
    }

    if ( victim == NULL  )
    {
        do_say(questman, "I'm sorry, but I don't have any quests for you at this time.");
        do_say(questman, "Try again later.");
        ch->nextquest = 1;
        ch->nextquest = 1;
        return;
    }

    if ( ( room = find_location( ch, victim->name ) ) == NULL )
    {
        sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
        do_say(questman, buf);
        sprintf(buf, "Try again later.");
        do_say(questman, buf);
        ch->nextquest = 1;
        return;
    }

    /*  40% chance it will send the player on a 'recover item' quest. */

    if (chance(40))
    {
        int objvnum = 0;

        switch(number_range(0,2))
        {
            case 0:
            objvnum = QUEST_OBJQUEST1;
            break;

            case 1:
            objvnum = QUEST_OBJQUEST2;
            break;

            case 2:
            objvnum = QUEST_OBJQUEST3;
            break;

        }

        questitem = create_object( get_obj_index(objvnum), ch->level );
        obj_to_room(questitem, room);
        ch->questobj = questitem->pIndexData->vnum;

        sprintf(buf, "Vile pilferers have stolen %s from the royal treasury!",questitem->short_descr);
        do_say(questman, buf);
        do_say(questman, "My court wizardess, with her magic mirror, has pinpointed its location.");

        /* I changed my area names so that they have just the name of the area
           and none of the level stuff. You may want to comment these next two
           lines. - Vassago */

        sprintf(buf, "Look in the general area of %s for %s!",room->area->name, room->name);
        do_say(questman, buf);
        return;
    }

    /* Quest to kill a mob */

    else
    {
    switch(number_range(0,1))
    {
        case 0:
        sprintf(buf, "An enemy of mine, %s, is making vile threats against the crown.",victim->short_descr);
        do_say(questman, buf);
        sprintf(buf, "This threat must be eliminated!");
        do_say(questman, buf);
        break;

        case 1:
        sprintf(buf, "Rune's most heinous criminal, %s, has escaped from the dungeon!",victim->short_descr);
        do_say(questman, buf);
        sprintf(buf, "Since the escape, %s has murdered %d civillians!",victim->short_descr, number_range(2,20));
        do_say(questman, buf);
        do_say(questman,"The penalty for this crime is death, and you are to deliver the sentence!");
        break;
    }

    if (room->name != NULL)
    {
        sprintf(buf, "Seek %s out somewhere in the vicinity of %s!",victim->short_descr,room->name);
        do_say(questman, buf);

        /* I changed my area names so that they have just the name of the area
           and none of the level stuff. You may want to comment these next two
           lines. - Vassago */

        sprintf(buf, "That location is in the general area of %s.",room->area->name);
        do_say(questman, buf);
    }
    ch->questmob = victim->pIndexData->vnum;
    }
    return;
}

/* Level differences to search for. Moongate has 350
   levels, so you will want to tweak these greater or
   less than statements for yourself. - Vassago */

bool quest_level_diff(int clevel, int mlevel)
{
    if (clevel > 0 && clevel < 10 && mlevel > 0 && mlevel < 10) return TRUE;
    else if (clevel > 9 && clevel < 102 && mlevel > 9 && mlevel < 102) return TRUE;
    else if (clevel > 101 && mlevel > 0) return TRUE;
    else return FALSE;
}
/*                
 *   Original Level Crap
 *   else if (clevel > 0 && clevel < 20 && mlevel < 30) return TRUE;
 *   else if (clevel > 19 && clevel < 40 && mlevel > 29 && mlevel < 45) return TRUE;
 *   else if (clevel > 39 && clevel < 70 && mlevel > 44 && mlevel < 80) return TRUE;
 *   else if (clevel > 69 && clevel  < 90 && mlevel > 79 && mlevel < 90) return TRUE;
 *   else if (clevel > 89 && clevel < 102 && mlevel > 89 && mlevel <100) return TRUE;
 */

/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {

        ch = d->character;

        if (ch->nextquest > 0)
        {
            ch->nextquest--;
            if (ch->nextquest == 0)
            {
                send_to_char("You may now quest again.\n\r",ch);
                return;
            }
        }
        else if (IS_SET(ch->act,PLR_QUESTOR))
        {
            if (--ch->countdown <= 0)
            {
                ch->nextquest = 30;
                send_to_char("You have run out of time for your quest!\n\rYou may quest again in 30 minutes.\n\r",ch);
                REMOVE_BIT(ch->act, PLR_QUESTOR);
                ch->questgiver = NULL;
                ch->countdown = 0;
                ch->questmob = 0;
            }
            if (ch->countdown > 0 && ch->countdown < 6)
            {
                send_to_char("Better hurry, you're almost out of time for your quest!\n\r",ch);
                return;
            }
        }
        }
    }
    return;
}

void do_token ( CHAR_DATA *ch, char *argument )
{

   OBJ_DATA *obj=NULL;   
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_STRING_LENGTH];
   int value;

   one_argument( argument, arg );

   if (IS_NPC(ch))
        return;

   if (arg[0] == '\0')
   {
        send_to_char("How many QPs should the token be worth?\n\r", ch);
        return;
   }
             
   if (ch->qps <= 0 )
   {
        send_to_char("You have no QPs left to spend!\n\r", ch);
        return;
   }
         
   else  
   { 
        value = atoi(arg);
        if (value <= 0 )
        {
           send_to_char("Tokens may only by made for positive amounts.\n\r", ch);
           return;
        }
   
        else if (value > ch->qps )
        {
           send_to_char("You don't have that many quest points.\n\r",ch);
           return;
        }

        else
        {
           obj = create_object(get_obj_index(OBJ_VNUM_TOKEN), 0);
                sprintf( buf, obj->short_descr, value );
                free_string( obj->short_descr );
                obj->short_descr = str_dup( buf );
        
                sprintf( buf, obj->description, value );
                free_string( obj->description );
                obj->description = str_dup( buf );
    
           obj->value[0]   =    value;
           ch->qps -=   value;
           obj_to_char(obj,ch);
           act( "You snap your fingers and produce a quest token!", ch, NULL, NULL,TO_CHAR);
           act( "$n snaps $s fingers and produces a quest token!", ch, NULL, NULL, TO_ROOM);
           return;
        }
   }
   return;
}

void do_qp ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   one_argument( argument, arg );

   if (IS_NPC(ch))
        return;

   if (arg[0] != '\0')
   {
        send_to_char("No argument needed.\n\r", ch);
        return;
   }

    do_qwest( ch, "points" );
        return;
}
void do_qi ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   one_argument( argument, arg );

   if (IS_NPC(ch))
        return;

   if (arg[0] != '\0')
   {
        send_to_char("No argument needed.\n\r", ch);
        return;
   }

    do_qwest( ch, "info" );
        return;
}
void do_qt ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   one_argument( argument, arg );

   if (IS_NPC(ch))
        return;

   if (arg[0] != '\0')
   {
        send_to_char("No argument needed.\n\r", ch);
        return;
   }

    do_qwest( ch, "time" );
        return;
}
void do_qr ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   one_argument( argument, arg );

   if (IS_NPC(ch))
        return;

   if (arg[0] != '\0')
   {
        send_to_char("No argument needed.\n\r", ch);
        return;
   }

    do_qwest( ch, "request" );
        return;
}
void do_qc ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   one_argument( argument, arg );

   if (IS_NPC(ch))
        return;

   if (arg[0] != '\0')
   {
        send_to_char("No argument needed.\n\r", ch);
        return;
   }

    do_qwest( ch, "complete" );
        return;
}
void do_ql ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   one_argument( argument, arg );

   if (IS_NPC(ch))
        return;

   if (arg[0] != '\0')
   {
        send_to_char("No argument needed.\n\r", ch);
        return;
   }

    do_qwest( ch, "list" );
        return;
}
void do_qb ( CHAR_DATA *ch, char *argument )
{
    send_to_char("You should spell out 'Quest Buy' on your own.\n\r",ch);
    send_to_char("After all, you would not want to make a typo like that!\n\r",ch);
}
void do_qq ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_STRING_LENGTH];
   one_argument( argument, arg );

   if (IS_NPC(ch))
        return;

   if (arg[0] != '\0')
   {
        send_to_char("No argument needed.\n\r", ch);
        return;
   }

    do_qwest( ch, "quit" );
        return;
}

