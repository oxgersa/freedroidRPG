/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *   Copyright (c) 2004-2007 Arthur Huillet
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

/**
 * This file contains all functions dealing with the dialog interface,
 * including blitting the chat protocol to the screen and drawing the
 * right portrait images to the screen.
 */

#define _chat_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "SDL_rotozoom.h"


#define PUSH_ROSTER 2
#define POP_ROSTER 3 

dialogue_option ChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
EXTERN char *PrefixToFilename[ ENEMY_ROTATION_MODELS_AVAILABLE ];
char* chat_protocol = NULL ;


void DoChatFromChatRosterData(int ChatPartnerCode , Enemy ChatDroid , int ClearProtocol );

/**
 *
 *
 */
void
push_or_pop_chat_roster ( int push_or_pop )
{
    static dialogue_option LocalChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
    
    if ( push_or_pop == PUSH_ROSTER )
    {
	memcpy ( LocalChatRoster , ChatRoster , sizeof ( dialogue_option ) * MAX_DIALOGUE_OPTIONS_IN_ROSTER ) ;
	int i;
	for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
	    {
	    delete_one_dialog_option ( i , TRUE );
	    }

    }
    else if ( push_or_pop == POP_ROSTER )
    {
	memcpy ( ChatRoster , LocalChatRoster , sizeof ( dialogue_option ) * MAX_DIALOGUE_OPTIONS_IN_ROSTER ) ;
    }
    else
    {
	ErrorMessage ( __FUNCTION__  , "\
There was an unrecognized parameter handled to this function." ,
				   PLEASE_INFORM, IS_FATAL );
    }
    
}; // push_or_pop_chat_roster ( int push_or_pop )


/**
 * This function finds the index of the array where the chat flags for
 * this person are stored.  It does this by exploiting on the (unique?)
 * dialog section to use entry of each (friendly) droid.
 */
int
ResolveDialogSectionToChatFlagsIndex ( char* SectionName )
{
    if ( strcmp ( SectionName , "Chandra" ) == 0 ) return PERSON_CHA ;
    if ( strcmp ( SectionName , "Sorenson" ) == 0 ) return PERSON_SORENSON;
    if ( strcmp ( SectionName , "614" ) == 0 ) return PERSON_614;
    if ( strcmp ( SectionName , "Stone" ) == 0 ) return PERSON_STONE;
    if ( strcmp ( SectionName , "Pendragon" ) == 0 ) return PERSON_PENDRAGON;
    if ( strcmp ( SectionName , "Dixon" ) == 0 ) return PERSON_DIXON;
    if ( strcmp ( SectionName , "RMS" ) == 0 ) return PERSON_RMS;
    if ( strcmp ( SectionName , "MER" ) == 0 ) return PERSON_MER;
    if ( strcmp ( SectionName , "Francis" ) == 0 ) return PERSON_FRANCIS;
    if ( strcmp ( SectionName , "Ernie" ) == 0 ) return PERSON_ERNIE;
    if ( strcmp ( SectionName , "Bruce" ) == 0 ) return PERSON_BRUCE;
    if ( strcmp ( SectionName , "Benjamin" ) == 0 ) return PERSON_BENJAMIN;
    if ( strcmp ( SectionName , "Bender" ) == 0 ) return PERSON_BENDER;
    if ( strcmp ( SectionName , "Spencer" ) == 0 ) return PERSON_SPENCER;
    if ( strcmp ( SectionName , "Butch" ) == 0 ) return PERSON_BUTCH;
    if ( strcmp ( SectionName , "Darwin" ) == 0 ) return PERSON_DARWIN;
    if ( strcmp ( SectionName , "Duncan" ) == 0 ) return PERSON_DUNCAN;
    if ( strcmp ( SectionName , "DocMoore" ) == 0 ) return PERSON_DOC_MOORE;
    if ( strcmp ( SectionName , "Melfis" ) == 0 ) return PERSON_MELFIS;
    if ( strcmp ( SectionName , "Michelangelo" ) == 0 ) return PERSON_MICHELANGELO;
    if ( strcmp ( SectionName , "Skippy" ) == 0 ) return PERSON_SKIPPY;
    if ( strcmp ( SectionName , "StandardOldTownGateGuard" ) == 0 ) return PERSON_STANDARD_OLD_TOWN_GATE_GUARD;
    if ( strcmp ( SectionName , "StandardNewTownGateGuard" ) == 0 ) return PERSON_STANDARD_NEW_TOWN_GATE_GUARD;
    if ( strcmp ( SectionName , "OldTownGateGuardLeader" ) == 0 ) return PERSON_OLD_TOWN_GATE_GUARD_LEADER;
    if ( strcmp ( SectionName , "StandardMSFacilityGateGuard" ) == 0 ) return PERSON_STANDARD_MS_FACILITY_GATE_GUARD;
    if ( strcmp ( SectionName , "MSFacilityGateGuardLeader" ) == 0 ) return PERSON_MS_FACILITY_GATE_GUARD_LEADER;
    if ( strcmp ( SectionName , "HEA" ) == 0 ) return PERSON_HEA;
    if ( strcmp ( SectionName , "StandardBotAfterTakeover" ) == 0 ) return PERSON_STANDARD_BOT_AFTER_TAKEOVER;
    if ( strcmp ( SectionName , "Tybalt" ) == 0 ) return PERSON_TYBALT;
    if ( strcmp ( SectionName , "Ewald" ) == 0 ) return PERSON_EWALD;
    if ( strcmp ( SectionName , "KevinGuard" ) == 0 ) return PERSON_KEVINS_GUARD;
    if ( strcmp ( SectionName , "Kevin" ) == 0 ) return PERSON_KEVIN;
    if ( strcmp ( SectionName , "Jasmine" ) == 0 ) return PERSON_JASMINE;
    if ( strcmp ( SectionName , "Lukas" ) == 0 ) return PERSON_LUKAS;
    if ( strcmp ( SectionName , "SADD" ) == 0 ) return PERSON_SADD;
    if ( strcmp ( SectionName , "Tania" ) == 0 ) return PERSON_TANIA;
    if ( strcmp ( SectionName , "SACD" ) == 0 ) return PERSON_SACD;
    if ( strcmp ( SectionName , "Koan" ) == 0 ) return PERSON_KOAN;
    if ( strcmp ( SectionName , "Boris" ) == 0 ) return PERSON_BORIS;
    if ( strcmp ( SectionName , "Lina" ) == 0 ) return PERSON_LINA;
    if ( strcmp ( SectionName , "Serge" ) == 0 ) return PERSON_SERGE;
    if ( strncmp ( SectionName , "subdlg_" , 6 ) == 0 ) return PERSON_SUBDIALOG_DUMMY;

    DebugPrintf ( -1000 , "\n--------------------\nSectionName: %s." , SectionName );
    ErrorMessage ( __FUNCTION__  , "\
There was a dialogue section to be used with a droid, that does not have a \n\
corresponding chat flags array index." ,
			       PLEASE_INFORM, IS_FATAL );
    return (-1);
    
}; // int ResolveDialogSectionToChatFlagsIndex ( Enemy ChatDroid )


/**
 * This function plants a cookie, i.e. sets a new text string with the
 * purpose of serving as a flag.  These flags can be set/unset from the dialog
 * file and used from within there and they get stored and loaded with
 * every gave via the tux_t structure.
 */
