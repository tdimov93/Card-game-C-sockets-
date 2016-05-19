/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  Fichier gérant le client du projet
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
#include "client.h"

int client_socket;
int hand[DECK_SIZE];
int stash[DECK_SIZE];
int cards_in_hand;
int cards_in_stash;

void disconnect(bool should_inform) {
	printf("Okay, okay..\n");
	if (should_inform) {
		send_light_msg(DISCONNECT, client_socket);
		close(client_socket);
	}
	exit(EXIT_SUCCESS);
}
void interrupt_handler(int signum) {
	if (signum == SIGINT) {
		//Ctr+C
		disconnect(TRUE);
	}
}

void refill() {
	memcpy(hand, stash, cards_in_stash * sizeof(int));
	memset(stash, -1, cards_in_stash * sizeof(int));
	cards_in_hand = cards_in_stash;
	cards_in_stash = 0;
}

void clear_cards() {
	memset(hand, -1, DECK_SIZE * sizeof(int));
	memset(stash, -1, DECK_SIZE * sizeof(int));
	cards_in_hand = 0;
	cards_in_stash = 0;
}

void print_cards() {
	int i;
	printf("current hand : ");
	for (i = 0; i < cards_in_hand; i++) {
		printf("%d: %-8s ", i, get_card_name(hand[i]));
		if(i+1 < cards_in_hand) {
			printf("|");
		}
	}
	printf("\n");
	printf("current stash : ");
	for (i = 0; i < cards_in_stash; i++) {
		printf("%d: %-8s ", i, get_card_name(stash[i]));
		if(i+1 < cards_in_stash ){
			printf("|");
		}
	}
	printf("\n");
}

int calculate_score() {
	printf("CALCULATING SCORE!\n");
	print_cards();
	int score = 0;
	int i;
	for (i = 0; i < cards_in_hand; i++) {
		score += get_card_points(hand[i]);
	}
	for (i = 0; i < cards_in_stash; i++) {
		score += get_card_points(stash[i]);
	}
	return score;
}

void receive_message(int client_socket,char** name) {
	char msgFromServer[MESSAGE_SIZE];
	char* msg = msgFromServer;

	int bytesReceived;
	if ((bytesReceived=recv(client_socket,msgFromServer,MESSAGE_SIZE,0))==-1) {
		perror("Receive");
		exit(EXIT_FAILURE);
	}
	msgFromServer[bytesReceived]='\0';
	fflush(stdout);
	int msg_code = extract_msg_code(&msg);
	if(msg_code == WAIT) {
		send_msg(NICKNAME, *name, client_socket);
	} else if(msg_code == REFUSE) {
		printf("Connection refused by the server. Please try again later.\n");
		close(client_socket);
		exit(-1);
	} else if(msg_code == DISCONNECT) {
		printf("Disconnected by the server.\n");
		printf("Game over!\n\n");
		close(client_socket);
		exit(-1);
	} else if(msg_code == ROUND) {
		printf("The current round is over.\n");
		int score = calculate_score();
		send_int_msg(SCORE, score, client_socket);
		printf("Your score : %d .. \n", score);
	} else if(msg_code == DEAL) {
		clear_cards();
		printf("Round begins. ");
		cards_in_hand = decode_msg_payload(&msg, hand, DECK_SIZE / 2);
		printf("These are your cards : \n");
	} else if(msg_code == ASK) {
		print_cards();
		int choice = -1;
		if (cards_in_hand + cards_in_stash == 1) {
			printf("You are playing your last card\n");
			//the player is about to play his last card, the round is over
			send_light_msg(EMPTY, client_socket);
		}
		if (cards_in_hand == 0) {
			//the player hasn't got any cards in his hand, his stash becomes his hand
			refill();
			printf("Out of cards, hand replenished from stash\n");
			print_cards();
		}
		int times = 0;
		do {
			if(times > 0) {
				printf("You are kindly requested to play one of your own cards\n");
				print_cards();
			}
			printf("Which card would you like to play ?\n");
			if (scanf("%d", &choice) == EOF) {
				//Ctr+D
				disconnect(FALSE);
			}
			times++;
		} while(choice < 0 || choice > cards_in_hand);
		send_int_msg(PLAY, hand[choice], client_socket);
		int i;
		for (i = choice; i < cards_in_hand-1; i++) {
			hand[i] = hand[i+1];
		}
		cards_in_hand--;
		printf("Your new hand is: ");
		print_cards();
		printf("\n");
	} else if (msg_code == GIVE){
		int* stash_ptr = stash + cards_in_stash;
		int size = decode_msg_payload(&msg, stash_ptr, MAX_PLAYERS);
		cards_in_stash += size;
		print_cards();
		printf("You win the turn!\n");
	} else if (msg_code == WINNER) {
		printf("Congratulations, you win the game!\n");
	} else {
		printf("Message not support yet !");
	}
}

void create_nickname(char* name) {
	char* request="Enter your nickname (20 characters max): ";
	printf("%s", request);
	scanf("%s", name);
	fflush(stdin);
	fflush(stdout);
}

void connectToServer(int *client_socket,char* server_ip,struct hostent *host,struct sockaddr_in *server_address){
	if ((host=gethostbyname(server_ip)) == NULL) {
		perror("Client: gethostbyname failed");
		exit(EXIT_FAILURE);
	}

	if ((*client_socket = socket(AF_INET,SOCK_STREAM, 0)) == -1) {
		perror("Client: socket");
		exit(EXIT_FAILURE);
	}

	server_address->sin_family = AF_INET;
	server_address->sin_port = htons(PORT_DIMOV);
	server_address->sin_addr = *((struct in_addr*)host->h_addr);
	memset(&(server_address->sin_zero), '\0', 8);

	if (connect(*client_socket, (struct sockaddr *)server_address,sizeof(struct sockaddr)) == -1) {
		perror("Client: connect");
		exit(EXIT_FAILURE);
	}
}

bool fdp_is_valid(int fdp) {
	return fcntl(fdp, F_GETFD) != -1 || errno != EBADF;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "You need to specify the server's ip address\n");
		return EXIT_FAILURE;
	}
	struct sockaddr_in server_address;// adresse du server
	struct hostent host;
	clear_cards();
	char name[NAMESIZE];
	char* name_ptr = name;

	struct sigaction interrupt;
	memset(&interrupt, 0, sizeof(interrupt));
	interrupt.sa_handler = &interrupt_handler;
	sigaction(SIGINT, &interrupt, NULL);

	create_nickname(name);
	connectToServer(&client_socket, argv[1], &host, &server_address);
	while (1) {
		if(!fdp_is_valid(client_socket)) {
			printf("Connection error.\n");
			break;
		}
		receive_message(client_socket, &name_ptr);
	}
	close(client_socket);
	return EXIT_SUCCESS;
}
