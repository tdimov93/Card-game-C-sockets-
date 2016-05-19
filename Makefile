TARGET= server client

CC=gcc
CFLAGS=-Wall -Wextra

normal: $(TARGET)

server: server.o common_utils.o server_utils.o cards.o
	$(CC) $(CFLAGS) server.o common_utils.o server_utils.o cards.o -o server

client: client.o common_utils.o cards.o
	$(CC) $(CFLAGS) client.o common_utils.o cards.o -o client

server.o: server.c common_utils.h server_utils.h cards.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c common_utils.h cards.h
	$(CC) $(CFLAGS) -c client.c

common_utils.o:	common_utils.h common_utils.c
	$(CC) $(CFLAGS) -c common_utils.c

server_utils.o: server_utils.h server_utils.c
	$(CC) $(CFLAGS) -c server_utils.c

cards.o: cards.h cards.c
	$(CC) $(CFLAGS) -c cards.c

clean:
	$(RM) $(TARGET)