void
PlantCookie ( char* CookieString )
{
    int i;
    
    //--------------------
    // First a security check against attempts to plant too long cookies...
    //
    if ( strlen ( CookieString ) >= MAX_COOKIE_LENGTH -1 )
    {
	fprintf( stderr, "\n\nCookieString: %s\n" , CookieString );
	ErrorMessage ( __FUNCTION__  , "\
There was a cookie given that exceeds the maximal length allowed for a\n\
cookie to be set in FreedroidRPG.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Maybe the cookie string received still has the ':' attached at
    // the end.  In this case we first remove the ':'.
    //
    if ( strlen ( CookieString ) > 1 )
    {
	DebugPrintf ( 1 , "\nLast character of cookie text received: %c." , 
		      CookieString [ strlen ( CookieString ) - 1 ] );
	if ( CookieString [ strlen ( CookieString ) - 1 ] == ':' )
	{
	    CookieString [ strlen ( CookieString ) - 1 ] = 0 ;
	    DebugPrintf ( 1 , "\nRemoving trailing ':' character from cookie text...");
	}
    }
    
    //--------------------
    // Check if maybe the cookie has already been set.  In this case we would
    // not have to do anything...
    //
    for ( i = 0 ; i < MAX_COOKIES ; i ++ )
    {
	if ( strlen ( Me . cookie_list [ i ] ) <= 0 )
	{
	    if ( !strcmp ( Me . cookie_list [ i ] , CookieString ) )
	    {
		DebugPrintf ( 0 , "\n\nTHAT COOKIE WAS ALREADY SET... DOING NOTHING...\n\n" );
		return;
	    }
	}
    }
    
    //--------------------
    // Now we find a good new and free position for our new cookie...
    //
    for ( i = 0 ; i < MAX_COOKIES ; i ++ )
    {
	if ( strlen ( Me . cookie_list [ i ] ) <= 0 )
	{
	    break;
	}
    }
    
    //--------------------
    // Maybe the position we have found is the last one.  That would mean too
    // many cookies, a case that should never occur in FreedroidRPG and that is
    // a considered a fatal error...
    //
    if ( i >= MAX_COOKIES ) 
    {
	fprintf( stderr, "\n\nCookieString: %s\n" , CookieString );
	ErrorMessage ( __FUNCTION__  , "\
There were no more free positions available to store this cookie.\n\
This should not be possible without a severe bug in FreedroidRPG.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Now that we know that we have found a good position for storing our
    // new cookie, we can do it.
    //
    strcpy ( Me . cookie_list [ i ] , CookieString );
    DebugPrintf ( 0 , "\n\nNEW COOKIE STORED:  Position=%d Text='%s'.\n\n" , 
		  i , CookieString );
    
    
}; // void PlantCookie ( char* CookieString )


/**
 * This function deletes planted cookie, i.e. delete a text string with the
 * purpose of serving as a flag.  These flags can be set/unset from the dialog
 * file and used from within there and they get stored and loaded with
 * every gave via the tux_t structure.
 */
void
DeleteCookie ( char* CookieString)
{
    DebugPrintf ( -4 , "\nDeleting cookie: '%s'." , CookieString );
    
    if ( strlen ( CookieString ) > 1 )
    {
	DebugPrintf ( 1 , "\nLast character of cookie text received: %c." , 
		      CookieString [ strlen ( CookieString ) - 1 ] );
	if ( CookieString [ strlen ( CookieString ) - 1 ] == ':' )
	{
	    CookieString [ strlen ( CookieString ) - 1 ] = 0 ;
	    DebugPrintf ( 1 , "\nRemoving trailing ':' character from cookie text...");
	}
    }

    int i;
    for ( i = 0 ; i < MAX_COOKIES ; i ++ )
	{
	DebugPrintf ( 1 , "\nCookie entry to compare to: %s." , Me . cookie_list [ i ] );
	if ( ! strlen ( Me . cookie_list [ i ] ) ) continue;
	if ( ! strcmp ( Me . cookie_list [ i ] , CookieString ) ) 
		break;
	//--------------------
	// Now some extra safety, cause the ':' termination character might still be on 
	// the cookie or on the comparison string
	//
	if ( strcmp ( Me . cookie_list [ i ] , CookieString ) >= ( ( int ) strlen ( CookieString ) ) ) 
	    break; 
	}
	
    if (i == MAX_COOKIES){
        DebugPrintf ( -4 , "Cookie not found.");
    } else {
	strcpy ( Me . cookie_list [ i ] , "" ) ;
	DebugPrintf ( 1 , "Cookie deleted.");
    }
}; // void DeleteCookie ( char* CookieString )


/**
 * This function restores all chat-with-friendly-droid variables to their
 * initial values.  This means, that NOT ALL FLAGS CAN BE SET HERE!!  Some
 * of them must remain at their current values!!! TAKE CARE!!
 */
void
RestoreChatVariableToInitialValue( )
{
    //--------------------
    // You can always ask the moron 614 bots the same things and they
    // will always respond in the very same manner, so no need to
    // remember anything that has been talked in any previous conversation
    // with them.
    //
    Me . Chat_Flags [ PERSON_614 ] [ 0 ] = 1 ;
    Me . Chat_Flags [ PERSON_614 ] [ 1 ] = 1 ;
    Me . Chat_Flags [ PERSON_614 ] [ 2 ] = 1 ;
    Me . Chat_Flags [ PERSON_614 ] [ 3 ] = 1 ;
    
    
}; // void RestoreChatVariableToInitialValue( )

/**
 * During the Chat with a friendly droid or human, there is a window with
 * the full text transcript of the conversation so far.  This function is
 * here to display said text window and it's content, scrolled to the
 * position desired by the player himself.
 */
void
display_current_chat_protocol ( int background_picture_code , enemy* ChatDroid , int with_update )
{
    SDL_Rect Subtitle_Window;
    int lines_needed ;
    int protocol_offset ;
    
#define AVERAGE_LINES_IN_PROTOCOL_WINDOW (UNIVERSAL_COORD_H(9))

    SetCurrentFont( FPS_Display_BFont );
    
    Subtitle_Window . x = CHAT_SUBDIALOG_WINDOW_X; 
    Subtitle_Window . y = CHAT_SUBDIALOG_WINDOW_Y; 
    Subtitle_Window . w = CHAT_SUBDIALOG_WINDOW_W;
    Subtitle_Window . h = CHAT_SUBDIALOG_WINDOW_H;

    //--------------------
    // First we need to know where to begin with our little display.
    //
    lines_needed = GetNumberOfTextLinesNeeded ( chat_protocol , Subtitle_Window , TEXT_STRETCH );
    DebugPrintf ( 1 , "\nLines needed: %d. " , lines_needed );
    
    if ( lines_needed <= AVERAGE_LINES_IN_PROTOCOL_WINDOW ) 
    {
	//--------------------
	// When there isn't anything to scroll yet, we keep the default
	// position and also the users clicks on up/down button will be
	// reset immediately
	//
	protocol_offset = 0 ;
	chat_protocol_scroll_override_from_user = 0 ;
    }
    else
	protocol_offset = ((int) ( 0.99 + FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) *
	     ( lines_needed - AVERAGE_LINES_IN_PROTOCOL_WINDOW + chat_protocol_scroll_override_from_user )) ;

    //printf("Poffset %d, sco %d, fh %d\n", protocol_offset, chat_protocol_scroll_override_from_user, FontHeight(GetCurrentFont()));
    
    //--------------------
    // Prevent the player from scrolling 
    // too high (negative protocol offset)
    //
    if ( protocol_offset < 0 )
    {
	chat_protocol_scroll_override_from_user ++ ;
	protocol_offset = 0 ;
    }
   
    //--------------------
    // Now we need to clear this window, cause there might still be some
    // garbage from the previous subtitle in there...
    //
    PrepareMultipleChoiceDialog ( ChatDroid , FALSE );
    
    //--------------------
    // Now we can display the text and update the screen...
    //
    SDL_SetClipRect( Screen, NULL );
    Subtitle_Window . x = CHAT_SUBDIALOG_WINDOW_X; 
    Subtitle_Window . y = CHAT_SUBDIALOG_WINDOW_Y; 
    Subtitle_Window . w = CHAT_SUBDIALOG_WINDOW_W;
    Subtitle_Window . h = CHAT_SUBDIALOG_WINDOW_H;
    DisplayText ( chat_protocol , Subtitle_Window.x , Subtitle_Window.y - protocol_offset , &Subtitle_Window , TEXT_STRETCH );
    if ( protocol_offset > 0 ) 
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_UP_BUTTON );
    else
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_OFF_BUTTON );
    if ( lines_needed <= AVERAGE_LINES_IN_PROTOCOL_WINDOW ) 
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_OFF2_BUTTON );
    else
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_DOWN_BUTTON );
    if ( with_update ) our_SDL_update_rect_wrapper ( Screen , Subtitle_Window.x , Subtitle_Window.y , Subtitle_Window.w , Subtitle_Window.h );
    
}; // void display_current_chat_protocol ( int background_picture_code , int with_update )

