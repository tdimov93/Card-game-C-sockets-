#include "server.h"

//j'ai commencé à épurer le code, mais malheureusement je n'ai pas eu assez de temps pour réduire le nombre de variables globales utilisé
int cl_count;
int pl_scores[MAX_PLAYERS]; //manque de temps pour shm
player players[MAX_PLAYERS];
bool game_in_progress;
bool time_is_up; //
bool running; //server is running
bool end_of_turn; //the current turn is over
bool sigempty; //a player has signaled that he's out of cards
bool end_of_round; //the current round is over

fct_ptr dispatcher[] = { add_player, refuse_connection, add_nickname, 0, 0, 0, receive_card, 0, end_round, 0, update_score }; //le but de ceci était d'alléger la fonction main en évitant une grande section de if-else ou bien un switch-case.

int main(int argc, char** argv) {
	int server_socket, max_fd, select_res, i, max_rounds, round_nb = 0;
	struct sockaddr_in my_addr, cl_addr;
	struct timeval timeout = {0, 15000}; //15 milliseconds
	fd_set fds;

	if (argc != 2) {
		fprintf(stderr, "Gimme args! [maximum number of rounds]\n");
		return EXIT_FAILURE;
	}
	max_rounds = atoi(argv[1]);

	srand(time(NULL));

	for (i = 0; i < MAX_PLAYERS; i++) {
		players[i].socket = 0;
	}

	cl_count = 0;
	game_in_progress = FALSE;
	time_is_up = FALSE;
	running = TRUE;

	struct sigaction alarm, interrupt;
	memset(&alarm, 0, sizeof(alarm));
	memset(&interrupt, 0, sizeof(interrupt));
	alarm.sa_handler = &alarm_handler;
	interrupt.sa_handler = &interrupt_handler;
	sigaction(SIGALRM, &alarm, NULL);
	sigaction(SIGINT, &interrupt, NULL);

	init_server(&server_socket, &my_addr);
	char message[MESSAGE_SIZE];
	char* msg;
	while (running) {
		FD_ZERO(&fds);
		FD_SET(server_socket, &fds);
		max_fd = server_socket + 1;
		int i;
		for (i = 0; i < MAX_PLAYERS; i++) {
			if (players[i].socket > 0) {
				FD_SET(players[i].socket, &fds);
			}
			if (players[i].socket >= max_fd) {
				max_fd = players[i].socket+1;
			}
		}
		if ((select_res = select(max_fd, &fds, NULL, NULL, &timeout)) < 0) {
			//errno is set to EINTR when select() is interrupted by a signal, in our case, the alarm
			if (errno != EINTR) {
				shutdown_server();
				return EXIT_FAILURE;
			}
		}

		//select_res = 0 when there's a timeout
		if (select_res > 0) {
			if (FD_ISSET(server_socket, &fds)) {
				add_client(server_socket, &cl_addr);
			}
			for (i = 0; i < cl_count; i++) {
				if (FD_ISSET(players[i].socket, &fds)) {
					msg = message;
					if (receive_msg(msg, players[i].socket)) {
						int msg_code = extract_msg_code(&msg);
						dispatcher[msg_code] (players[i].socket, &msg);
					} else {
						remove_player(players, i, FALSE);
					}
				}
			}
		}
		if (game_in_progress) {
			if (end_of_turn && !sigempty) {
				//the current turn is over and none of the players have signaled that they're out of cards
				end_of_turn = FALSE;
				//prompt players to play a card
				broadcast_light(ASK, players, cl_count);
			}
			if (end_of_round) {
				//current round is over
				round_nb++;
				if (round_nb == max_rounds) {
					end_game();
					round_nb = 0;
					printf("game over\n");
				} else {
					start_round();
				}
			}
		}
	}
	shutdown_socket(server_socket);
	return EXIT_SUCCESS;
}

