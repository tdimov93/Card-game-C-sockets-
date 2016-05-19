/*
 * =====================================================================================
 *
 *       Filename:  example.c
 *
 *    Description:  Fichier contenant les fonctions du projet
 *
 *        Version:  1.0
 *        Created:  05/04/2016 03:54:34 PM
 *       Revision:  1
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:  IPL-Student
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "cards.h"
#include "common_utils.h"

int hand[DECK_SIZE];
int stash[DECK_SIZE];
int cards_in_hand;
int cards_in_stash;

void receive_msg(int i, int socket, char** nickname) {
	char msg[MESSAGE_SIZE];
	char* msg_ptr = msg; // !!
	int bytes_received;
	if ((bytes_received=recv(socket,msg_ptr,MESSAGE_SIZE,0))==-1){
			perror("Client receive from server : ");
			exit(EXIT_FAILURE);
	}
	msg[bytes_received]='\0';
	int msg_code = extract_msg_code(&msg_ptr); //renvoie le code et déplace le pointeur
	if (msg_code == WAIT) {
		printf("received a connection confirmation WAIT\n");
		printf("my nickname : %s\n", *nickname);
		send_msg(NICKNAME, *nickname, socket);
	} else if (msg_code == REFUSE || msg_code == DISCONNECT) {
		printf("received a connection refusal REFUSE\n");
		close(socket);
	} else if (msg_code == ROUND) {
		printf("new round\n");
	} else if (msg_code == DEAL) {
		cards_in_hand = decode_msg_payload(&msg_ptr, hand, DECK_SIZE); //remplit la liste de cartes et renvoie le nombre de cartes reçues
	} // ...
}