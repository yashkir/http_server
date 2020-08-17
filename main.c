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

/* Load a file into a buffer, returns -1 on error */
int load_file(const char *filename, char *buffer, int n)
{
    FILE* fp;
    int i;

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        perror("Load file error");
        return -1;
    }

    for(i = 0; i < n; i++)
    {
        buffer[i] = fgetc(fp);
        if (feof(fp))
        {
            buffer[i] = '\0';
            break;
        }
    }

    fclose(fp);
}

/* Clean up sockets on SIGINT */
void handle_sigint(int sig)
{
    printf("\n\033[31;1mSIGINT: shutting down socket...\033[0m\n");
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

/* Sends a 200 or 404 appropriate to the request. */
int send_reply(int socket_fd, const char *request)
{
    char reply[MAX_REPLY] = "Default Reply.";
    char unknown[] = "Unknown request.\n";

    char buffer[1024];
    char path[1024]; //TODO dynamic
    char length[256];
    char GET[4];
    int i;

    strcpy(GET, "GET");

    /* Check the type of request */
    if (strncmp(GET, request, 3) == 0)
    {
        printf("received GET\n");

        /* Get the filename */
        for (i = 4; i < strlen(request); ++i)
        {
            if (isspace(request[i]))
            {
              path[i - 4] = '\0';
              break;
            }
            else
            {
              path[i - 4] = request[i];
            }

        }

        printf("Path requested: %s\n", path);

        /* Route root to index.html */
        if (strcmp(path, "/") == 0) {
          strcpy(path, "index.html");
        }

        /* Try to load the requested file and reply */
        if (load_file(path, buffer, 1024) == 0)
        {
            strcpy(reply, "HTTP/1.1 200 OK\n");
            strcat(reply, "Content-Type: text/html\n");
            sprintf(length, "Content-Length: %ld\n\n", strlen(buffer)); // TODO smaller + buffer
            strcat(reply, length);
            strcat(reply, buffer); 
        }
        else
        {
            strcpy(reply, "HTTP/1.1 404 Not Found\n");
        }


    }
    else
    {
        strcpy(reply, unknown);
    }

    printf("Sending %ld:\n'%s'\n", strlen(reply), reply);
    write(socket_fd, reply, strlen(reply));

}

/* Initializes sockets, binds, listens, and runs an accept() loop */
int main(int argc, char **argv)
{
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int valread;
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    char buffer[1024] = {0}; //TODO

    signal(SIGINT, handle_sigint);

    printf("\n\033[34;1m--> STARTING SERVER . . .\033[0m\n");

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
            /* Let send_reply take over */
            send_reply(new_socket, buffer);
        }
        close(new_socket);
    }
}
