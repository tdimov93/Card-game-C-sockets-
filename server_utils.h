/*
 * =====================================================================================
 *
 *       Filename:  server_utils.h
 *
 *    Description:	Utilitary functions used by the server program
 *
 *        Version:  1.0
 *        Created:  2016-05-17 15:24:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:
 *
 * =====================================================================================
 */
#include "config.h"
#include "common_utils.h"

typedef struct player {
	int socket;
	char nickname[NAMESIZE];
	int played_card;
	bool isempty;
} player;

void broadcast( int msg_code, char* payload, player* recipients, int rcp_count ); //sends a message (code and body) to all players
void broadcast_light( int msg_code, player* recipients, int rcp_count ); //send a message code to all player
void extract_player_nickname( char** msg, char* nickname);
int rand_range( int upper_limit );
bool array_contains( int* haystack, int needle, int length );