/**
 * This function should first display a subtitle and then also a sound
 * sample.  It is not very sophisticated or complicated, but nevertheless
 * important, because this combination does indeed occur so often.
 */
void
GiveSubtitleNSample( char* SubtitleText , char* SampleFilename , enemy* ChatDroid , int with_update )
{

    strcat ( chat_protocol , SubtitleText );
    strcat ( chat_protocol , "\n" );
    
    if ( strcmp ( SubtitleText , "NO_SUBTITLE_AND_NO_WAITING_EITHER" ) )
    {
	display_current_chat_protocol ( CHAT_DIALOG_BACKGROUND_PICTURE_CODE , ChatDroid , with_update );
	PlayOnceNeededSoundSample( SampleFilename , TRUE , FALSE );
    }
    else
    {
	PlayOnceNeededSoundSample( SampleFilename , FALSE , FALSE );
    }
}; // void GiveSubtitleNSample( char* SubtitleText , char* SampleFilename )

/**
 * Chat options may contain some extra commands, that specify things that
 * the engine is supposed to do, like open a shop interface, drop some
 * extra item to the inventory, remove an item from inventory, assign a
 * mission, mark a mission as solved and such things.
 *
 * This function is supposed to decode such extra commands and then to
 * execute the desired effect as well.
 *
 */
