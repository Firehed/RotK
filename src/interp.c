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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"

bool	check_social	args( ( CHAR_DATA *ch, char *command,
			    char *argument ) );
char last_command[MAX_STRING_LENGTH]; 

/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2

//DECLARE_DO_FUN(do_qwest         );


/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;

bool				global_quest	= FALSE;


/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] =
{
    /*
     * Common movement commands.
     */
    { "north",		do_north,	POS_STANDING,    0,  1,  LOG_NEVER, 0 },
    { "east",		do_east,	POS_STANDING,	 0,  1,  LOG_NEVER, 0 },
    { "south",		do_south,	POS_STANDING,	 0,  1,  LOG_NEVER, 0 },
    { "west",		do_west,	POS_STANDING,	 0,  1,  LOG_NEVER, 0 },
    { "up",		do_up,		POS_STANDING,	 0,  1,  LOG_NEVER, 0 },
    { "down",		do_down,	POS_STANDING,	 0,  1,  LOG_NEVER, 0 },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "at",             do_at,          POS_DEAD,       L6,  1,  LOG_NORMAL, 1 },

    { "cast",		do_cast,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "ooc",            do_ooc,         POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "buy",		do_buy,		POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "channels",       do_channels,    POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "exits",		do_exits,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "get",		do_get,		POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "goto",           do_goto,        POS_DEAD,       IM,  1,  LOG_NORMAL, 1 },
    { "group",          do_group,       POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "guild",		do_guild,	POS_DEAD,	L2,  1,  LOG_ALWAYS, 1 },
    { "hit",		do_kill,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 0 },
    { "rp",         do_rp,      POS_DEAD,        0,  1,  LOG_NEVER,  0 },
    { "inventory",	do_inventory,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "kill",		do_kill,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "look",		do_look,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "clan",		do_clantalk,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "music",          do_music,   	POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "mock",		do_mock,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "order",		do_order,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "practice",       do_practice,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "rest",		do_rest,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "sit",		do_sit,		POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "sockets",        do_sockets,	POS_DEAD,       IM,  1,  LOG_NORMAL, 1 },
    { "ipcheck",        do_ipcheck,	POS_DEAD,       IM,  1,  LOG_NORMAL, 1 },
    { "stand",		do_stand,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "tell",		do_tell,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "unlock",         do_unlock,      POS_RESTING,     0,  1,  LOG_NORMAL, 1 },
    { "wield",		do_wear,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "wizhelp",	do_wizhelp,	POS_DEAD,	 0,  2,  LOG_NORMAL, 1 },
    { "quest",		do_qwest,	POS_STANDING,	0,   1,  LOG_NEVER,  1 },
    { "qp",		do_qp,     	POS_STANDING,	0,   1,  LOG_NEVER,  0 },
    { "qi",		do_qi,	        POS_STANDING,	0,   1,  LOG_NEVER,  0 },
    { "qt",		do_qt,	        POS_STANDING,	0,   1,  LOG_NEVER,  0 },
    { "qr",		do_qr,          POS_STANDING,	0,   1,  LOG_NEVER,  0 },
    { "qc",		do_qc,     	POS_STANDING,	0,   1,  LOG_NEVER,  0 },
    { "ql",		do_ql,     	POS_STANDING,	0,   1,  LOG_NEVER,  0 },
    { "catchup",        do_catchup,     POS_DEAD,       0,   1,  LOG_NEVER,  0 },
    { "qb",		do_qb,     	POS_STANDING,	0,   1,  LOG_NEVER,  0 },
    { "qq",		do_qq,     	POS_STANDING,	0,   1,  LOG_NEVER,  0 },


    /*
     * Informational commands.
     */
    { "affects",	do_affects,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "areas",		do_areas,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "bug",		do_bug,		POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "changes",	do_changes,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "commands",	do_commands,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "compare",	do_compare,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "consider",	do_consider,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "count",		do_count,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "credits",	do_credits,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "equipment",	do_equipment,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "examine",	do_examine,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
/*  { "groups",		do_groups,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, */
    { "?",		do_help,	POS_DEAD,	 0,  1,  LOG_NORMAL, 0 },
    { "help",		do_help,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "idea",		do_idea,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "info",           do_groups,      POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "motd",		do_motd,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "news",		do_news,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "peek",		do_peek,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "read",		do_read,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "refill",     do_refill,  POS_STANDING,   0,  1,  LOG_NEVER,  1   },
    { "report",		do_report,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "rules",		do_rules,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "score",		do_score,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "scan",		do_scan,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "skills",		do_skills,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "socials",	do_socials,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "show",		do_show,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "spells",		do_spells,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "spy",            do_spy,         POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "story",		do_story,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "time",		do_time,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "token",		do_token,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "typo",		do_typo,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "weather",	do_weather,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "who",		do_who,		POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "whois",		do_whois,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "wizlist",	do_wizlist,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "worth",		do_worth,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },

    /*
     * Configuration commands.
     */
    { "alia",		do_alia,	POS_DEAD,	 0,  1,  LOG_NORMAL, 0 },
    { "alias",		do_alias,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "autolist",	do_autolist,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "autoall",	do_autoall,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "autoassist",	do_autoassist,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "autoexit",	do_autoexit,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "autogold",	do_autogold,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "autoloot",	do_autoloot,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "autosac",	do_autosac,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    /* Eric 12-17-01 */
    { "autotitle",	do_autotitle,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "autosplit",	do_autosplit,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "autostore",	do_autostore,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "autopeek",	do_autopeek,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "brief",		do_brief,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
/*  { "channels",	do_channels,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 }, */
    { "colour",         do_colour,      POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "color",          do_colour,      POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "combine",	do_combine,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "compact",	do_compact,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "description",	do_description,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "lore",		do_lore,	POS_RESTING,	 0,  1,  LOG_NEVER,  1 },
    { "long",		do_long,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "nofollow",	do_nofollow,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "noloot",		do_noloot,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "nosummon",	do_nosummon,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "notran",		do_notran,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "outfit",		do_outfit,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "pklist",         do_pklist,      POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "password",	do_password,	POS_DEAD,	 0,  1,  LOG_NEVER,  1 },
    { "paets",          do_paets,       POS_DEAD,        0,  1,  LOG_NEVER,  1 },
    { "prompt",		do_prompt,	POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    { "scroll",		do_scroll,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "title",		do_title,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "pretitle",	do_pretitle,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "unalias",	do_unalias,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },

    /*
     * Communication commands.
     */
    { "afk",		do_afk,		POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "age",            do_age,         POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "answer",		do_answer,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "ansi",		do_colour,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "auction",	do_auction,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, 
    { "deaf",		do_deaf,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "emote",		do_emote,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "pmote",		do_pmote,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { ".",		do_gossip,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 0 },
    { "ic",		do_gossip,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "chat",		do_gossip,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "gossip",		do_gossip,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "=",		do_cgossip,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 0 },
    { "cgossip",	do_cgossip,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "-",		do_qgossip,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 0 },
    { "qgossip",	do_qgossip,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { ",",		do_emote,	POS_RESTING,	 0,  1,  LOG_NORMAL, 0 },
    { "grats",		do_grats,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "gtell",		do_gtell,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { ";",		do_gtell,	POS_DEAD,	 0,  1,  LOG_NORMAL, 0 },
/*  { "music",		do_music,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, */
    { "note",		do_note,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "rpnote",         do_rpnote,      POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "roleplaynote",   do_rpnote,      POS_SLEEPING,    0,  1,  LOG_NORMAL, 0 },
    { "blind",		do_blind_rage,	POS_FIGHTING,	 0,  1,	 LOG_NEVER,  1 },
    { "whirlwind",		do_whirlwind,	POS_FIGHTING,	 0,  1,	 LOG_NEVER,  1 },
    { "rage",		do_blind_rage,	POS_FIGHTING,	 0,  1,	 LOG_NEVER,  1 },
    { "multishot",		do_multishot,	POS_FIGHTING,	 0,  1,	 LOG_NEVER,  1 },
    { "pose",		do_pose,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "iquest",		do_quest,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "ask",		do_ask,		POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "quote",		do_quote,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "quiet",		do_quiet,	POS_SLEEPING, 	 0,  1,  LOG_NORMAL, 1 },
    { "reply",		do_reply,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "replay",		do_replay,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "say",		do_say,		POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "'",		do_say,		POS_RESTING,	 0,  1,  LOG_NORMAL, 0 },
    { "shout",		do_shout,	POS_RESTING,	 3,  1,  LOG_NORMAL, 1 },
    { "unread",		do_unreadnew,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "unreadold",	do_unread,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "weddings",	do_weddings,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "yell",		do_yell,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    /*
     * Object manipulation commands.
     */
    { "brandish",	do_brandish,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "close",		do_close,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "drink",		do_drink,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "drop",		do_drop,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "eat",		do_eat,		POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "envenom",	do_envenom,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "fill",		do_fill,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "give",		do_give,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "heal",		do_heal,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "repent",		do_repent,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "curse",		do_curse,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "hold",		do_wear,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "list",		do_list,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "lock",		do_lock,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "locker",     do_locker,  POS_STANDING,    0,  1,  LOG_NORMAL, 1 },
    { "open",		do_open,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "pick",		do_pick,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "pour",		do_pour,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "put",		do_put,		POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "recall",		do_recall,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "htrecall",       do_htrecall,    POS_FIGHTING,    0,  1,  LOG_NORMAL, 1 },
    { "rub",		do_rub,		POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "/",		do_recall,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 0 },
    { "recite",		do_recite,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "remove",		do_remove,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "sell",		do_sell,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "second",		do_second,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "take",		do_get,		POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "junk",           do_sacrifice,   POS_RESTING,     0,  1,  LOG_NORMAL, 0 },
    { "tap",      	do_sacrifice,   POS_RESTING,     0,  1,  LOG_NORMAL, 0 },
/*  { "unlock",		do_unlock,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 }, */
    { "value",		do_value,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "wear",		do_wear,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "zap",		do_zap,		POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    /*
     * Combat commands.
     */
    { "backstab",	do_backstab,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "bash",		do_bash,	POS_FIGHTING,    0,  1,  LOG_NORMAL, 1 },
    { "quietnuke",      do_quiet_nuke,  POS_DEAD,       ML,  1,  LOG_ALWAYS, 1 },
    { "bs",		do_backstab,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 0 },
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "circle",		do_circle,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "deathbite",	do_death_bite,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "feed",		do_feed,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "dirt",		do_dirt,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "flee",		do_flee,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "gouge",		do_gouge,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "kick",		do_thunderkick,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "thunderkick",		do_thunderkick,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "impale",		do_impale,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "range",		do_range,	POS_FIGHTING,	 0,  1,  LOG_NEVER, 1 },
    { "knee",		do_knee,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "bind",		do_bind,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "holyfury",	do_holy_fury,   POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "fury",      	do_holy_fury,   POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "strike",		do_strike,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "awareness",	do_awareness,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "deathblow",	do_deathblow,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "murde",		do_murde,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 0 },
    { "murder",		do_murder,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 1 },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  1,  LOG_NORMAL, 0 },
    { "surrender",      do_surrender,   POS_FIGHTING,    0,  1,  LOG_NORMAL, 1 },
    { "trip",		do_trip,	POS_FIGHTING,    0,  1,  LOG_NORMAL, 1 },

    /*
     * Mob command interpreter (placed here for faster scan...)
     */
    { "mob",		do_mob,		POS_DEAD,	 0,  LOG_NEVER,  0 },

    /*
     * Miscellaneous commands.
     */
//    { "qwest",          do_qwest,       POS_RESTING,     0,  1,  LOG_NORMAL, 1 },
    { "enter", 		do_enter, 	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "follow",		do_follow,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "gain",		do_gain,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "go",		do_enter,	POS_STANDING,	 0,  1,  LOG_NORMAL, 0 },
/*  { "group",		do_group,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, */
    { "groups",		do_groups,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "hide",		do_hide,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "member",		do_member,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "play",		do_play,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
/*  { "practice",	do_practice,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, */
    { "qui",		do_qui,		POS_DEAD,	 0,  1,  LOG_NORMAL, 0 },
    { "quit",		do_quit,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "rent",		do_rent,	POS_DEAD,	 0,  1,  LOG_NORMAL, 0 },
    { "save",		do_save,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "snoop",		do_snoop,	POS_DEAD,	IM,  1,  LOG_ALWAYS, 1 },
    { "sneak",		do_sneak,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "split",		do_split,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "steal",		do_steal,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "train",		do_train,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "track",		do_track,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "visible",	do_visible,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 },
    { "where",		do_where,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 },
    { "donate",		do_donate,	POS_STANDING,	 5,  1,  LOG_NORMAL, 1 },
    { "cdonate",	do_cdonate,	POS_STANDING,	 5,  1,  LOG_NORMAL, 1 },
    { "class",		do_class,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "forge",		do_forge,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 0 },
    { "forget",		do_forget,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "remembe",	do_remembe,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 0 },
    { "remember",	do_remember,	POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 },
    { "voodoo",		do_voodoo,	POS_STANDING,	20,  1,  LOG_NORMAL, 1 },


    /*
     * Immortal commands.
     */
    { "advance",	do_advance,	POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "damage",         do_damage,      POS_DEAD,       IM,  1,  LOG_NORMAL, 1 },
    { "doubleexp",  do_doubleexp, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    { "nopk",  do_nopk, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    { "nocorpse",  do_nocorpse, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    { "bloodbath",  do_bloodbath, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    { "nosafe",  do_nosafe, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    { "dump",		do_dump,	POS_DEAD,	ML,  1,  LOG_ALWAYS, 0 },
    { "trust",		do_trust,	POS_DEAD,	ML,  1,  LOG_ALWAYS, 1 },
    { "violate",	do_violate,	POS_DEAD,	ML,  1,  LOG_ALWAYS, 1 },
/*      Removed due to MAJOR fuckups
    { "marry",		do_marry,	POS_DEAD,	IM,  1,	 LOG_ALWAYS, 1 },
    { "divorce",	do_divorce,	POS_DEAD,	IM,  1,	 LOG_ALWAYS, 1 },
    { "spousetalk",	do_spousetalk,  POS_RESTING,	0,   1,	 LOG_NEVER,  1 },
    { "consent",	do_consent,	POS_RESTING,	0,   1,  LOG_NORMAL, 1 }, */
    { "godsight",   do_godsight, POS_DEAD, IM, 1, LOG_NORMAL, 1 },
    { "allow",		do_allow,	POS_DEAD,	L2,  1,  LOG_ALWAYS, 1 },
    { "ban",		do_ban,		POS_DEAD,	L2,  1,  LOG_ALWAYS, 1 },
    { "deny",		do_deny,	POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "disconnect",	do_disconnect,	POS_DEAD,	L3,  1,  LOG_ALWAYS, 1 },
    { "dupe",		do_dupe,	POS_DEAD,	L1,  1,  LOG_NORMAL, 1 },
    { "finger",		do_finger,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 },
    { "flag",		do_flag,	POS_DEAD,	L2,  1,  LOG_ALWAYS, 1 },
    { "freeze",		do_freeze,	POS_DEAD,	L4,  1,  LOG_ALWAYS, 1 },
    { "permban",	do_permban,	POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "pload",		do_pload,	POS_DEAD,	L1,  1,  LOG_NORMAL, 1 },
    { "punload",	do_punload,	POS_DEAD,	L1,  1,  LOG_NORMAL, 1 },
    { "protect",	do_protect,	POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "nogsocial",      do_nogsocial,   POS_SLEEPING,    0,  1,  LOG_NEVER,  1 },
    { "reboo",		do_reboo,	POS_DEAD,	L1,  1,  LOG_NORMAL, 0 },
    { "reboot",		do_reboot,	POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "set",		do_set,		POS_DEAD,	L1,  1,  LOG_NORMAL, 1 },
    { "rset",           do_rset,        POS_DEAD,       L1,  1,  LOG_NORMAL, 1 },
    { "oset",           do_oset,        POS_DEAD,       L1,  1,  LOG_NORMAL, 1 },
    { "mset",           do_mset,        POS_DEAD,       L1,  1,  LOG_NORMAL, 1 },
    { "sset",           do_sset,        POS_DEAD,       L1,  1,  LOG_NORMAL, 1 },
    { "shutdow",	do_shutdow,	POS_DEAD,	ML,  1,  LOG_NORMAL, 0 },
    { "shutdown",	do_shutdown,	POS_DEAD,	ML,  1,  LOG_ALWAYS, 1 },
    { "spellup",	do_spellup,	POS_DEAD,	IM,  1,  LOG_ALWAYS, 1 },
/*  { "sockets",	do_sockets,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 }, */
    { "wedpost",	do_wedpost,	POS_DEAD,	ML,  1,  LOG_NORMAL, 1 },
    { "wizlock",	do_wizlock,	POS_DEAD,	L2,  1,  LOG_ALWAYS, 1 },
    { "balance",	do_balance,	POS_DEAD,	0,   1,  LOG_NORMAL,  1 },
    { "deposit",	do_deposit,	POS_RESTING,	0,   1,  LOG_NORMAL,  1 },
    { "withdraw",	do_withdraw,	POS_RESTING,	0,   1,  LOG_NORMAL,  1 },
    { "pray",		do_pray,	POS_DEAD,	0,   1,	 LOG_NEVER,  1 },
    { "force",		do_force,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "load",		do_load,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "mload",		do_mload,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "oload",		do_oload,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "vload",          do_vload,       POS_DEAD,       IM,  1,  LOG_NORMAL, 1 },
    { "newlock",	do_newlock,	POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "nochannels",	do_nochannels,	POS_DEAD,	L6,  1,  LOG_ALWAYS, 1 },
//    { "nobeep",		do_nobeep,	POS_DEAD,	L6,  1,  LOG_ALWAYS, 1 },
    { "noemote",	do_noemote,	POS_DEAD,	L5,  1,  LOG_ALWAYS, 1 },
    { "noclan",		do_noclan,	POS_DEAD,	L3,  1,  LOG_ALWAYS, 1 },
    { "norestore",	do_norestore,	POS_DEAD,	L3,  1,  LOG_ALWAYS, 1 },
    { "noshout",	do_noshout,	POS_DEAD,	L6,  1,  LOG_ALWAYS, 1 },
    { "notell",		do_notell,	POS_DEAD,	L5,  1,  LOG_ALWAYS, 1 },
    { "notitle",	do_notitle,	POS_DEAD,	L3,  1,  LOG_ALWAYS, 1 },
    { "pack",		do_pack,	POS_DEAD,	 0,  2,  LOG_NORMAL, 1 },
    { "pecho",		do_pecho,	POS_DEAD,	L5,  1,  LOG_NORMAL, 1 },
    { "purge",		do_purge,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "restore",	do_restore,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "sla",		do_sla,		POS_DEAD,	L3,  1,  LOG_NORMAL, 0 },
    { "slay",		do_slay,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "teleport",	do_transfer,    POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "transfer",	do_transfer,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "globals",    do_globals, POS_DEAD,   1, 1,  LOG_NEVER,  1},
    { "corner",		do_corner,	POS_DEAD,	IM,  1,  LOG_ALWAYS, 1 },
    { "twit",		do_twit,	POS_DEAD,	L7,  1,  LOG_NORMAL, 1 },
    { "pardon",		do_pardon,	POS_DEAD,	L7,  1,  LOG_NORMAL, 1 },
    { "killer",		do_killer,	POS_DEAD,	ML,  1,  LOG_NORMAL, 1 },
    { "unkiller",	do_unkiller,	POS_DEAD,	ML,  1,	 LOG_NORMAL, 1 },
/*  { "at",		do_at,		POS_DEAD,	L6,  1,  LOG_NORMAL, 1 }, */
    { "poofin",		do_bamfin,	POS_DEAD,	L8,  1,  LOG_NORMAL, 1 },
    { "poofout",	do_bamfout,	POS_DEAD,	L8,  1,  LOG_NORMAL, 1 },
    { "gecho",		do_echo,	POS_DEAD,	L4,  1,  LOG_NORMAL, 1 },
/*  { "goto",		do_goto,	POS_DEAD,	L8,  1,  LOG_NORMAL, 1 }, */
    { "ghost",		do_ghost,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "holylight",	do_holylight,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "slist",          do_slist,       POS_RESTING,    0,   1,  LOG_NORMAL, 1 },
    { "incognito",	do_incognito,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "invis",		do_invis,	POS_DEAD,	IM,  1,  LOG_NORMAL, 0 },
    { "invisin",	do_invisin,	POS_DEAD,	L8,  1,  LOG_NORMAL, 1 },
    { "invisout",	do_invisout,	POS_DEAD,	L8,  1,  LOG_NORMAL, 1 },
    { "log",		do_log,		POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "memory",		do_memory,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "mwhere",		do_mwhere,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "owhere",		do_owhere,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "peace",		do_peace,	POS_DEAD,	L6,  1,  LOG_NORMAL, 1 },
    { "allpeace",	do_allpeace,	POS_DEAD,	L1,  1,  LOG_NORMAL, 1 },
    { "penalty",	do_penalty,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "echo",		do_recho,	POS_DEAD,	L8,  1,  LOG_NORMAL, 1 },
    { "return",         do_return,      POS_DEAD,       L7,  1,  LOG_NORMAL, 1 },
    { "stat",		do_stat,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "rstat",		do_rstat,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "mstat",		do_mstat,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "ostat",		do_ostat,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "string",		do_string,	POS_DEAD,	L6,  1,  LOG_NORMAL, 1 },
    { "switch",		do_switch,	POS_DEAD,	L7,  1,  LOG_NORMAL, 1 },
    { "wizinvis",	do_invis,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "wecho",		do_wecho,	POS_DEAD,	ML,  1,  LOG_NORMAL, 1 },
    { "vnum",		do_vnum,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "ofind",		do_ofind,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "mfind",		do_mfind,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "zecho",		do_zecho,	POS_DEAD,	L4,  1,  LOG_NORMAL, 1 },
    { "clead",		do_clead,	POS_DEAD,	L4,  1,  LOG_NORMAL, 1 },
    { "cleadgossip", do_cleadgossip, POS_SLEEPING, 0, 1, LOG_NORMAL, 0 },
    { "leadgossip", do_cleadgossip, POS_SLEEPING, 0, 1, LOG_NORMAL, 0 },
    { "confiscate",	do_confiscate,	POS_DEAD,	IM,  1,	 LOG_ALWAYS, 1 },
    { "clone",		do_clone,	POS_DEAD,	L6,  1,  LOG_NORMAL, 1 },

    { "wiznet",		do_wiznet,	POS_DEAD,	 0,  2,  LOG_NORMAL, 1 },
    { "beep",		do_beep,	POS_RESTING,	 0,  2,	 LOG_NEVER,  1 },
    { "nuke",		do_nuke,	POS_DEAD,	ML,  1,  LOG_ALWAYS, 1 },
    //{ "immort",	do_immort,	POS_DEAD,	ML,  1,  LOG_ALWAYS, 1 },
    { "immtalk",	do_immtalk,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "imotd",          do_imotd,       POS_DEAD,       IM,  1,  LOG_NORMAL, 1 },
    { ":",		do_immtalk,	POS_DEAD,	IM,  1,  LOG_NORMAL, 0 },
    { "smote",		do_smote,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "prefi",		do_prefi,	POS_DEAD,	IM,  1,  LOG_NORMAL, 0 },
    { "prefix",		do_prefix,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "mpoint",		do_mpoint,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "mquest",		do_mquest,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "randclan",	do_randclan,	POS_DEAD,	ML,  1,  LOG_NORMAL, 1 },
    { "sign",		do_sign,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "immkiss",	do_immkiss,	POS_DEAD,	L4,  1,  LOG_NORMAL, 1 },
    { "squire",		do_squire,	POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "knight",		do_knight,	POS_DEAD,	ML,  1,  LOG_ALWAYS, 1 },
    { "recover",	do_recover,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "gsocial",	do_gocial,	POS_SLEEPING,	0,   1,	 LOG_NEVER,  1 },
    { "gset",		do_gset,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "wizslap",	do_wizslap,	POS_DEAD,	IM,  1,  LOG_NORMAL, 1 },
    { "wipe",		do_wipe,	POS_DEAD,	L1,  1,  LOG_ALWAYS, 1 },
    { "mpdump",		do_mpdump,	POS_DEAD,	IM,  1,  LOG_NEVER, 1 },
    { "mpstat",		do_mpstat,	POS_DEAD,       IM,  1,  LOG_NEVER, 1 },

    /*
     * OLC
     */
    { "edit",   do_olc,		POS_DEAD,    0,  LOG_NORMAL, 1	},
    { "asave",  do_asave,	POS_DEAD,    0,  LOG_NEVER,  1	},
    { "alist",  do_alist,	POS_DEAD,    0,  LOG_NEVER,  1	},
    { "resets", do_resets,	POS_DEAD,    0,  LOG_NORMAL, 1	},
    { "redit",  do_redit,	POS_DEAD,    0,	 LOG_NEVER,  1	},
    { "medit",  do_medit,	POS_DEAD,    0,	 LOG_NEVER,  1	},
    { "aedit",  do_aedit,	POS_DEAD,    0,  LOG_NORMAL, 1	},
    { "oedit",  do_oedit,	POS_DEAD,    0,  LOG_NEVER,  1	},
    { "mpedit", do_mpedit,	POS_DEAD,    0,  LOG_NORMAL, 1	},
    { "hedit",	do_hedit,	POS_DEAD,    0,	 LOG_NORMAL, 1	},
    /*
     * Commands needed last on list
     */
    { "restring",	do_restring,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "delet",		do_delet,	POS_DEAD,	 0,  1,  LOG_NORMAL, 0 },
    { "delete",		do_delete,	POS_STANDING,	 0,  1,  LOG_NORMAL, 1 },
    { "rerol",		do_rerol,	POS_DEAD,	 0,  2,  LOG_NORMAL, 0 },
    { "reroll",		do_reroll,	POS_STANDING,	 0,  2,  LOG_NORMAL, 1 },
    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  1,  LOG_NORMAL, 0 }
};

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;

    /*
     * Placed here, so we don't have to worry about tildes, period.
     * RW
     */
    smash_tilde( argument );

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * No hiding.
     */
    REMOVE_BIT( ch->affected_by, AFF_HIDE );

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    strcpy( buf, argument); 
    sprintf(last_command,"%s in room[%d]: %s.",ch->name,
    ch->in_room->vnum,
    buf);
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace(*argument) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	&&   !str_prefix( command, cmd_table[cmd].name )
	&&   cmd_table[cmd].level <= trust )
	{
	    if (cmd_table[cmd].tier == 1)
	    {
		found = TRUE;
		break;
	    } else if (ch->class >= MAX_CLASS/3)
	    {
		found = TRUE;
		break;
	    } else if (ch->level >= LEVEL_HERO)
	    {
		found = TRUE;
		break;
	    }
	}
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   (cmd_table[cmd].log == LOG_ALWAYS
    &&   ch->level != MAX_LEVEL ) )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
	/*
	 * Look for command in socials table.
	 */
	if ( !check_social( ch, command, argument ) )
	    send_to_char( "Huh?\n\r", ch );
	return;
    }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r", ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r", ch);
	    break;

	case POS_SITTING:
	    send_to_char( "Better stand up first.\n\r",ch);
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r", ch);
	    break;

	}
	return;
    }

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}

bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	&&   !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
	return FALSE;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r", ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too bad for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r", ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	if ( !str_cmp( social_table[cmd].name, "snore" ) )
	    break;
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
	act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
	act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
	act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
    }
    else
    {
	act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
	act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
	act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );

	if ( !IS_NPC(ch) && IS_NPC(victim)
	&&   !IS_AFFECTED(victim, AFF_CHARM)
	&&   IS_AWAKE(victim)
	&&   victim->desc == NULL)
	{
	    switch ( number_bits( 4 ) )
	    {
	    case 0:

	    case 1: case 2: case 3: case 4:
	    case 5: case 6: case 7: case 8:
		act( social_table[cmd].others_found,
		    victim, NULL, ch, TO_NOTVICT );
		act( social_table[cmd].char_found,
		    victim, NULL, ch, TO_CHAR    );
		act( social_table[cmd].vict_found,
		    victim, NULL, ch, TO_VICT    );
		break;

	    case 9: case 10: case 11: case 12:
		act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
		act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
		act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
		break;
	    }
	}
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( char *arg )
{

   if ( *arg == '\0' )
        return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit( *arg ) )
            return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}

/*
 * Given a string like 14*foo, return 14 and 'foo'
 */
int mult_argument(char *argument, char *arg)
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '*' )
        {
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '*';
            strcpy( arg, pdot+1 );
            return number;
        }
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;

    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level <  LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch )
	&&   cmd_table[cmd].show)
	{
	    if (cmd_table[cmd].tier == 1)
	    {
		sprintf( buf, "%-12s", cmd_table[cmd].name );
		send_to_char( buf, ch );
		if ( ++col % 6 == 0 )
		    send_to_char( "\n\r", ch );
	    } else if (ch->class >= MAX_CLASS/3)
	    {
		sprintf( buf, "%-12s", cmd_table[cmd].name );
		send_to_char( buf, ch );
		if ( ++col % 6 == 0 )
		    send_to_char( "\n\r", ch );
	    } else if (ch->level >= LEVEL_HERO)
	    {
		sprintf( buf, "%-12s", cmd_table[cmd].name );
		send_to_char( buf, ch );
		if ( ++col % 6 == 0 )
		    send_to_char( "\n\r", ch );
	    }
	}
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;

    col = 0;
    if ( ( ch->class < MAX_CLASS/3 )
    &&   ( get_trust( ch ) < LEVEL_HERO ) )
    {
	return;
    }
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( (cmd_table[cmd].level >= LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch )
        &&   cmd_table[cmd].show )
	||   ( ( cmd_table[cmd].tier == 2 )
	&&   ( cmd_table[cmd].level <= get_trust( ch ) )
        &&   cmd_table[cmd].show ) )

	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}


