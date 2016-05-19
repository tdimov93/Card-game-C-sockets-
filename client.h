/*
 * =====================================================================================
 *
 *       Filename:  client.h
 *
 *    Description:  :x
 *
 *        Version:  1.0
 *        Created:  2016-05-18 15:48:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:	IPL
 *
 * =====================================================================================
 */
#ifndef CLIENTH_H
#define CLIENTH_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include "config.h"
#include "common_utils.h"
#include "cards.h"

#define PORT PORT_DIMOV
#endif

void print_cards();
int calculate_score();
void receive_message(int clientSocket,char** name);
void createNickname(char *name);
void connectToServer(int *clientSocket,char* serverIP,struct hostent *he,struct sockaddr_in *serverAddress);
bool fdp_is_valid(int fdp);