int
ExecuteChatExtra ( char* ExtraCommandString , Enemy ChatDroid )
{
    int TempValue;
    char WorkString[5000];
    char *TempMessage;
    item NewItem;
    char tmp_filename [ 5000 ] ;
    char fpath[2048];
    int mis_num , mis_diary_entry_num;

    if ( ! strcmp ( ExtraCommandString , "BreakOffAndBecomeHostile" ) )
	{
	ChatDroid -> is_friendly = FALSE ;
	ChatDroid -> combat_state = ATTACK ;
	return 1;
	}
    else if ( ! strcmp ( ExtraCommandString , "DropDead" ) )
	{
	hit_enemy(ChatDroid, ChatDroid->energy + 1, 0, -1, 0);
	ChatDroid -> energy = 0;
	return (1);
	}
    else if ( ! strcmp ( ExtraCommandString , "EverybodyBecomesHostile" ) )
	{
	enemy *erot;
	BROWSE_ALIVE_BOTS(erot)	
	    {
	    erot -> is_friendly = FALSE ;
	    }
	SwitchBackgroundMusicTo(BIGFIGHT_BACKGROUND_MUSIC_SOUND);
	}
    else if ( ! strcmp ( ExtraCommandString , "SetCompletelyFixedProperty" ) )
	{
	ChatDroid -> CompletelyFixed = TRUE ;
	ChatDroid -> follow_tux = FALSE;
	}
    else if ( ! strcmp ( ExtraCommandString , "SetFollowTuxProperty" ) )
	{
	ChatDroid -> follow_tux = TRUE ;
	ChatDroid -> CompletelyFixed = FALSE ;
	}
    else if ( ! strcmp ( ExtraCommandString , "SetMoveFreelyProperty" ) )
	{
	ChatDroid -> CompletelyFixed = FALSE ;
	ChatDroid -> follow_tux = FALSE ;
	}
    else if ( ! strcmp ( ExtraCommandString , "KillTux" ) )
	{
	Me . energy = 0;
	}
    else if ( ! strcmp ( ExtraCommandString , "MakeTuxTownGuardMember" ) )
	{
	Me . is_town_guard_member = TRUE ;
	Mission_Status_Change_Sound();
	}
    else if ( ! strcmp ( ExtraCommandString , "IncreaseMeleeWeaponSkill" ) )
	{
	ImproveSkill(&Me . melee_weapon_skill); 
	SetNewBigScreenMessage( "Melee fighting ability improved!" );
	}
    else if ( ! strcmp ( ExtraCommandString , "IncreaseRangedWeaponSkill" ) )
	{
	ImproveSkill(&Me . ranged_weapon_skill); 
	SetNewBigScreenMessage( "Ranged combat ability improved!" );
	}
    else if ( ! strcmp ( ExtraCommandString , "IncreaseSpellcastingSkill" ) )
	{
	ImproveSkill(&Me . spellcasting_skill); 
	SetNewBigScreenMessage( "Programming ability improved!" );
	}
    else if ( ! strcmp ( ExtraCommandString , "IncreaseHackingSkill" ) )
	{
	ImproveSkill(&Me . hacking_skill); 
	SetNewBigScreenMessage( "Hacking ability improved!" );
	}
    else if ( CountStringOccurences ( ExtraCommandString , "IncreaseProgramVersionName:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked increasing program version. --> have to decode... " );
	char * pos = strstr(ExtraCommandString, "IncreaseProgramVersionName");
	pos += strlen("IncreaseProgramVersionName:");
	char * pos2 = pos;
	while ( * pos2 != '\"') pos2++;
	char pname [ pos2 - pos + 1 ];
	strncpy(pname, pos, pos2 - pos);
	pname[pos2-pos] = 0;
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Program name is: %d." , pname );
	Me . base_skill_level [ get_program_index_with_name(pname) ] ++;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "TuxLoseHP:" ) )
	{
	char * pos = strstr(ExtraCommandString, "TuxLoseHP");
	pos += strlen("TuxLoseHP:");
	while ( ! isdigit ( * pos ) || *pos != '-' ) pos ++;
	char * pos2 = pos;
	while ( isdigit(*pos2) || *pos2 == '-' ) pos2++;
	char pname [ pos2 - pos + 1 ];
	strncpy(pname, pos, pos2 - pos);
	pname[pos2-pos] = 0;
	Me . energy -= atoi ( pname ) ;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "DeleteItem:" ) )
	{
	char * pos = strstr(ExtraCommandString, "DeleteItem");
	pos += strlen("DeleteItem:");
	while ( isspace(*pos) ) pos ++;
	if ( isdigit( * pos ) )
	    ErrorMessage(__FUNCTION__, "A chat extra command tried to specify an item type number, but would be required to use a name instead. This command was %s\n", PLEASE_INFORM, IS_FATAL, ExtraCommandString);

	char * pos2 = pos;
	while ( (*pos2) != ':' && *pos2 != '\0' ) 
	    pos2 ++;

	char pname[100];
	int multiplicity = 1;

	strncpy(pname, pos, pos2-pos);
	pname[pos2-pos] = 0;
	TempValue = GetItemIndexByName(pname);

	if(*pos2 == ':')
	    {
	    pos=pos2 + 1;
	    pos2 ++;
	    while( isdigit(*pos2) ) pos2++;
	    strncpy(pname, pos, pos2-pos);
	    multiplicity = atoi(pname);
	    }
	else multiplicity = 1;

	DeleteInventoryItemsOfType( TempValue , multiplicity );
	}
    else if ( CountStringOccurences ( ExtraCommandString , "GiveItem:" ) )
	{
	char * pos = strstr(ExtraCommandString, "GiveItem");
	pos += strlen("GiveItem:");
	while ( isspace(*pos) ) pos ++;
	if ( isdigit( * pos ) )
	    ErrorMessage(__FUNCTION__, "A chat extra command tried to specify an item type number, but would be required to use a name instead. This command was %s\n", PLEASE_INFORM, IS_FATAL, ExtraCommandString);

	char * pos2 = pos;
	while ((*pos2) != ':' && *pos2 != '\0') 
	    {
	    pos2 ++;
	    }
	char pname[100];
	strncpy(pname, pos, pos2-pos);
	pname[pos2-pos] = 0;

	NewItem.type = GetItemIndexByName(pname)  ;
	NewItem.prefix_code = (-1);
	NewItem.suffix_code = (-1);
	FillInItemProperties ( &NewItem , TRUE , 1);

	if(*pos2 == ':')
	    {
	    pos=pos2 + 1;
	    pos2 ++;
	    while( isdigit(*pos2) ) pos2++;
	    strncpy(pname, pos, pos2-pos);
	    NewItem.multiplicity = atoi(pname);
	    }
	else NewItem.multiplicity = 1;


	//--------------------
	// Either we put the new item directly into inventory or we issue a warning
	// that there is no room and then drop the item to the floor directly under 
	// the current Tux position.  That can't fail, right?
	//
	if ( !TryToIntegrateItemIntoInventory ( & NewItem , NewItem.multiplicity ) )
	    {
	    DropItemToTheFloor ( &NewItem , Me . pos . x , Me . pos . y , Me . pos. z ) ;
	    SetNewBigScreenMessage( _("1 Item received (on floor)") );
	    }
	else
	    {
	    SetNewBigScreenMessage( _("1 Item received!") );
	    }
	}
    else if ( CountStringOccurences ( ExtraCommandString , "OpenQuestDiaryEntry:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked enabling a new quest diary entry: %s." ,
		ExtraCommandString + strlen ( "OpenQuestDiaryEntry:" ) );
	strncpy ( WorkString , ExtraCommandString + strlen ( "OpenQuestDiaryEntry:" ) , 10 );
	WorkString [ 10 ] = 0 ;
	sscanf ( WorkString , "M%dE%d:" , &mis_num , &mis_diary_entry_num );
	DebugPrintf ( CHAT_DEBUG_LEVEL	, "\nreceived mission number: %d and diary entry number: %d." , 
		mis_num , mis_diary_entry_num );
	quest_browser_enable_new_diary_entry ( mis_num , mis_diary_entry_num );
	}
    else if ( CountStringOccurences ( ExtraCommandString , "ExecuteActionWithLabel:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked execution of action with label: %s. Doing it... " ,
		ExtraCommandString + strlen ( "ExecuteActionWithLabel:" ) );
	ExecuteActionWithLabel ( ExtraCommandString + strlen ( "ExecuteActionWithLabel:" ) ) ;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "ExecuteSubdialog:" ) )
	{
	strcpy ( tmp_filename , ExtraCommandString + strlen ( "ExecuteSubdialog:" ) ) ;
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked start of SUBDIALOG! with label: %s. Doing it... " ,
		tmp_filename );

	push_or_pop_chat_roster ( PUSH_ROSTER );

	//--------------------
	// We have to load the subdialog specified...
	//
	// InitChatRosterForNewDialogue(  );
	strcat ( tmp_filename , ".dialog" );
	char finaldir[50];
	sprintf(finaldir, "%s", DIALOG_DIR);
	find_file (tmp_filename , finaldir, fpath, 0);

	int i, j;
	for (i = 0; i < MAX_ANSWERS_PER_PERSON; i ++)
	    {
	    Me . Chat_Flags [ PERSON_SUBDIALOG_DUMMY    ] [ i ] = (ChatRoster [ i ] . position_x != -1);
	    }

	for (i = 0; i < MAX_ANSWERS_PER_PERSON; i ++)
	    {
	    for (j = 0; j < MAX_ANSWERS_PER_PERSON; j ++)
		{
		if( i == (ChatRoster [ i ] . change_option_nr [ j ])) continue;
		if(ChatRoster [ i ] . change_option_nr [ j ] > 0  &&  ChatRoster [ i ] . change_option_to_value [ j ] == 1)
		    {
		    Me . Chat_Flags [ PERSON_SUBDIALOG_DUMMY ] [ ChatRoster [ i ] . change_option_nr [ j ] ] = 0;
		    }
		}

	    if ( strlen ( ChatRoster [ i ] . on_goto_condition ) )
		{
		Me . Chat_Flags [ PERSON_SUBDIALOG_DUMMY ] [ ChatRoster [ i ] . on_goto_first_target ] = 0;
		Me . Chat_Flags [ PERSON_SUBDIALOG_DUMMY ] [ ChatRoster [ i ] . on_goto_second_target ] = 0;
		}

	    if ( ChatRoster [ i ] . link_target )
		Me . Chat_Flags [ PERSON_SUBDIALOG_DUMMY ] [ ChatRoster [ i ] . link_target ] = 0;
	    }


	LoadChatRosterWithChatSequence ( fpath );
	DoChatFromChatRosterData( PERSON_SUBDIALOG_DUMMY , ChatDroid , FALSE );

	push_or_pop_chat_roster ( POP_ROSTER );

	if ( ! ChatDroid -> energy ) //if the droid was killed, end the chat
	    return 1;

	}
    else if ( CountStringOccurences ( ExtraCommandString , "PlantCookie:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked planting of a cookie: %s. Doing it... " ,
		ExtraCommandString + strlen ( "PlantCookie:" ) );
	PlantCookie ( ExtraCommandString + strlen ( "PlantCookie:" ) ) ;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "DeleteCookie:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked deleting of a cookie: %s. Doing it... " ,
		ExtraCommandString + strlen ( "DeleteCookie:" ) );
	DeleteCookie ( ExtraCommandString + strlen ( "DeleteCookie:" ) ) ;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "InitTradeWithCharacter:" ) )
	{
	TempValue = ResolveDialogSectionToChatFlagsIndex ( ExtraCommandString + strlen ( "InitTradeWithCharacter:" ) ) ;
	InitTradeWithCharacter( TempValue );
	}
    else if ( CountStringOccurences ( ExtraCommandString , "AssignMission:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked assigning of mission. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AssignMission:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Mission to assign is: %d." , TempValue );
	AssignMission ( TempValue );
	}
    else if ( CountStringOccurences ( ExtraCommandString , "MarkMissionComplete:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked marking a mission as completed. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "MarkMissionComplete:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Mission to mark as complete is: %d." , TempValue );
	Me . AllMissions[ TempValue ] . MissionIsComplete = TRUE;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "AddExperienceBonus:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding an exerpience bonus. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AddExperienceBonus:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...bonus to add is: %d." , TempValue );
	Me . Experience += TempValue;
	sprintf( WorkString , "+%d Experience Points" , TempValue );
	SetNewBigScreenMessage ( WorkString );
	}
    else if ( CountStringOccurences ( ExtraCommandString , "AddBigScreenMessageBUT_WITH_TERMINATION_CHARACTER_PLEASE:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding a big screen message. --> have to decode... " );
	TempMessage = ReadAndMallocStringFromData ( ExtraCommandString , "AddBigScreenMessageBUT_WITH_TERMINATION_CHARACTER_PLEASE:" , ":" ) ;
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...message is: %s." , TempMessage );
	SetNewBigScreenMessage ( TempMessage );
	free ( TempMessage ) ;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "AddBaseMagic:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding some base magic points. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AddBaseMagic:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of magic points mentioned is: %d." , TempValue );
	Me . base_magic += TempValue;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "AddBaseDexterity:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding some base dexteritypoints. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AddBaseDexterity:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of dexterity points mentioned is: %d." , TempValue );
	Me . base_dexterity += TempValue;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "SubtractPointsToDistribute:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked subtracting points to distribute. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "SubtractPointsToDistribute:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of points mentioned is: %d." , TempValue );
	Me . points_to_distribute -= TempValue;
	}
    else if ( CountStringOccurences ( ExtraCommandString , "SubtractGold:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked subtracting gold. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "SubtractGold:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of gold mentioned is: %d." , TempValue );
	Me . Gold -= TempValue;
	sprintf ( WorkString , "%d bucks given" , TempValue );
	SetNewBigScreenMessage ( WorkString );
	}
    else if ( CountStringOccurences ( ExtraCommandString , "ForceBotRespawnOnLevel:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked forcing bot respawn on level --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "ForceBotRespawnOnLevel:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Level to respawn bots on is: %d." , TempValue );
	respawn_level ( TempValue );
	}
    else if ( CountStringOccurences ( ExtraCommandString , "AddGold:" ) )
	{
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding gold. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AddGold:" , "%d" , 
		&TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of gold mentioned is: %d." , TempValue );
	Me . Gold += TempValue;
	sprintf ( WorkString , "%d bucks received" , TempValue );
	SetNewBigScreenMessage ( WorkString );
	}
    else if ( ! strcmp ( ExtraCommandString , "CompletelyHealTux" ) )
	{
	Me . energy = Me . maxenergy ;
	}
    else if ( !strcmp ( ExtraCommandString , "EndDialog" ) )
	{
	return ( 1 );
	}
    else 
	{
	fprintf( stderr, "\n\nExtraCommandString: %s \n" , ExtraCommandString );
	ErrorMessage ( __FUNCTION__  , "\
		ERROR:  UNKNOWN COMMAND STRING GIVEN!",
		PLEASE_INFORM, IS_FATAL );
	}
    return ( 0 );
}; // int ExecuteChatExtra ( char* ExtraCommandString )

/**
 *
 *
 */
void
make_sure_chat_portraits_loaded_for_this_droid ( Enemy this_droid )
{
    SDL_Surface* Small_Droid;
    SDL_Surface* Large_Droid;
    char fpath[2048];
    char fname[500];
    int i;
    int model_number;
    static int first_call = TRUE ;
    static int this_type_has_been_loaded [ ENEMY_ROTATION_MODELS_AVAILABLE ] ;
    
    //--------------------
    // We make sure we only load the portrait files once and not
    // every time...
    //
    if ( first_call )
    {
	for ( i = 0 ; i < ENEMY_ROTATION_MODELS_AVAILABLE ; i ++ )
	    this_type_has_been_loaded [ i ] = FALSE ;
    }
    first_call = FALSE ;
    
    //--------------------
    // We look up the model number for this chat partner.
    //
    model_number = this_droid -> type ;
    
    //--------------------
    // We should make sure, that we don't double-load images that we have loaded
    // already, thereby wasting more resources, including OpenGL texture positions.
    //
    if ( this_type_has_been_loaded [ model_number ] )
	return;
    this_type_has_been_loaded [ model_number ] = TRUE ;
    
    //--------------------
    // At first we try to load the image, that is named after this
    // chat section.  If that succeeds, perfect.  If not, we'll revert
    // to a default image.
    //
    strcpy( fname, "droids/" );
    strcat( fname, PrefixToFilename [ model_number ] ) ;
    strcat( fname , "/portrait.png" );
    find_file (fname, GRAPHICS_DIR, fpath, 0);
    DebugPrintf ( 2 , "\nFilename used for portrait: %s." , fpath );
    
    Small_Droid = our_IMG_load_wrapper (fpath) ;
    if ( Small_Droid == NULL )
    {
	strcpy( fname, "droids/" );
	strcat( fname, "DefaultPortrait.png" );
	find_file (fname, GRAPHICS_DIR, fpath, 0);
	Small_Droid = our_IMG_load_wrapper ( fpath ) ;
    }
    if ( Small_Droid == NULL )
    {
	fprintf( stderr, "\n\nfpath: %s \n" , fpath );
	ErrorMessage ( __FUNCTION__  , "\
It wanted to load a small portrait file in order to display it in the \n\
chat interface of Freedroid.  But:  Loading this file has ALSO failed.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    Large_Droid = zoomSurface( Small_Droid , (float) Droid_Image_Window . w / (float) Small_Droid -> w , 
			       (float) Droid_Image_Window . w / (float) Small_Droid -> w , 0 );
    
    SDL_FreeSurface( Small_Droid );
    
    if ( use_open_gl )
	{
	chat_portrait_of_droid [ model_number ] . surface = SDL_CreateRGBSurface(0, Large_Droid -> w, Large_Droid -> h, 32, rmask, gmask, bmask, amask);
	SDL_SetAlpha(Large_Droid, 0, SDL_ALPHA_OPAQUE);
	our_SDL_blit_surface_wrapper ( Large_Droid, NULL, chat_portrait_of_droid [ model_number ] . surface, NULL );
	SDL_FreeSurface ( Large_Droid );
	}
    else chat_portrait_of_droid [ model_number ] . surface = Large_Droid;
    
    
}; // void make_sure_chat_portraits_loaded_for_this_droid ( Enemy this_droid )

/**
 * This function prepares the chat background window and displays the
 * image of the dialog partner and also sets the right font.
 */
void
PrepareMultipleChoiceDialog ( Enemy ChatDroid , int with_flip )
{
    //--------------------
    // The dialog will always take more than a few seconds to process
    // so we need to prevent framerate distortion...
    //
    Activate_Conservative_Frame_Computation( );
    
    //--------------------
    // We make sure that all the chat portraits we might need are
    // loaded....
    //
    make_sure_chat_portraits_loaded_for_this_droid ( ChatDroid ) ;
    
    //--------------------
    // We select small font for the menu interaction...
    //
    SetCurrentFont( FPS_Display_BFont );
    
//    AssembleCombatPicture ( USE_OWN_MOUSE_CURSOR ) ;
    blit_special_background ( CHAT_DIALOG_BACKGROUND_PICTURE_CODE );
    our_SDL_blit_surface_wrapper ( chat_portrait_of_droid [ ChatDroid -> type ] . surface , NULL , 
				   Screen , &Droid_Image_Window );
    
    if ( with_flip ) 
	our_SDL_flip_wrapper( Screen );
        
}; // void PrepareMultipleChoiceDialog ( int Enum )

/**
 * It is possible to specify a conditional goto command from the chat
 * information file 'Freedroid.dialogues'.  But in order to execute this
 * conditional jump, we need to know whether a statment given as pure text
 * string is true or not.  This function is intended to find out whether
 * it is true or not.
 */
int
TextConditionIsTrue ( char* ConditionString )
{
    int TempValue;
    char* CookieText;
    int i ;
    int old_town_mission_score;
    
    if ( CountStringOccurences ( ConditionString , "MissionComplete" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for mission complete." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String referred to mission number: %d." , TempValue );
	
	if ( Me . AllMissions [ TempValue ] . MissionIsComplete )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "MissionAssigned" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for mission assigned." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String referred to mission number: %d." , TempValue );
	
	if ( Me . AllMissions [ TempValue ] . MissionWasAssigned )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "HaveItemWithName" ) )
    {

    char * pos = strstr(ConditionString, "HaveItemWithName");
    pos += strlen("HaveItemWithName:");
    while ( isspace(*pos) ) pos ++;
    if ( isdigit( * pos ) )
	ErrorMessage(__FUNCTION__, "A chat extra command tried to specify an item type number, but would be required to use a name instead. This command was %s\n", PLEASE_INFORM, IS_FATAL, ConditionString);

    char * pos2 = pos;
    while ((*pos2) != ':' && *pos2 != '\0')
	{
	pos2 ++;
	}
    char pname[100];
    strncpy(pname, pos, pos2-pos);
    pname[pos2-pos] = 0;

    if ( CountItemtypeInInventory ( GetItemIndexByName(pname) ) )
	return ( TRUE );
    else
	return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "PointsToDistributeAtLeast" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for available skill points to distribute." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned number of points: %d." , TempValue );
	
	if ( Me . points_to_distribute >= TempValue )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "GoldIsLessThan" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for amount of gold Tux has on him." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned concrete amout of gold: %d." , TempValue );
	
	if ( Me . Gold < TempValue )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "MeleeSkillLesserThan" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for melee skill lesser than value." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned level: %d." , TempValue );
	
	if ( Me . melee_weapon_skill < TempValue )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "CookieIsPlanted" ) )
    {
	DebugPrintf ( -4 , "\nCondition String identified as question for cookie planted." );
	
	CookieText = 
	    ReadAndMallocStringFromData ( ConditionString , "CookieIsPlanted:" , ":" ) ;
	DebugPrintf ( -4 , "\nCookieText mentioned: '%s'." , CookieText );
	
	for ( i = 0 ; i < MAX_COOKIES ; i ++ )
	{
	    DebugPrintf ( 1 , "\nCookie entry to compare to: %s." , Me . cookie_list [ i ] );
	    if ( ! strlen ( Me . cookie_list [ i ] ) ) continue;
	    if ( ! strcmp ( Me . cookie_list [ i ] , CookieText ) ) 
		return ( TRUE );
	    //--------------------
	    // Now some extra safety, cause the ':' termination character might still be on 
	    // the cookie or on the comparison string
	    //
	    if ( strcmp ( Me . cookie_list [ i ] , CookieText ) >= ( ( int ) strlen ( CookieText ) ) ) 
		return ( TRUE ); 
	}
	
	free ( CookieText );
	
	return ( FALSE );
	
    }
    else if ( CountStringOccurences ( ConditionString , "OldTownMissionScoreAtLeast" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for old town mission score." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned mission score of : %d old town mission points." , 
		      TempValue );
	
	old_town_mission_score = 0 ;
	if ( Me . AllMissions [ 0 ] . MissionIsComplete )
	    old_town_mission_score += 10 ;
	if ( Me . AllMissions [ 1 ] . MissionIsComplete )
	    old_town_mission_score += 15 ;
	if ( Me . AllMissions [ 2 ] . MissionIsComplete )
	    old_town_mission_score += 10 ;
	if ( Me . AllMissions [ 3 ] . MissionIsComplete )
	    old_town_mission_score += 10 ;
	if ( Me . AllMissions [ 4 ] . MissionIsComplete )
	    old_town_mission_score += 20 ;
	if ( Me . AllMissions [ 5 ] . MissionIsComplete )
	    old_town_mission_score += 15 ;
	
	if ( old_town_mission_score >= TempValue )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "True" ) )
    {
        return ( TRUE );
    }
    else if ( CountStringOccurences ( ConditionString , "False" ) )
    {
        return ( FALSE );
    }

    
    fprintf( stderr, "\n\nConditionString: %s. \n" , ConditionString );
    ErrorMessage ( __FUNCTION__  , "\
There were was a Condition string (most likely used for an on-goto-command\n\
in the Freedroid.dialogues file, that contained a seemingly bogus condition.\n\
Freedroid was unable to determine the type of said condition.",
			       PLEASE_INFORM, IS_FATAL );
    
    return ( TRUE );
}; // int TextConditionIsTrue ( char* ConditionString )