void do_spellup(CHAR_DATA *ch, char *argument)
{
 CHAR_DATA *vch;
  char arg[MAX_INPUT_LENGTH];
 DESCRIPTOR_DATA *d;

  argument = one_argument(argument,arg);

  if (IS_NPC(ch))
    return;
 if (arg[0] == '\0')
  { send_to_char("Spellup whom?\n\r",ch);
  return; }


    if ( !str_cmp( arg, "world" ) && (ch->level >= SQUIRE))
    {
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING
            &&   d->character != ch
            &&   d->character->in_room != NULL
            &&   ch->level >= d->character->ghost_level
            &&   can_see( ch, d->character ) )
            {
     char buf[MAX_STRING_LENGTH];
                sprintf( buf, "%s %s", d->character->name, arg );
                do_spellup( ch, buf );
            }
	}
       return;
    }
    if (str_cmp("room",arg))
    { 
	if ((vch = get_char_world(ch,arg)) == NULL)
	    { 
		send_to_char("They aren't here.\n\r",ch);
		return;
	    }
    }
  else
    vch = ch;

  if (!str_cmp("room",arg))
  for (vch = ch->in_room->people;vch;vch = vch->next_in_room)
  { if (vch == ch)
    continue;
    if (IS_NPC(vch))
      continue;

spell_shockshield(skill_lookup("shockshield"),ch->level,ch,vch,TARGET_CHAR);
spell_fireshield(skill_lookup("fireshield"),ch->level,ch,vch,TARGET_CHAR);
spell_iceshield(skill_lookup("iceshield"),ch->level,ch,vch,TARGET_CHAR);
spell_acidshield(skill_lookup("acidshield"),ch->level,ch,vch,TARGET_CHAR);
spell_glassshield(skill_lookup("glassshield"),ch->level,ch,vch,TARGET_CHAR);
spell_watershield(skill_lookup("watershield"),ch->level,ch,vch,TARGET_CHAR);
spell_holyshield(skill_lookup("holyshield"),ch->level,ch,vch,TARGET_CHAR);
    spell_shield(skill_lookup("shield"),ch->level,ch,vch,TARGET_CHAR);
    spell_armor(skill_lookup("armor"),ch->level,ch,vch,TARGET_CHAR);
spell_sanctuary(skill_lookup("sanctuary"),ch->level,ch,vch,TARGET_CHAR);
    spell_fly(skill_lookup("fly"),ch->level,ch,vch,TARGET_CHAR);
    spell_frenzy(skill_lookup("frenzy"),ch->level,ch,vch,TARGET_CHAR);
    spell_giant_strength(skill_lookup("giant strength"),ch->level,ch,vch,TARGET_CHAR);
    spell_bless(skill_lookup("bless"),ch->level,ch,vch,TARGET_CHAR);
    spell_haste(skill_lookup("haste"),ch->level,ch,vch,TARGET_CHAR);
  }
  else
  {
spell_shockshield(skill_lookup("shockshield"),ch->level,ch,vch,TARGET_CHAR);
spell_fireshield(skill_lookup("fireshield"),ch->level,ch,vch,TARGET_CHAR);
spell_iceshield(skill_lookup("iceshield"),ch->level,ch,vch,TARGET_CHAR);
spell_acidshield(skill_lookup("acidshield"),ch->level,ch,vch,TARGET_CHAR);
spell_glassshield(skill_lookup("glassshield"),ch->level,ch,vch,TARGET_CHAR);
spell_watershield(skill_lookup("watershield"),ch->level,ch,vch,TARGET_CHAR);
spell_holyshield(skill_lookup("holyshield"),ch->level,ch,vch,TARGET_CHAR);
    spell_shield(skill_lookup("shield"),ch->level,ch,vch,TARGET_CHAR);
    spell_armor(skill_lookup("armor"),ch->level,ch,vch,TARGET_CHAR);
spell_sanctuary(skill_lookup("sanctuary"),ch->level,ch,vch,TARGET_CHAR);
    spell_fly(skill_lookup("fly"),ch->level,ch,vch,TARGET_CHAR);
    spell_frenzy(skill_lookup("frenzy"),ch->level,ch,vch,TARGET_CHAR);
    spell_giant_strength(skill_lookup("giant strength"),ch->level,ch,vch,TARGET_CHAR);
    spell_bless(skill_lookup("bless"),ch->level,ch,vch,TARGET_CHAR);
    spell_haste(skill_lookup("haste"),ch->level,ch,vch,TARGET_CHAR);
  }
  return;

}


