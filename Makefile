all: client server

server:
	$(CC) server.c -o server

client:
	$(CC) client.c -o client

.PHONY: clean
clean:
	-rm -f server client
