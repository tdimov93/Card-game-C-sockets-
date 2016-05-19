/*
 * =====================================================================================
 *
 *       Filename:  common_utils.h
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
#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "config.h"

#define MESSAGE_SIZE 82 //the maximum size a message could ever have

/*
 * sends a pre-formatted message to a given socket.
 *
 * msg: a pre-formatted message
 * socket: the receiving socket
 */
void send_prepared_msg( char* msg, int socket);

/*
 * formats and sends a full message (message code and body) to the given socket.
 *
 * msg_code: the message code
 * payload: additional payload or body
 * socket: the receiving socket
 */
void send_msg( int msg_code, const char* payload, int socket);

/*
 * formats and sends a message without a body to the given socket.
 *
 * msg_code: the message code
 * socket: the receiving socket
 */
void send_light_msg( int msg_code, int socket);

// int* ?
void send_int_msg( int msg_code, int payload, int socket);

/* *
* extracts the message code header from the rest of the message.
*
* msg : the received message to decode; the pointer is advanced to right after the message code
*
* */
int extract_msg_code( char** msg );

/* *
* decodes the message payload
*
* raw_payload: the payload to decode; the pointer is advanced to right after the last decoded element
* decoded_payload: area to store the decoded data. It should be alloca    ted and have enough space to contain all of the information necessary.
* max_elements: the maximum number of elements in the payload
*
* return: the actual number of elements found
*
* */
int decode_msg_payload( char** raw_payload, int* decoded_payload, int max_elements);

#endif