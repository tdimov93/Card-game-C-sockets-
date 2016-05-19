TARGET= server client

CC=gcc

normal: $(TARGET)

server: server.o common_utils.o server_utils.o cards.o
	$(CC) server.o common_utils.o server_utils.o cards.o -o server

client: client.o common_utils.o cards.o
	$(CC) client.o common_utils.o cards.o -o client

server.o: server.c common_utils.h server_utils.h cards.h
	$(CC) -c server.c

client.o: client.c common_utils.h cards.h
	$(CC) -c client.c

common_utils.o:	common_utils.h common_utils.c
	$(CC) -c common_utils.c

server_utils.o: server_utils.h server_utils.c
	$(CC) -c server_utils.c

cards.o: cards.h cards.c
	$(CC) -c cards.c

clean:
	$(RM) $(TARGET)
