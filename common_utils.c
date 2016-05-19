/*
 * =====================================================================================
 *
 *       Filename:  common_utils.c
 *
 *    Description: util functions used by the server and the client
 *
 *        Version:  1.0
 *        Created:  2016-05-02 09:20:00
 *       Revision:  none
 *       Compiler:  cc
 *
 *         Author: DIMOV Theodor, DRAGOMIR Philippe
 *
 * =====================================================================================
 */
#include "common_utils.h"


void send_prepared_msg(char* pmsg, int socket) {
	if (send(socket, pmsg, MESSAGE_SIZE, 0) == -1) {
		perror("Failed to send a mesesage to the serveur");
		exit(EXIT_FAILURE);
	}
}

void send_msg(int msg_code, const char* payload, int socket) {
	char msg[MESSAGE_SIZE];
	sprintf(msg, "%d %s", msg_code, payload);
	send_prepared_msg(msg, socket);
}

void send_light_msg(int msg_code, int socket) {
	char msg[MESSAGE_SIZE];
	sprintf(msg, "%d", msg_code);
	send_prepared_msg(msg, socket);
}

void send_int_msg(int msg_code, int payload, int socket) {
	char msg[MESSAGE_SIZE];
	sprintf(msg, "%d %d", msg_code, payload);
	send_prepared_msg(msg, socket);
}

int extract_msg_code(char** msg) {
	return atoi(strtok_r(*msg, " ", msg));
}

int decode_msg_payload(char** raw_payload, int* decoded_payload, int max_elements) {
	int i;
	for (i = 0; i < max_elements; i++) {
		char* token = strtok_r(*raw_payload, " ", raw_payload );
		if (token == NULL) {
			return i;
		}
		*(decoded_payload + i) = atoi(token);
	}
	return i;
}