/**
 *
 *
 */
int
ProcessThisChatOption ( int MenuSelection , int ChatPartnerCode , Enemy ChatDroid )
{
    int i;
    int enddialog = 0;

    //--------------------
    // Now a menu section has been made.  We do the reaction:
    // say the samples and the replies, later we'll set the new option values
    //
    // But it might be the case that this option is more technical and not accompanied
    // by any reply.  This case must also be caught.
    //
    //printf("Processing option %d with partner %d\n", MenuSelection, ChatPartnerCode);
    if ( strcmp ( ChatRoster [ MenuSelection ] . option_sample_file_name , "NO_SAMPLE_HERE_AND_DONT_WAIT_EITHER" ) )
    {
	// PlayOnceNeededSoundSample( ChatRoster [ MenuSelection ] . option_sample_file_name , TRUE );
	strcat ( chat_protocol , "\1TUX:" );
	GiveSubtitleNSample ( L_(ChatRoster [ MenuSelection ] . option_text) ,
			      ChatRoster [ MenuSelection ] . option_sample_file_name , ChatDroid , TRUE ) ;
	strcat ( chat_protocol , "\2" );
    }
    
    //--------------------
    // Now we can proceed to execute
    // the rest of the reply that has been set up for this (the now maybe modified)
    // dialog option.
    //
    for ( i = 0 ; i < MAX_REPLIES_PER_OPTION ; i ++ )
    {
	//--------------------
	// Once we encounter an empty string here, we're done with the reply...
	//
	if ( ! strlen ( ChatRoster [ MenuSelection ] . reply_subtitle_list [ i ] ) ) 
	    break;
	
	GiveSubtitleNSample ( L_(ChatRoster [ MenuSelection ] . reply_subtitle_list [ i ]) ,
			      ChatRoster [ MenuSelection ] . reply_sample_list [ i ] , ChatDroid , TRUE ) ;
    }
    
    //--------------------
    // Now that all the replies have been made, we can start on changing
    // the option flags to their new values
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	//--------------------
	// Maybe all nescessary changes were made by now.  Then it's time
	// to quit...
	//
	if ( ChatRoster [ MenuSelection ] . change_option_nr [ i ] == (-1) ) 
	    break;
	
	Me . Chat_Flags [ ChatPartnerCode ] [ ChatRoster [ MenuSelection ] . change_option_nr [ i ] ] =
	    ChatRoster [ MenuSelection ] . change_option_to_value [ i ]  ;
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nChanged chat flag nr. %d to new value %d." ,
		      ChatRoster [ MenuSelection ] . change_option_nr[i] ,
		      ChatRoster [ MenuSelection ] . change_option_to_value[i] );
    }
    
    //--------------------
    // Maybe this option should also invoke some extra function like opening
    // a shop interface or something.  So we do this here.
    //
    for ( i = 0 ; i < MAX_EXTRAS_PER_OPTION ; i ++ )
    {
	//--------------------
	// Maybe all nescessary extras were executed by now.  Then it's time
	// to quit...
	//
	if ( !strlen ( ChatRoster [ MenuSelection ] . extra_list [ i ] ) )
	    break;
	
	DebugPrintf ( CHAT_DEBUG_LEVEL	, "\nStarting to invoke extra.  Text is: %s.\n" ,
		      ChatRoster [ MenuSelection ] . extra_list[i] );
	
	if ( ExecuteChatExtra ( ChatRoster [ MenuSelection ] . extra_list[i] , ChatDroid ) == 1 )
		enddialog = 1;
	
	if ( ! ChatDroid -> is_friendly )
		enddialog = 1 ;
	
	//--------------------
	// It can't hurt to have the overall background redrawn after each extra command
	// which could have destroyed the background by drawing e.g. a shop interface
	display_current_chat_protocol ( CHAT_DIALOG_BACKGROUND_PICTURE_CODE , ChatDroid , FALSE );
    }
    
    //--------------------
    // Maybe there was an ON-GOTO-CONDITION specified for this option.
    // Then of course we have to jump to the new location!!!
    //
    if ( strlen ( ChatRoster [ MenuSelection ] . on_goto_condition ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nON-GOTO-CONDITION ENCOUNTERED... CHECKING... " );
	if ( TextConditionIsTrue ( ChatRoster [ MenuSelection ] . on_goto_condition ) )
	{
	    DebugPrintf( CHAT_DEBUG_LEVEL , "...SEEMS TRUE... CONTINUING AT OPTION: %d. " , 
			 ChatRoster [ MenuSelection ] . on_goto_first_target );
	    MenuSelection = ChatRoster [ MenuSelection ] . on_goto_first_target ;
	}
	else
	{
	    DebugPrintf( CHAT_DEBUG_LEVEL , "...SEEMS FALSE... CONTINUING AT OPTION: %d. " , 
			 ChatRoster [ MenuSelection ] . on_goto_second_target );
	    MenuSelection = ChatRoster [ MenuSelection ] . on_goto_second_target ;
	}
	enddialog = ProcessThisChatOption ( MenuSelection , ChatPartnerCode , ChatDroid );
    }
    else if ( ChatRoster [ MenuSelection ] . link_target )
    {
        MenuSelection = ChatRoster [ MenuSelection ] . link_target ;
        enddialog = ProcessThisChatOption ( MenuSelection , ChatPartnerCode , ChatDroid );
    }

return (enddialog);  
  
}; // int ProcessThisChatOption ( int MenuSelection , int ChatPartnerCode , Enemy ChatDroid )


