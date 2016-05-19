/*
 * =====================================================================================
 *
 *       Filename:  server.h
 *
 *    Description:	
 *
 *        Version:  1.0
 *        Created:  2016-05-15 12:13:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:  IPL
 *
 * =====================================================================================
 */
#ifndef SERVERH_H
#define SERVERH_H
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "common_utils.h"
#include "server_utils.h"
#include "cards.h"

#define MIN_PLAYERS 2
#define PORT PORT_DIMOV
#define BUFFER_SIZE 1024
#define BACKLOG 5
#define COUNTDOWN 10 //30 seconds wait time
#endif
typedef void (*fct_ptr)( );

void init_server(int*, struct sockaddr_in*); //creates and binds socket
void alarm_handler(int); //handles alarm timeouts
void interrupt_handler(int); //shuts down the server when a SIGINT occurs
void shutdown_socket(int); //closes a given socket
void shutdown_server(); //halts the server
void add_client(int, struct sockaddr_in*); //adds a client to the fdset
void add_player(int); //confirm connection and inform client
void remove_player(player*, int, bool); //removes a player from the game
void refuse_connection(int); //refuses a given client's connection request
bool receive_msg(char*, int); //handles incoming messages
void clear_lobby(); //informs awaiting players that the game is over
void add_nickname(int, char**); //saves a given player's nickname
void start_game(); //starts the game
void deal_cards(); //shuffles the deck and deals cards to all players
void start_round(); //starts a new round
void receive_card(int, char**); //receives a given player's played card
void end_round(int, char**); //ends a round
void update_score(int, char**); //updates a given player's score in shm
void create_nicknames_shared_memory(char* nickname);//create a sharedMemory with the nickname setted in it
void end_game();
int find_index(player*, int);
