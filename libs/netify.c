#include "netify.h"

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

int netify_socket_bind(int port) {
    struct sockaddr_in server_sockaddr_in;

    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr_in.sin_port = htons(port);

    int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    int result =
        bind(socket_file_descriptor, (struct sockaddr *)&server_sockaddr_in,
             sizeof(server_sockaddr_in));

    if (result == -1) {
        printf("NETIFY::ERROR::Socket bind failed.\n");

        return -1;
    } else {
        printf("NETIFY::INFO::Socket binded successfully.\n");
    }

    return socket_file_descriptor;
}

int netify_socket_close(int socketfd) {
    close(socketfd);

    return 0;
}

char *netify_listen_socket(int socketfd, char *buffer, int buffer_len) {
    listen(socketfd, NETIFY_MAX_QUEUED_REQUEST_COUNT);

    int connection_file_descriptor = accept(socketfd, NULL, 0);

    int result = read(connection_file_descriptor, buffer, buffer_len);
    if (result == -1) {
        printf("NETIFY::ERROR::Socket read failed.\n");
    } else if (result == 0) {
        printf("NETIFY::INFO::Socket read successfully.\n");
    }

    char status[] = { 'H','e','l','l','o',' ','W','o','r','l','d','!','\n' };
    write(connection_file_descriptor, &status, sizeof(status));

    close(connection_file_descriptor);

    return buffer;
}
