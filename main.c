#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <signal.h>

#define PORT 8080
#define MAX_REPLY 1024

int server_fd;

void handle_sigint(int sig)
{
    printf("SIGINT: shutting down socket...\n");
    /*if (shutdown(server_fd, SHUT_RDWR))
    {
        printf("Error shutting down socket!\n");
    }
    else
    {
        printf("Socket shut down; exiting.");
    }
    */
    close(server_fd);
    exit(0);
}

int send_reply(int socket_fd, const char *request)
{
    char reply[MAX_REPLY] = "Default Reply.";
    char unknown[] = "Unknown request.\n";

    char filename[1024];
    char length[256];
    char GET[4];
    int i;

    strcpy(GET, "GET");

    if (strncmp(GET, request, 3) == 0)
    {
        printf("received GET\n");
        for (i = 4; i < strlen(request); ++i)
        {
            if (isspace(request[i]))
            {
                filename[i-4] = '\0';
                break;
            }
            else
            {
                filename[i-4] = request[i];
            }

        }
        printf("Filename requested: %s", filename);

        strcpy(reply, "HTTP/1.1 200 OK\n");
        strcat(reply, "Content-Type: text/html\n");
        sprintf(length, "Content-Length: %ld\n\n", strlen(filename));//TODO smaller
        strcat(reply, length);
        strcat(reply, filename);
    }
    else
    {
        strcpy(reply, unknown);
    }

    printf("Sending:\n'%s'\n", reply);
    write(socket_fd, reply, strlen(reply));

}

int main(int argc, char **argv)
{
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int valread;
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    char buffer[1024] = {0}; //TODO

    signal(SIGINT, handle_sigint);

    /* Socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("cannot create socket");
        return 1;
    }

    /* Bind 
     * Set our struct to 0 and convert our parameters to internal
     * representations. */
    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY is 0.0.0.0
    address.sin_port = htons(PORT);

    while (1) {
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("bind failed");
            fprintf(stderr, "Re-attempting in 5 seconds...\n");
            sleep(5);
        }
        else {
            break;
        }
    }

    /* Listen, with a max backlog */
    if (listen(server_fd, 3) < 0) {
        perror("In listen");
    }

    /* LOCKING accept() */
    while (1)
    {
        printf("Waiting for new connection...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
        (socklen_t*)&addrlen)) <0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        valread = read(new_socket, buffer, 1024);

        if (valread < 0)
        {
            printf("Nothing to read");
        }
        else
        {
            //printf("\n\nReceived request:\n%s\n\n", buffer);
            send_reply(new_socket, buffer);
        }
        close(new_socket);
    }
}
