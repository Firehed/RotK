
void do_balance ( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
	return;

    sprintf( buf, "You have %d coins in the bank.\n\r", ch->bank );
    send_to_char( buf, ch );
    return;
}

void do_deposit ( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *banker;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int amnt;

    if (IS_NPC(ch))
	{
	return;
	}

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))  
    {
	sprintf( buf, "But you are not in a bank.\n\r" );
	send_to_char( buf, ch );
	return;
    }

    banker = NULL;
    for ( banker = ch->in_room->people; banker; banker = banker->next_in_room )
    {
	if ( IS_NPC( banker ) && IS_SET(banker->pIndexData->act, ACT_BANKER) )
	    break;
    }

    if ( !banker )
    {
	sprintf( buf, "The banker is currently not available.\n\r" );
	send_to_char( buf, ch );
	return;
    }
 
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	sprintf( buf, "How much platinum do you wish to deposit?\n\r" );
	send_to_char( buf, ch );
	return;
    }

    amnt = atoi( arg );
    
    if ( amnt >= (ch->platinum + 1) )
    {
	sprintf( buf, "%s, you do not have %d platinum coins.", ch->name, amnt );
	do_say( banker, buf );
	return;
    }

    ch->bank += amnt;
    ch->platinum -= amnt;
    sprintf( buf, "%s, your account now contains: %d coins,", ch->name, ch->bank );
    do_say( banker, buf );
    sprintf( buf, "after depositing: %d coins.", amnt );
    do_say( banker, buf );
    return;
}

void do_withdraw ( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *banker
    char arg[MAX_INPUT_LENGTH]
    char buf[MAX_STRING_LENGTH]
    int amnt

    if (IS_NPC(ch))
	{
	return;
	}
    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))  
    {
	sprintf( buf, "But you are not in a bank.\n\r" );
	send_to_char( buf, ch );
	return;
  }

/*    banker = NULL;
    for ( banker = ch->in_room->people; banker; banker = banker->next_in_room )
    {
	if ( IS_NPC( banker ) && IS_SET(banker->pIndexData->act, ACT_BANKER) )
	{
	    break;
	}
    }*/

    if ( ch->in_room->people == !banker )
    {
	 sprintf( buf, "The banker is currently not available.\n\r" );
	 send_to_char( buf, ch );
	 return;
    }
 
argument = one_argument(argument, arg);

    if ( argument[0] == '\0' )
    {
	 sprintf( buf, "How much platinum do you wish to withdraw?\n\r" );
	 do_say( buf, banker );
	 return;
    }

    amnt = atoi( argument );
    
    if ( amnt >= (ch->bank + 1) )
    {
	sprintf( buf, "%s, you do not have %d platinum coins in the bank.", ch->name, amnt );
	do_say( banker, buf );
	return;
    }

    ch->platinum += amnt;
    ch->bank -= amnt;
    sprintf( buf, "%s, your account now contains: %d coins,", ch->name, ch->bank );
    do_say( banker, buf );
    sprintf( buf, "after withdrawing: %d coins.", amnt );
    do_say( banker, buf );
    return;
}

