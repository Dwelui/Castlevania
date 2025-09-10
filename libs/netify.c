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
    printf("NETIFY::INFO::Successfuly closed socket.\n");

    return 0;
}

int netify_connection_accept(int socketfd) {
    int result = listen(socketfd, NETIFY_MAX_QUEUED_REQUEST_COUNT);
    if (result == -1) {
        printf("NETIFY::ERROR::Failed to establish connection.\n");
    } else if (result == 0) {
        printf("NETIFY::INFO::Connection established successfully.\n");
    }

    return accept(socketfd, NULL, 0);
}

int netify_connection_read(int connectionfd, char *req_buffer, int req_buffer_len) {
    int result = read(connectionfd, req_buffer, req_buffer_len);
    if (result == -1) {
        printf("NETIFY::ERROR::Socket read failed.\n");
    } else if (result == 0) {
        printf("NETIFY::INFO::Socket read successfully.\n");
    }

    return result;
}

int netify_connection_write(int connectionfd, char *res_buffer, int res_buffer_len) {
    int result = write(connectionfd, res_buffer, res_buffer_len);
    if (result == -1) {
        printf("NETIFY::ERROR::Failed to write to connection.\n");
    } else if (result > 0) {
        printf("NETIFY::INFO::Successfuly written to connection.\n");
    }

    return result;
}

int netify_connection_close(int connectionfd) {
    close(connectionfd);
    printf("NETIFY::INFO::Successfuly closed connection.\n");

    return 0;
}
