#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

int main(int argc, char **argv)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int valread;
    char *hello = "<strong>Hello</hello> from the best server.";

    char buffer[1024] = {0}; //TODO

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

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        return 1;
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

        printf("\n\nReceived request:\n%s\n\n", buffer);
        write(new_socket, hello, strlen(hello));
        printf("Hello sent...\n");
        close(new_socket);
    }
}
