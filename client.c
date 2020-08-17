/*
 * Title:       C HTTP Client
 * Author:      Yuriy Yashkir <yuriy.yashkir@gmail.com>
 * Description: Tests the server by requesting a file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define ADDRESS "127.0.0.1"
#define PORT 8080


int main(int argc, char const *argv[])
{
    int sock;
    int valread;
    struct sockaddr_in server_addr;

    char *hello = "GET / HTTP/1.1";
    char buffer[1024] = {0}; //TODO

    printf("--------------------\n");
    printf("--- HTTP  CLIENT ---\n");
    printf("--------------------\n\n");

    printf("Creating socket ...\n");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("cannot create socket");
        return -1;
    }

    printf("Setting server address(%s) and port(%d) ...\n", ADDRESS, PORT);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr) <= 0)
    {
        printf("Invalid or unsupported Address.\n");
        return -1;
    }

    printf("Connecting to %s:%d ...\n", ADDRESS, PORT);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }

    printf("Sending request...");

    if (send(sock, hello, strlen(hello), 0) < 0)
    {
        fprintf(stderr, "Failed to send.");
        return -1;
    }

    printf("Reading reply ...\n");

    if ((valread = read(sock, buffer, 1024)) < 1)
    {
        fprintf(stderr, "Unable to read.");
        return -1;
    }

    printf("Read %d:\n----------\n%s\n----------\n", valread, buffer);

    return 0;
}