void alarm_handler(int signum) {
	if (signum == SIGALRM) {
		if (cl_count < 2) {
			clear_lobby();
		} else {
			start_game();
		}
		time_is_up = TRUE;
	}
}

int find_index(player* players, int socket) {
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].socket == socket) {
			return i;
		}
	}
	return -1;
}


void interrupt_handler(int signum) {
	if (signum == SIGINT) {
		shutdown_server();
	}
}

void init_server(int *server_socket,struct sockaddr_in *my_addr) {

	if ((*server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(EXIT_FAILURE);
	}

	memset(my_addr, 1, sizeof(struct sockaddr_in));
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(PORT);
	my_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	//memset(&(my_addr->sin_zero), '\0', 8);

	if (bind(*server_socket, (struct sockaddr *)my_addr, sizeof(*my_addr)) == -1) {
		perror("Bind");
		exit(EXIT_FAILURE);
	}

	if (listen(*server_socket, BACKLOG)) {
		perror("Listen");
		exit(EXIT_FAILURE);
	}
}

void add_client(int server_socket, struct sockaddr_in *cl_addr) {
	int new_cl_socket;
	int cl_addr_length = sizeof(struct sockaddr_in);
	if ((new_cl_socket = accept(server_socket, (struct sockaddr *)cl_addr, (socklen_t*) &cl_addr_length)) < 0) {
		perror("Connection error");
		exit(EXIT_FAILURE);
	} else {
		//either accept or refuse the connection
		//players, cl_count
		if (game_in_progress || cl_count == MAX_PLAYERS) {
			refuse_connection(new_cl_socket);
		} else {
			add_player(new_cl_socket);
		}
	}
}

void add_player(int socket) {
	players[cl_count].played_card = -1;
	players[cl_count].isempty = FALSE;
	players[cl_count++].socket = socket;
	send_int_msg(WAIT, COUNTDOWN, socket);
	if (cl_count == 1) {
		//first client, set an alarm for 30 seconds
		alarm(COUNTDOWN);
	}
}


void end_game() {
	printf("player count : %d\n", cl_count);
	int i, windex, highscore = 0;
	for (i = 0; i < cl_count; i++) {
		if (pl_scores[i] >= highscore) {
			highscore = pl_scores[i];
			windex = i;
		}
	}
	printf("winner : %s with %d points\n", players[windex].nickname, pl_scores[windex]);
	send_light_msg(WINNER, players[windex].socket);
	clear_lobby();
}

void remove_player(player* players, int index, bool sockopen) {
	if (sockopen) {
		shutdown_socket(players[index].socket);
	}
	memcpy(players[index].nickname, "\0", NAMESIZE);
	players[index].socket = 0;
	pl_scores[index] = 0;
	cl_count--;
	int j;
	for (j = index+1; j <= cl_count; j++) { //on vient de supprimer un joueur, cl_count a été décrémenté --> '<='
		players[j-1].socket = players[j].socket;
		sprintf(players[j-1].nickname, "%s", players[j].nickname);
	}
	if (game_in_progress && cl_count == 1) {
		end_game();
	}
}

void refuse_connection(int socket) {
	send_light_msg(REFUSE, socket);
}

void add_nickname(int socket, char** msg) {
	char nickname[NAMESIZE];
	extract_player_nickname(msg, nickname);
	int index = find_index(players, socket);
	sprintf(players[index].nickname, "%s", nickname);
}

void deal_cards() {
	int cards_per_player = DECK_SIZE / cl_count;
	int dealt_cards[cards_per_player * cl_count];
	int total_dealt_cards = 0;
	int player;
	for (player = 0; player < cl_count; player++) {
		int card;
		int str_length = 0;
		char msg[3* cards_per_player]; //2 caractèress par carte + 1 espace
		msg[0] = '\0';
		for (card = 0; card < cards_per_player; card++) {
			int random_card;
			do {
				random_card = rand_range(DECK_SIZE);
				//choisir une carte tant qu'on n'en trouve pas une qui n'a pas encore été choisie
			} while (array_contains(dealt_cards, random_card, total_dealt_cards));
			//rajouter la carte au message
			str_length += sprintf(msg+str_length, "%d ", random_card);
			dealt_cards[total_dealt_cards++] = random_card;
		}
		//distribuer les cartes choisies au joueur
		send_msg(DEAL, msg, players[player].socket);
		printf("cards dealt : \n");
		printf("%s\n", msg);
	}
}

void clear_lobby() {
	broadcast_light(DISCONNECT, players, cl_count);
	game_in_progress = FALSE;
	while (players[0].socket > 0) {
			remove_player(players, 0, FALSE);
	}
}

bool receive_msg(char* msg, int fd) {
	int bytes_received;
	if ((bytes_received = recv(fd, msg, MESSAGE_SIZE, 0)) <= 0) {
		if (bytes_received == 0) {
			printf("Client disconnected.\n");
		}
		else {
			perror("Could not receive message");
		}
		return FALSE;
	}
	return TRUE;
}

void start_game() {
	start_round();
	game_in_progress = TRUE;
}

void start_round() {
	deal_cards();
	end_of_turn = TRUE; //voir fin du main
	end_of_round = FALSE;
	sigempty = FALSE;
}

void shutdown_socket(int socket) {
	printf("Shutting down socket number %d\n", socket);
	if (close(socket) < 0) {
		perror("Socket shutdown");
		exit(EXIT_FAILURE);
	}
}

void shutdown_server() {
	printf("server shutting down ..\n");
	clear_lobby();
	running = FALSE;
	//free shared memory (allocate some first)
}

void receive_card(int socket, char** msg) {
	static int received_cards_count = 0;
	static int str_length = 0;
	static char cards[MAX_PLAYERS * 3];//:
	int player_index = find_index(players, socket);
	int card;
	decode_msg_payload(msg, &card, 1);
	players[player_index].played_card = card;
	received_cards_count++;
	str_length += sprintf(cards+str_length, "%d ", card);
	if (received_cards_count == cl_count) {
		int highest_card_holder, highest_card = 0, i, empty_count = 0, empty_index;
		for (i = 0; i < cl_count; i++) {
			if (players[i].played_card >= highest_card) {
				highest_card = players[i].played_card;
				highest_card_holder = i;
			}
			if (players[i].isempty) {
				empty_count++;
				empty_index = i;
			}
			players[i].played_card = -1;
			players[i].isempty = FALSE;
		}
		send_msg(GIVE, cards, players[highest_card_holder].socket);
		memset(cards, 0, cl_count * sizeof(int));
		received_cards_count = 0;
		highest_card = -1;
		str_length = 0;
		end_of_turn = TRUE;
		if (empty_count == 1 && empty_index == highest_card_holder) { //the only player who's out of cards wins the turn, round is still on
			printf("saved by the bell!\n");
			sigempty = FALSE;
		}
		if (sigempty) {
			broadcast_light(ROUND, players, cl_count);
		}
	}
}

void end_round(int socket, char** msg) {
	if (!sigempty) {
		printf("end of round !\n");
		sigempty = TRUE;
	}
	players[find_index(players, socket)].isempty = TRUE;
}

void update_score(int socket, char** msg) {
	static int count = 0;
	int score;
	decode_msg_payload(msg, &score, 1);
	int i;
	for (i = 0; i < cl_count; i++) {
		if (players[i].socket == socket) {
			pl_scores[i] += score;
		}
	}
	count++;
	if (count == cl_count) {
		count = 0;
		end_of_round = TRUE;
		int i;
		for (i = 0; i < cl_count; i++) {
			if (players[i].socket > 0) {
				printf("%s : %d points\n", players[i].nickname, pl_scores[i]);
			}
		}
		char buffer[10];
		printf("press any key to play next round\n");
		fgets(buffer, 10, stdin);
	}
}