/**
 * This is the most important subfunction of the whole chat with friendly
 * droids and characters.  After the pure chat data has been loaded from
 * disk, this function is invoked to handle the actual chat interaction
 * and the dialog flow.
 */
void
DoChatFromChatRosterData( int ChatPartnerCode , Enemy ChatDroid , int clear_protocol )
{
    int i ;
    SDL_Rect Chat_Window;
    int MenuSelection = (-1) ;
    char* DialogMenuTexts[ MAX_ANSWERS_PER_PERSON ];
    char enemy_started_the_talk =  (ChatDroid -> combat_state == RUSH_TUX_AND_OPEN_TALK );

    //--------------------
    // We always should clear the chat protocol.  Only for SUBDIALOGS it is
    // suitable not to clear the chat protocol.
    //
    if ( clear_protocol )
    {
	if ( chat_protocol != NULL ) free ( chat_protocol );
	chat_protocol = MyMalloc ( 500000 ); // enough for any chat...
	strcpy ( chat_protocol , "\2--- Start of Dialog ---\n" );
	chat_protocol_scroll_override_from_user = 0 ;
	SetCurrentFont ( FPS_Display_BFont );
    }
    
    display_current_chat_protocol ( CHAT_DIALOG_BACKGROUND_PICTURE_CODE , ChatDroid , FALSE );
    
    Chat_Window . x = 242 ; Chat_Window . y = 100 ; Chat_Window . w = 380; Chat_Window . h = 314 ;
    
    //--------------------
    // We load the option texts into the dialog options variable..
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
    	DialogMenuTexts[i] = "";
	if ( strlen ( ChatRoster [ i ] . option_text ) )
	{
	    DialogMenuTexts [ i ] = L_(ChatRoster [ i ] . option_text) ;
	}
    }
    // DialogMenuTexts [ MAX_ANSWERS_PER_PERSON - 1 ] = " END ";

    //--------------------
    // Now we execute all the options that were marked to be executed
    // prior to dialog startup
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	if ( ChatRoster [ i ] . always_execute_this_option_prior_to_dialog_start )
	{
	    DebugPrintf ( CHAT_DEBUG_LEVEL , "\nExecuting option no. %d prior to dialog start.\n" , i );
	    if (( ProcessThisChatOption ( i , ChatPartnerCode , ChatDroid ) == 1))
		return;
	}
    }
    
    while (1)
    {
	//--------------------
	// Now maybe this is one of the bots that is rushing the Tux!  Then of course
	// we won't do the first selection, but instead immediately call the very first
	// dialog option and then continue with normal dialog.
	//
	if ( enemy_started_the_talk )
	{
	    MenuSelection = 0 ;
	    enemy_started_the_talk = 0;
	}
	else
	{
	    MenuSelection = ChatDoMenuSelectionFlagged ( _("What will you say?") , DialogMenuTexts , Me . Chat_Flags [ ChatPartnerCode ]  , 1 , -1 , FPS_Display_BFont , ChatDroid );
	    //--------------------
	    // We do some correction of the menu selection variable:
	    // The first entry of the menu will give a 1 and so on and therefore
	    // we need to correct this to more C style.
	    //
	    MenuSelection --;
	}
	if ( ( MenuSelection >= MAX_ANSWERS_PER_PERSON ) || ( MenuSelection < 0 ) )
	{
	    DebugPrintf ( 0 , "%s: Error: MenuSelection %i out of range!\n" , __FUNCTION__, MenuSelection );
	    MenuSelection = END_ANSWER ;
	}
	
	if (( ProcessThisChatOption ( MenuSelection , ChatPartnerCode , ChatDroid ) )  ==  1 )
	{
	    return;
	}
	
    }
    
}; // void DoChatFromChatRosterData( ... )

