l/*--------------------------------------------------------------------------
              .88b  d88. db    db d8888b.   .d888b. db   dD
              88'YbdP`88 88    88 88  `8D   VP  `8D 88 ,8P'
              88  88  88 88    88 88   88      odD' 88,8P
              88  88  88 88    88 88   88    .88'   88`8b
              88  88  88 88b  d88 88  .8D   j88.    88 `88.
              YP  YP  YP ~Y8888P' Y8888D'   888888D YP   YD
This material is copyrighted (c) 1999 - 2000 by Thomas J Whiting 
(twhiting@hawmps.2y.net). Usage of this material  means that you have read
and agree to all of the licenses in the ../licenses directory. None of these
licenses may ever be removed.
----------------------------------------------------------------------------
A LOT of time has gone into this code by a LOT of people. Not just on
this individual code, but on all of the codebases this even takes a piece
of. I hope that you find this code in some way useful and you decide to
contribute a small bit to it. There's still a lot of work yet to do.
---------------------------------------------------------------------------*/
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "interp.h"
#include "db.h"
#include "magic.h"
void    lightning        args( ( void ) );
void    ice	         args( ( void ) );
void    hail	         args( ( void ) );
void    blizzard         args( ( void ) );
void    fog              args( ( void ) );
void    weather_update   args( ( void ) );
extern char *target_name;
#define LI1		MAX_LEVEL/6
#define LI2		MAX_LEVEL/3
#define LI1		MAX_LEVEL
/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The {yday{x has begun.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The {Ysun{x rises in the east.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The {Ysun{x slowly disappears in the west.\n\r" );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "The {Dnight{x has begun.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	break;

    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 12 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    switch ( weather_info.sky )
    {
    default: 
	    weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
if ((number_chance(10))&&  (time_info.hour <= 6) )
      {
	    weather_info.sky = SKY_FOGGY;
     }
      else  if (number_chance(15))
      {
	    weather_info.sky = SKY_CLOUDY;
     }
      else   if (number_chance(30))
      {
	    weather_info.sky = SKY_RAINING;
     }
else
      if (number_chance(45))
      {
            weather_info.sky = SKY_CLOUDLESS;
     }


	break;

    case SKY_CLOUDY:
      if (number_chance(15))
	{
	    weather_info.sky = SKY_SNOWING;
	}
	else
      if (number_chance(15))
	{
	    weather_info.sky = SKY_HAILSTORM;
	}
	else
      if (number_chance(15))
	{
	    weather_info.sky = SKY_THUNDERSTORM;
	}
	else
      if (number_chance(15))
	{
	    weather_info.sky = SKY_ICESTORM;
	}      
else      if (number_chance(15))
	{
	    weather_info.sky = SKY_CLOUDLESS;
	}
else if (number_chance(25))
	{
	    weather_info.sky = SKY_CLOUDY;
	}
    case SKY_RAINING:
      if (number_chance(15))
        {
            weather_info.sky = SKY_LIGHTNING;
            lightning       ( );
        }
      else
      if (number_chance(10))
        {
            weather_info.sky = SKY_HAILSTORM;
	    hail();
        }
      else
      if (number_chance(10))
        {
            weather_info.sky = SKY_THUNDERSTORM;
        }
      else
      if (number_chance(10))
        {
            weather_info.sky = SKY_CLOUDY;
        }
else  if (number_chance(55))
        {
            weather_info.sky = SKY_RAINING;
        }
        break;

    case SKY_SNOWING:
if (number_chance(15))
	{
	    weather_info.sky = SKY_BLIZZARD;
	}
else if (number_chance(15))
	{
	    weather_info.sky = SKY_CLOUDY;
	}
else if (number_chance(15))
	{
	    weather_info.sky = SKY_RAINING;
	}
	else 
 if (number_chance(55))
	{
	    weather_info.sky = SKY_SNOWING;
	}

	break;
    case SKY_LIGHTNING:
if (number_chance(15))
	{
	    weather_info.sky = SKY_THUNDERSTORM;
          }
else if (number_chance(15))
	{
	    weather_info.sky = SKY_RAINING;
	}
else if (number_chance(15))
	{
	    weather_info.sky = SKY_CLOUDY;
	}
else if (number_chance(15))
	{
	    weather_info.sky = SKY_HAILSTORM;
          }
else if (number_chance(40))
	{
	    weather_info.sky = SKY_LIGHTNING;
          }
	break;
    case SKY_FOGGY:
 if (number_chance(45))
	{
 weather_info.sky = SKY_CLOUDY;
	}
else if (number_chance(55))
	{
        fog();
  weather_info.sky = SKY_FOGGY;
	break;
	}

    case SKY_THUNDERSTORM:
if (number_chance(15))
	{
	    weather_info.sky = SKY_RAINING;
          }
else if (number_chance(15))
	{
	    weather_info.sky = SKY_CLOUDY;
	}
else if (number_chance(15))
	{
	    weather_info.sky = SKY_LIGHTNING;
	}
else if (number_chance(15))
	{
	    weather_info.sky = SKY_HAILSTORM;
hail ();          }
else if (number_chance(40))
	{
	    weather_info.sky = SKY_THUNDERSTORM;
          }
	break;

    case SKY_HAILSTORM:
 if (number_chance(15))
	{
	    weather_info.sky = SKY_CLOUDY;
	}
 else
if (number_chance(30))
	{
	    weather_info.sky = SKY_RAINING;
          }
      else
 if (number_chance(55))
	{
	    weather_info.sky = SKY_HAILSTORM;
	hail();
	}
	break;

    case SKY_ICESTORM:
 if (number_chance(15))
	{
	    weather_info.sky = SKY_CLOUDY;
          }
      else
 if (number_chance(15))
	{
	    weather_info.sky = SKY_BLIZZARD;
          }
else
 if (number_chance(15))
	{
	    weather_info.sky = SKY_SNOWING;
	}
      else
 if (number_chance(55))
	{
ice();
   weather_info.sky = SKY_ICESTORM;
  }
	break;


    case SKY_BLIZZARD:
 if (number_chance(15))
	{
	    weather_info.sky = SKY_SNOWING;
	}
else if (number_chance(15))
	{
           blizzard();
	    weather_info.sky = SKY_ICESTORM;
ice();
	}
else if (number_chance(15))
	{
           blizzard();
	    weather_info.sky = SKY_CLOUDY;
	}
else
 if (number_chance(55))
	{
           blizzard();
	    weather_info.sky = SKY_BLIZZARD;
	}
break;

    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTDOORS(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}






/*
New weather command. Reads the new weather stats as well as  tells you
what time of day it is (morning, noon, night) 
Added so that players don't HAVE to have autoweather enabled*/
void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    char buf2[MSL];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";


    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }
        send_to_char("{x\n\r",ch); 

        if (weather_info.sky == SKY_RAINING)
        {
  if ( number_range( 0, 100 ) <= 15  )
{
        send_to_char("{g[{YWeather{g] {CThere is a cold rain trickling down.{x",ch);
}
else
  if ( number_range( 0, 100 ) <= 30  )
{
        send_to_char("{g[{YWeather{g] {CThere is a warm rain trickling down.{x",ch);
}
else   if ( number_range( 0, 100 ) <= 45  )
{
        send_to_char("{g[{YWeather{g] {CBig Huge {GFrogs{C are dropping from the sky like raindrops",ch); 
}

else
  if ( number_range( 0, 100 ) <= 60  )
{
        send_to_char("{g[{YWeather{g] {CBig Huge {YDragons{C are falling from the sky.",ch); 
}
else
  if ( number_range( 0, 100 ) <= 75  )
{
        send_to_char("{g[{YWeather{g] {CBig Huge {BPuppies{C are falling out of the sky like raindrops.",ch); 
}
else   if ( number_range( 0, 100 ) <= 85  )
{
        send_to_char("{g[{YWeather{g] {YKitten{C Sized animals appear to be falling left and right. Bonzai!!!!!.",ch); 
}
else
  if ( number_range( 0, 100 ) <= 100  )
{
        send_to_char("{g[{YWeather{g] {CBig Huge {RRats{C are falling out of the sky like raindrops.",ch); 
}

        send_to_char("{x\n\r",ch); 

        }

        else if (weather_info.sky == SKY_CLOUDY)
        {
        sprintf( buf, "{g[{YWeather{g] %s{x.\n\r",
        weather_info.change >= 0
        ? "{CA warm breeze can be felt about"
        : "{CA cold breeze can be felt about" );
    send_to_char( buf, ch );
        }
                 else if (weather_info.sky == SKY_CLOUDLESS)
        {
        send_to_char("{g[{YWeather{g] {CThe sky is beautiful, not a cloud around.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_THUNDERSTORM)
        {
        send_to_char("{g[{YWeather{g] {CThe skies thunder as a storm approaches.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_ICESTORM)
        {
        send_to_char("{g[{YWeather{g] {CSheets of ice appear to be falling from the sky.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_HAILSTORM)
        {
        send_to_char("{g[{YWeather{g] {CGolfball like substances are falling from the sky.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_SNOWING)
        {
        send_to_char("{g[{YWeather{g] {CA light snow is falling.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_BLIZZARD)
        {
        send_to_char("{g[{YWeather{g] {CThere is a blizzard about.{x\n\r",ch);
       }
                 else if (weather_info.sky == SKY_FOGGY)
        {
        send_to_char("{g[{YWeather{g] {CA misty haze covers the horizon.{x\n\r",ch);
        }

        else if (weather_info.sky == SKY_LIGHTNING)
        {
        send_to_char("{g[{YWeather{g] {CA {y Lightning{w storm is approaching {x\n\r",ch);
     }
    sprintf( buf2,
	"{B[{W Time  {B] {RIt is {M%d{R o'clock {Y%s{x\n\r",
	(time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
	time_info.hour >= 12 ? "pm" : "am");
    send_to_char( buf2, ch );
    return;

} 
/*  Xantha  10/00 */



bool IS_OUTDOORS( CHAR_DATA *ch )
{
    if( !IS_SET(ch->in_room->room_flags, ROOM_INDOORS) &&
        ch->in_room->sector_type != SECT_INSIDE )
        return TRUE;
    else
        return FALSE;
}

void do_autoweather(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_AUTOWEATHER))
    {
   send_to_char("You will no longer see weather descriptions in rooms\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOWEATHER);
    }
    else
    {
      send_to_char("You will now see weather descriptions in rooms\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOWEATHER);
    }
}

void show_weather(CHAR_DATA *ch)
{
    char buf  [MAX_STRING_LENGTH];

/*for weather stuff -- tjw*/
        if (weather_info.sky == SKY_RAINING)
        {
  if ( number_range( 0, 100 ) <= 15  )
{
        send_to_char("{g[{YWeather{g] {CThere is a cold rain trickling down.{x\n\r",ch);
}
else
  if ( number_range( 0, 100 ) <= 30  )
{
        send_to_char("{g[{YWeather{g] {CThere is a warm rain trickling down.{x\n\r",ch);
}
else   if ( number_range( 0, 100 ) <= 45  )
{
        send_to_char("{g[{YWeather{g] {CBig Huge {GFrogs{C are dropping from the sky like raindrops.{x\n\r",ch); 
}
else
  if ( number_range( 0, 100 ) <= 60  )
{
        send_to_char("{g[{YWeather{g] {CBig Huge {YPanthers{C are falling out of the sky like raindrops.{x\n\r",ch); 
}
else
  if ( number_range( 0, 100 ) <= 75  )
{
        send_to_char("{g[{YWeather{g] {CBig Huge {BPuppies{C are falling out of the sky like raindrops.{x\n\r",ch); 
}
else   if ( number_range( 0, 100 ) <= 85  )
{
        send_to_char("{g[{YWeather{g] {YCanth{C Sized animals appear to be falling left and right. Bonzai!!!!!.{x\n\r",ch); 
}
else
  if ( number_range( 0, 100 ) <= 100  )
{
        send_to_char("{g[{YWeather{g] {CBig Huge {RDRAGONS{C are falling out of the sky like raindrops.{x\n\r",ch); 
}
	    	    send_to_char( "{x\n\r",ch);

        }

        else if (weather_info.sky == SKY_CLOUDY)
        {
        sprintf( buf, "{g[{YWeather{g] %s{x.\n\r",
        weather_info.change >= 0
        ? "{CA warm breeze can be felt about"
        : "{CA cold breeze can be felt about" );
    send_to_char( buf, ch );
        }
                 else if (weather_info.sky == SKY_CLOUDLESS)
        {
        send_to_char("{g[{YWeather{g] {CThe sky is beautiful, not a cloud around.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_THUNDERSTORM)
        {
        send_to_char("{g[{YWeather{g] {CThe skies thunder as a storm approaches.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_ICESTORM)
        {
        send_to_char("{g[{YWeather{g] {CSheets of ice appear to be falling from the sky.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_HAILSTORM)
        {
        send_to_char("{g[{YWeather{g] {CGolfball like substances are falling from the sky.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_SNOWING)
        {
        send_to_char("{g[{YWeather{g] {CA light snow is falling.{x\n\r",ch);
        }
                 else if (weather_info.sky == SKY_BLIZZARD)
        {
        send_to_char("{g[{YWeather{g] {CThere is a blizzard about.{x\n\r",ch);
       }
                 else if (weather_info.sky == SKY_FOGGY)
        {
        send_to_char("{g[{YWeather{g] {CA misty haze covers the horizon.{x\n\r",ch);
        }

        else if (weather_info.sky == SKY_LIGHTNING)
        {
        send_to_char("{g[{YWeather{g] {CA {y Lightning{w storm is approaching {x\n\r",ch);
     }
        else 
        {
        send_to_char("{g[{YWeather{g] {CBUG!!!!!!!!!!!!!!! A descriptor does not exist for this condition  {x\n\r",ch);
        }
        }

void lightning( void )
{
 DESCRIPTOR_DATA *d;
  for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING
                && IS_OUTDOORS( d->character )
                && IS_AWAKE  ( d->character )
                && number_chance(10)
                && !IS_IMMORTAL(d->character)
                && d->character->level > 17
                && weather_info.sky == SKY_LIGHTNING ) {

 send_to_char("{x{RYou see a brilliant flash come down from the sky and then black out!{x\n\r",d->character);
 act( "$n has been struck by lightning!", d->character, NULL, NULL,TO_ROOM);
if(check_immune(d->character,DAM_LIGHTNING) != IS_IMMUNE) {
 if(d->character->fighting) {
stop_fighting(d->character,TRUE); }
if(check_immune(d->character,DAM_LIGHTNING) != IS_RESISTANT)
if (d->character->level < LI1);
d->character->hit -= d->character->hit/25;
if (d->character->level < LI2);
d->character->hit -= d->character->hit/20;
if (d->character->level < LI3);
d->character->hit -= d->character->hit/15;
WAIT_STATE(d->character,40); 
 } else {
if(check_immune(d->character,DAM_LIGHTNING) == IS_VULNERABLE) {
d->character->hit -= d->character->hit/10;
WAIT_STATE(d->character,40); } }
} }
}
void blizzard( void )
{
 DESCRIPTOR_DATA *d;
  for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING
                && IS_OUTDOORS( d->character )
                && weather_info.sky == SKY_BLIZZARD )
 {
  if ( number_range( 0, 2000 ) <= 1000  )
      {
send_to_char("{RThe sky before you is a mist of white blur. Perhaps you should find a safe place indoors.{x \n\r",d->character);
     }
else
  if ( number_range( 0, 2000 ) <= 1250  )
      {
send_to_char("{RNot being able to see where you are going, you slip and fall into a hole.{x \n\r",d->character );
 act( "$n falls face first into the oncoming drifts!", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 5;
     }
else
  if ( number_range( 0, 2000 ) <= 1500  )
      {
send_to_char("{RYou've managed to slide yourself right into a tree. Good Job. Better hope you didn't break anything!{x \n\r",d->character);
 act( "$n has planted themselves headfirst into a tree. Encore!", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 15;
}
else
  if ( number_range( 0, 2000 ) <= 1950  )
      {
send_to_char("{RThe sky before you is a mist of white blur. Perhaps you should find a safe place indoors.{x \n\r",d->character);
     }
else
  if ( number_range( 0, 2000 ) <= 2000  )
      {
send_to_char("{RYour body twitches and your limbs start to freeze one by one.{x \n\r", d->character);
 act( "You watch as $n's limbs start to freeze !", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 30;
     }
} }
}
void ice( void )
{
 DESCRIPTOR_DATA *d;
  for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING
                && IS_OUTDOORS( d->character )
                && weather_info.sky == SKY_ICESTORM )
 {
  if ( number_range( 0, 2000 ) <= 1000  )
      {
send_to_char("{RIt's starting to rain sheets of ice. Perhaps you should find a way inside{x \n\r",d->character);
     }
else
  if ( number_range( 0, 2000 ) <= 1250  )
      {
send_to_char("{RThe ice around your feet firms up and causes you to fall  flat on your face{x \n\r",d->character );
 act( "$n falls face first into the ground!", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 5;
     }
else
  if ( number_range( 0, 2000 ) <= 1500  )
      {
send_to_char("{RYou've managed to slide yourself right into a tree. Good Job. Better hope you didn't break anything!{x \n\r",d->character);
 act( "$n has planted themselves headfirst into a tree. Encore!", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 15;
}
else
  if ( number_range( 0, 2000 ) <= 1950  )
      {
send_to_char("{RThe sky before you is a mist of white blur. Perhaps you should find a safe place indoors.{x \n\r",d->character);
     }
else
  if ( number_range( 0, 2000 ) <= 2000  )
      {
send_to_char("{RYour body twitches and your limbs start to freeze one by one.{x \n\r", d->character);
 act( "You watch as $n's limbs start to freeze !", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 30;
     }
} }
}
void hail( void )
{
 DESCRIPTOR_DATA *d;
  for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING
                && IS_OUTDOORS( d->character )
                && weather_info.sky == SKY_HAILSTORM )
 {
  if ( number_range( 0, 2000 ) <= 1000  )
      {
send_to_char("{RWas that a golfball or hail? It might be a good idea to  find yourself a way indoors quickly.{x \n\r",d->character);
     }
else
  if ( number_range( 0, 2000 ) <= 1250  )
      {
send_to_char("{ROh MAN! You were just hit in the face by hail!{x\n\r",d->character );
 act( "You watch in ammusement as $n is hit in the face by a baby iceball  ", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 5;
     }
else
  if ( number_range( 0, 2000 ) <= 1950  )
      {
send_to_char("{RThe sky before you is a mist of white blur. Perhaps you should find a safe place indoors.{x \n\r",d->character);
     }
else
  if ( number_range( 0, 2000 ) <= 2000  )
      {
send_to_char("{RYour body twitches and your limbs start to freeze one by one.{x \n\r", d->character);
 act( "You watch as $n's limbs start to freeze !", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 30;
     }
} }
}
 
void fog( void )
{
 DESCRIPTOR_DATA *d;
  for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING
                && IS_OUTDOORS( d->character )
                && weather_info.sky == SKY_FOGGY )
 {
  if ( number_range( 0, 2000 ) <= 1000  )
      {
send_to_char("{RThe morning fog is as thick as pea soup. Perhaps you should find your way indoors.{x \n\r",d->character);
     }
else
  if ( number_range( 0, 2000 ) <= 1250  )
      {
send_to_char("{RNot being able to see where you are going, you slip and fall into a hole.{x \n\r",d->character );
 act( "$n falls face first into a hole!", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 5;
     }
else
  if ( number_range( 0, 2000 ) <= 1500  )
      {
send_to_char("{RYou've walked straight into a tree! Way to go!!{x \n\r",d->character);
 act( "$n has planted themselves headfirst into a tree. Encore!", d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 15;
}
else
  if ( number_range( 0, 2000 ) <= 1950  )
      {
send_to_char("{RThe morning fog is as thick as pea soup. Perhaps you should find your way indoors.{x \n\r",d->character);
     }
else
  if ( number_range( 0, 2000 ) <= 2000  )
      {
send_to_char("{RNot being able to see where you're going, you fall straight into a river.{x \n\r", d->character);
act( "$n pulls $m clothes out of the river after falling in!",d->character, NULL, NULL,TO_ROOM);
d->character->hit -= 30;
     }

} }
}
 
bool number_chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}

void spell_control_weather(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    if ( !str_cmp( target_name, "better" ) )
{

if (weather_info.sky == SKY_CLOUDLESS)
{
send_to_char("But it's so beautiful outside already\n\r",ch);
return;
}
else
if (weather_info.sky == SKY_CLOUDY)
{
send_to_char("You recite the ancient spell and the clouds part in obedience\n\r",ch);
act( "$n makes a strange movement with their hands and the clouds part.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_CLOUDLESS;
return;
}
else
if (weather_info.sky == SKY_RAINING)
{
send_to_char("You recite the ancient spell and the  rain stops in obedience\n\r",ch);
act( "$n makes a strange movement with their hands and the rain stops.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_CLOUDY;
return;
}
else
if (weather_info.sky == SKY_LIGHTNING)
{
send_to_char("You recite the ancient spell and the lightning ceases in obedience\n\r",ch);
act( "$n makes a strange movement with their hands and the lightning stops.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_THUNDERSTORM;
return;
}
else
if (weather_info.sky == SKY_THUNDERSTORM)
{
send_to_char("You recite the ancient spell and the storm ceases\n\r",ch);
act( "$n makes a strange movement with their hands and the  storm ceases.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_RAINING;
return;
}
else
if (weather_info.sky == SKY_SNOWING)
{
send_to_char("You recite the ancient spell and the snow ceases in obedience\n\r",ch);
act( "$n makes a strange movement with their hands and the snow ceases.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_CLOUDY;
return;
}
else
if (weather_info.sky == SKY_BLIZZARD)
{
send_to_char("You recite the ancient spell and the  horizon clears\n\r",ch);
act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_SNOWING;
return;
}
else
if (weather_info.sky == SKY_ICESTORM)
{
send_to_char("You recite the ancient spell and the  horizon clears\n\r",ch);
act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_RAINING;
return;
}
else
if (weather_info.sky == SKY_HAILSTORM)
{
send_to_char("You recite the ancient spell and the  horizon clears\n\r",ch);
act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_RAINING;
return;
}
else
if (weather_info.sky == SKY_FOGGY)
{
send_to_char("You recite the ancient spell and the  horizon clears\n\r",ch);
act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_CLOUDY;
return;
}
else 
{
            send_to_char("Bad Weather Call. Please notify the imms.\n\r",ch);
}

}
    else if ( !str_cmp( target_name, "worse" ) )
{

if (weather_info.sky == SKY_CLOUDLESS)
{
send_to_char("You recite the ancient spell and the clouds  come at your command.\n\r",ch);
act( "$n makes a strange movement with their hands and the clouds  darken the sky.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_CLOUDY;
return;
}
else
if (weather_info.sky == SKY_CLOUDY)
{
send_to_char("You recite the ancient spell and the clouds trickle down rain\n\r",ch);
act( "$n makes a strange movement with their hands and the clouds open up to rain.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_RAINING;
return;
}
else
if (weather_info.sky == SKY_RAINING)
{
send_to_char("You recite the ancient spell and the  rain  turns to hail\n\r",ch);
act( "$n makes a strange movement with their hands and the rain  turns to hail.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_HAILSTORM;
return;
}
else
if (weather_info.sky == SKY_LIGHTNING)
{
send_to_char("You recite the ancient spell and the  clouds send down sheets of ice\n\r",ch);
act( "$n makes a strange movement with their hands and the lightning turns to ice.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_ICESTORM;
return;
}
else
if (weather_info.sky == SKY_THUNDERSTORM)
{
send_to_char("You recite the ancient spell and the clouds clap in thunderous approval\n\r",ch);
act( "$n makes a strange movement with their hands and the  clouds clap in thunder.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_LIGHTNING;
return;
}
else
if (weather_info.sky == SKY_SNOWING)
{
send_to_char("You recite the ancient spell and the snow increases in obedience\n\r",ch);
act( "$n makes a strange movement with their hands and the snow turns to a blizzard.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_BLIZZARD;
return;
}
else
if (weather_info.sky == SKY_BLIZZARD)
{
send_to_char("It's already as bad as it can get\n\r",ch);
return;
}
else
if (weather_info.sky == SKY_ICESTORM)
{
send_to_char("It's already as bad as it can get\n\r",ch);
return;
}
else
if (weather_info.sky == SKY_HAILSTORM)
{
send_to_char("It's already as bad as it can get\n\r",ch);
return;
}
else
if (weather_info.sky == SKY_FOGGY)
{
send_to_char("You recite the ancient spell and the  horizon clears\n\r",ch);
act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
weather_info.sky = SKY_CLOUDY;
return;

}
else 
{
            send_to_char("Bad Weather Call. Please notify the imms.\n\r",ch);
}

}
    else
        send_to_char ("Do you want it to get better or worse?\n\r", ch );

    send_to_char( "Ok.\n\r", ch );
    return;
}


void do_wset( CHAR_DATA *ch, char *argument )
{
char arg1 [MIL];
argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax:\n\r",ch);
        send_to_char( "  set weather <condition> \n\r", ch);
        send_to_char( "  Condition can be :\n\r",ch);
        send_to_char("   hail, fog, icestorm, blizzard, snowing,  rain\n\r",ch);
        send_to_char("   lightning, thunderstorm, cloudless, cloudy  \n\r",ch);
        return;
    }
 if(!str_cmp(arg1,"cloudless"))
{
send_to_char( "You wave your hands and in reverence to you, the clouds dissapear \n\r", ch);
          weather_info.sky = SKY_CLOUDLESS;
act( "$n makes a strange movement with their hands and the clouds part.", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"fog"))
{
send_to_char( "You wave your hands and in reverence to you, a mist vapors the horizon \n\r", ch);
          weather_info.sky = SKY_FOGGY;
act( "$n makes a strange movement with their hands and a mist vapors the horizon.", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"hail"))
{
send_to_char( "You wave your hands and in reverence to you, hailstones fall from the sky \n\r", ch);
          weather_info.sky = SKY_HAILSTORM;
act( "$n makes a strange movement with their hands and hailstones fall from the sky.", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"icestorm"))
{
send_to_char( "You wave your hands and in reverence to you, it starts raining ice \n\r", ch);
          weather_info.sky = SKY_ICESTORM;
act( "$n makes a strange movement with their hands and it starts raining ice.", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"blizzard"))
{
send_to_char( "You wave your hands and in reverence to you, snowflakes cover the horizon making it impossible to see.\n\r", ch);
          weather_info.sky = SKY_BLIZZARD;
act( "$n makes a strange movement with their hands and snowflakes cover the horizon making it impossibile to see", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"snowing"))
{
send_to_char( "You wave your hands and in reverence to you, snowflakes  fall from the sky..\n\r", ch);
          weather_info.sky = SKY_SNOWING;
act( "$n makes a strange movement with their hands and snowflakes fall from the sky", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"rain"))
{
send_to_char( "You wave your hands and in reverence to you, a warm rain starts to fall.\n\r", ch);
          weather_info.sky = SKY_RAINING;
act( "$n makes a strange movement with their hands and a warm rain starts to fall", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"lightning"))
{
send_to_char( "You wave your hands and in reverence to you, lightning pierces the sky. \n\r", ch);
          weather_info.sky = SKY_LIGHTNING;
act( "$n makes a strange movement with their hands and lightning pierces  the sky", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"thunderstorm"))
{
send_to_char( "You wave your hands and in reverence to you, The clouds clap in thunder.\n\r", ch);
          weather_info.sky = SKY_THUNDERSTORM;
act( "$n makes a strange movement with their hands and the clouds clap in thunder", ch, NULL, NULL, TO_ROOM);
}
else
 if(!str_cmp(arg1,"cloudy"))
{
send_to_char( "You wave your hands and in reverence to you, clouds cover the horizon, threatening rain.\n\r", ch);
          weather_info.sky = SKY_CLOUDY;
act( "$n makes a strange movement with their hands and clouds cover the horizon, threatening rain", ch, NULL, NULL, TO_ROOM);
}
/* okay, we haven't met any conditions so far. Did they forget? */
else
    {
        send_to_char( "Syntax:\n\r",ch);
        send_to_char( "  set weather <condition> \n\r", ch);
        send_to_char( "  Condition can be :\n\r",ch);
        send_to_char("   hail, fog, icestorm, blizzard, snowing,  rain\n\r",ch);
        send_to_char("   lightning, thunderstorm, cloudless, cloudy  \n\r",ch);
        return;
    }

}
