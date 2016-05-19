/*
 * ===================================================================
 *
 *       Filename:  server_utils.c
 *
 *    Description:	Contains the implementations of the utilitary functions defined in server_utils.h
 *
 *        Version:  1.0
 *        Created:  2016-05-17 15:26:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Phlippe
 *   Organization:  IPL
 *
 * ===================================================================
 */
#include "server_utils.h"

void broadcast(int msg_code, char* payload, player* recipients, int rcp_count) {
	char msg[MESSAGE_SIZE];
	sprintf(msg, "%d %s", msg_code, payload);
	int i;
	for (i = 0; i < rcp_count; i++) {
		if (recipients[i].socket != 0) {
			send_prepared_msg(msg, recipients[i].socket);
		}
	}
}

void broadcast_light(int msg_code, player* recipients, int rcp_count) {
	char msg[MESSAGE_SIZE];
	sprintf(msg, "%d", msg_code);
	int i;
	for (i = 0; i < rcp_count; i++) {
		if (recipients[i].socket != 0) {
			send_prepared_msg(msg, recipients[i].socket);
		}
	}
}

void extract_player_nickname(char** msg, char* nickname) {
	sprintf(nickname,"%s", strtok_r(*msg, " ", msg));
}

int rand_range(int upper_limit) {
	return (int) (( (double) upper_limit / RAND_MAX) * rand());
}

bool array_contains(int* haystack, int needle, int length) {
	int* array_ptr = haystack;
	for (; (array_ptr - haystack) < length; array_ptr++) {
		if (*array_ptr == needle) {
			return TRUE;
		}
	}
	return FALSE;
}