/**
 * When the Tux (or rather the player :) ) clicks on a friendly droid,
 * a chat menu will be invoked to do the communication with that friendly
 * character.  However, before the chat menu even starts up, there is a
 * certain time frame still spent in the isometric viewpoint where the
 * two characters (Tux and the chat partner) should turn to each other,
 * so the scene looks a bit more personal and realistic.  This function
 * handles that time interval and the two characters turning to each
 * other.
 */
void
DialogPartnersTurnToEachOther ( Enemy ChatDroid )
{
    int TurningDone = FALSE;
    float AngleInBetween;
    float WaitBeforeTurningTime = 0.00001 ;
    float WaitAfterTurningTime = 0.0001 ;
    int TurningStartTime;
    float OldAngle;
    float RightAngle;
    float TurningDirection;
    
#define TURN_SPEED 900.0

    //--------------------
    // We reset the mouse cursor shape and abort any other
    // mouse global mode operation.
    //
    global_ingame_mode = GLOBAL_INGAME_MODE_NORMAL ;
    
    Activate_Conservative_Frame_Computation();
    
    //--------------------
    // We make sure the one droid in question is in the standing and not
    // in the middle of the walking motion when turning to the chat partner...
    //
    // Calling AnimatEnemies() ONCE for this task seems justified...
    //
    ChatDroid -> speed . x = 0 ;
    ChatDroid -> speed . y = 0 ;
    
    //--------------------
    // At first do some waiting before the turning around starts...
    //
    TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
    while ( !TurningDone )
    {
	StartTakingTimeForFPSCalculation();       
	
	AssembleCombatPicture ( SHOW_ITEMS | USE_OWN_MOUSE_CURSOR ); 
	
	our_SDL_flip_wrapper ( Screen );
	
	if ( ( SDL_GetTicks() - TurningStartTime ) >= 1000.0 * WaitBeforeTurningTime )
	    TurningDone = TRUE;
	
	ComputeFPSForThisFrame();
    }
    
    //--------------------
    // Now we find out what the final target direction of facing should
    // be.
    //
    // For this we use the atan2, which gives angles from -pi to +pi.
    // 
    // Attention must be paid, since 'y' in our coordinates ascends when
    // moving down and descends when moving 'up' on the scren.  So that
    // one sign must be corrected, so that everything is right again.
    //
    RightAngle = ( atan2 ( - ( Me . pos . y - ChatDroid -> pos . y ) ,  
			   + ( Me . pos . x - ChatDroid -> pos . x ) ) * 180.0 / M_PI ) ;
    //
    // Another thing there is, that must also be corrected:  '0' begins
    // with facing 'down' in the current rotation models.  Therefore angle
    // 0 corresponds to that.  We need to shift again...
    //
    RightAngle += 90 ;
    
    //--------------------
    // Now it's time do determine which direction to move, i.e. if to 
    // turn to the left or to turn to the right...  For this purpose
    // we convert the current angle, which is between 270 and -90 degrees
    // to one between -180 and +180 degrees...
    //
    if ( RightAngle > 180.0 ) RightAngle -= 360.0 ; 
    
    // DebugPrintf ( 0 , "\nRightAngle: %f." , RightAngle );
    // DebugPrintf ( 0 , "\nCurrent angle: %f." , ChatDroid -> current_angle );
    
    //--------------------
    // Having done these preparations, it's now easy to determine the right
    // direction of rotation...
    //
    AngleInBetween = RightAngle - ChatDroid -> current_angle ;
    if ( AngleInBetween > 180 ) AngleInBetween -= 360;
    if ( AngleInBetween <= -180 ) AngleInBetween += 360;
    
    if ( AngleInBetween > 0 )
	TurningDirection = +1 ; 
    else 
	TurningDirection = -1 ; 
    
    //--------------------
    // Now we turn and show the image until both chat partners are
    // facing each other, mostly the chat partner is facing the Tux,
    // since the Tux may still turn around to somewhere else all the 
    // while, if the chose so
    //
    TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
    while ( !TurningDone )
    {
	StartTakingTimeForFPSCalculation();       
	
	AssembleCombatPicture ( SHOW_ITEMS | USE_OWN_MOUSE_CURSOR ); 
	our_SDL_flip_wrapper ( Screen );
	
	OldAngle = ChatDroid -> current_angle;
	
	ChatDroid -> current_angle = OldAngle + TurningDirection * Frame_Time() * TURN_SPEED ;
	
	//--------------------
	// In case of positive turning direction, we wait, till our angle is greater
	// than the right angle.
	// Otherwise we wait till our angle is lower than the right angle.
	//
	AngleInBetween = RightAngle - ChatDroid -> current_angle ;
	if ( AngleInBetween > 180 ) AngleInBetween -= 360;
	if ( AngleInBetween <= -180 ) AngleInBetween += 360;
	
	if ( ( TurningDirection > 0 ) && ( AngleInBetween < 0 ) ) TurningDone = TRUE;
	if ( ( TurningDirection < 0 ) && ( AngleInBetween > 0 ) ) TurningDone = TRUE;
	
	ComputeFPSForThisFrame();
    }
    
    //--------------------
    // Now that turning around is basically done, we still wait a few frames
    // until we start the dialog...
    //
    TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
    while ( !TurningDone )
    {
	StartTakingTimeForFPSCalculation();       
	
	AssembleCombatPicture ( SHOW_ITEMS | USE_OWN_MOUSE_CURSOR ); 
	our_SDL_flip_wrapper ( Screen );
	
	if ( ( SDL_GetTicks() - TurningStartTime ) >= 1000.0 * WaitAfterTurningTime )
	    TurningDone = TRUE;
	
	ComputeFPSForThisFrame();
    }
    
}; // void DialogPartnersTurnToEachOther ( Enemy ChatDroid )

