#include "netify.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int netify_connection_read(int connectionfd, char *req_buffer,
                           int req_buffer_len) {
    int result = read(connectionfd, req_buffer, req_buffer_len);
    if (result == -1) {
        printf("NETIFY::ERROR::Connection read failed.\n");
    } else if (result == 0) {
        printf("NETIFY::INFO::Connection read successfully.\n");
    }

    return result;
}

int netify_connection_write(int connectionfd, char *res_buffer,
                            int res_buffer_len) {
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

// TODO:
// - Add validation for len parameters;
// - Generate status_code depending on passed status code;
// - Add correct amount of new lines (after status_code line, after headers)
// - Create Status Code constants and mapping to names.
int netify_response_send(int connectionfd, int status_code,
                         char *headers_buffer, int headers_buffer_len,
                         char *message_buffer, int message_buffer_len) {
    char status_code_buffer[] = "HTTP/1.1 200 OK";
    int status_code_buffer_len = strlen(status_code_buffer);

    int message_padding_len = 2;

    int res_buffer_len = status_code_buffer_len + headers_buffer_len +
                         message_padding_len + message_buffer_len;
    char *res_buffer = (char *)malloc(res_buffer_len);
    int i, j = 0;

    for (i = 0; i < status_code_buffer_len; i++, j++) {
        res_buffer[j] = status_code_buffer[i];
    }

    for (i = 0; i < headers_buffer_len; i++, j++) {
        res_buffer[j] = headers_buffer[i];
    }

    for (i = 0; i < message_padding_len; i++, j++) {
        res_buffer[j] = '\n';
    }

    for (i = 0; i < message_buffer_len; i++, j++) {
        res_buffer[j] = message_buffer[i];
    }

    printf("NETIFY::INFO::Sending response with %d bytes.\n", res_buffer_len);

    return netify_connection_write(connectionfd, res_buffer, res_buffer_len);
}
