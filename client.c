#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int sock;
    int valread;
    struct sockaddr_in server_addr;

    char *hello = "<strong>Hello</hello> from client.";
    char buffer[1024] = {0}; //TODO

    /* Socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("cannot create socket");
        return -1;
    }

    /* Connect 
     * Set our struct to 0 and convert our parameters to internal
     * representations. */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        printf("Invalid or unsupported Address.\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection Failed.\n");
        return -1;
    }

    /* Send and Receive */
    send(sock, hello, strlen(hello), 0);
    printf("Hello sent to server...");
    valread = read(sock, buffer, 1024);
    printf("Got:\n----------\n%s----------\n", buffer);
    return 0;
}