/**
 * This is more or less the 'main' function of the chat with friendly 
 * droids and characters.  It is invoked directly from the user interface
 * function as soon as the player requests communication or there is a
 * friendly bot who rushes Tux and opens talk.
 */
void 
ChatWithFriendlyDroid( Enemy ChatDroid )
{
    int i ;
    SDL_Rect Chat_Window;
    char* DialogMenuTexts[ MAX_ANSWERS_PER_PERSON ];
    int ChatFlagsIndex = (-1);
    char fpath[2048];
    char tmp_filename[5000];
    
    //--------------------
    // Now that we know, that a chat with a friendly droid is planned, the 
    // friendly droid and the Tux should first turn to each other before the
    // real dialog is started...
    //
    DialogPartnersTurnToEachOther ( ChatDroid );
    
    Chat_Window . x = 242 ; Chat_Window . y = 100; Chat_Window . w = 380; Chat_Window . h = 314;
    
    //--------------------
    // First we empty the array of possible answers in the
    // chat interface.
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	DialogMenuTexts [ i ] = "" ;
    }
    
    //--------------------
    // This should make all the answering possibilities available
    // that are there without any prerequisite and that can be played
    // through again and again without any modification.
    //
    RestoreChatVariableToInitialValue( 0 ); 
    
    while ( MouseLeftPressed ( ) || MouseRightPressed() );
    
    //--------------------
    // We clean out the chat roster from any previous use
    //
    InitChatRosterForNewDialogue(  );
    
    ChatFlagsIndex = ResolveDialogSectionToChatFlagsIndex ( ChatDroid -> dialog_section_name ) ;
    
    //--------------------
    // Now that the 'LoadChatRosterWithChatSequence' function will also be
    // used from within the dialog editor, but with explicit path and file
    // name there, we can not assemble the file name inside of the function
    // but must do it here instead...
    //
    strcpy ( tmp_filename , ChatDroid -> dialog_section_name );
    strcat ( tmp_filename , ".dialog" );
    char finaldir[50];
    sprintf(finaldir, "%s", DIALOG_DIR);
    find_file (tmp_filename , finaldir, fpath, 0);
    LoadChatRosterWithChatSequence ( fpath );

    if ( ! Me . chat_character_initialized [ ChatFlagsIndex ] )
	{ // then we must initialize this character
        int i, j;
	for (i = 0; i < MAX_ANSWERS_PER_PERSON; i ++)
		{
		Me . Chat_Flags [ ChatFlagsIndex ] [ i ] =  (ChatRoster [ i ] . position_x != -1);
		}

	for (i = 0; i < MAX_ANSWERS_PER_PERSON; i ++)
		{
		for (j = 0; j < MAX_ANSWERS_PER_PERSON; j ++)
			{	
			if( i == (ChatRoster [ i ] . change_option_nr [ j ])) continue;
			if(ChatRoster [ i ] . change_option_nr [ j ] > 0  &&  ChatRoster [ i ] . change_option_to_value [ j ] == 1)
				{
				Me . Chat_Flags [ ChatFlagsIndex ] [ ChatRoster [ i ] . change_option_nr [ j ] ] = 0;
				}
			}

		if ( strlen ( ChatRoster [ i ] . on_goto_condition ) )
			{
			Me . Chat_Flags [ ChatFlagsIndex ] [ ChatRoster [ i ] . on_goto_first_target ] = 0;
			Me . Chat_Flags [ ChatFlagsIndex ] [ ChatRoster [ i ] . on_goto_second_target ] = 0;
			}
		
		if ( ChatRoster [ i ] . link_target ) 
			Me . Chat_Flags [ ChatFlagsIndex ] [ ChatRoster [ i ] . link_target ] = 0;
		}

        Me . chat_character_initialized [ ChatFlagsIndex ] = 1;
	}

    
    //--------------------
    // Now with the loaded chat data, we can do the real chat now...
    //
    DoChatFromChatRosterData( ChatFlagsIndex , ChatDroid , TRUE );
    
}; // void ChatWithFriendlyDroid( int Enum );



#undef _chat_c
