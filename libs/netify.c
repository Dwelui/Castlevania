#include "netify.h"
#include "logify.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const char *http_status_to_string(int code) {
    switch (code) {
    case 200:
        return "OK";
    case 201:
        return "Created";
    case 204:
        return "No Content";
    case 400:
        return "Bad Request";
    case 401:
        return "Unauthorized";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 500:
        return "Internal Server Error";
    case 501:
        return "Not Implemented";
    default:
        return "Unknown Status";
    }
}

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
        logify_log(ERROR, "NETIFY::Socket bind failed.");
        return -1;
    } else {
        logify_log(INFO, "NETIFY::Socket binded successfully on port: %d.", port);
    }

    return socket_file_descriptor;
}

int netify_socket_close(int socketfd) {
    close(socketfd);

    logify_log(INFO, "NETIFY::Successfuly closed socket.");

    return 0;
}

int netify_connection_accept(int socketfd,
                             struct sockaddr_in *client_sockaddr_in,
                             socklen_t *client_sockaddr_in_len) {
    int result = listen(socketfd, NETIFY_MAX_QUEUED_REQUEST_COUNT);
    if (result == -1) {
        logify_log(ERROR, "NETIFY::Failed to establish connection.");
        return -1;
    } else if (result == 0) {
        logify_log(INFO, "NETIFY::Connection established successfully.");
    }

    result = accept(socketfd, (struct sockaddr *)client_sockaddr_in,
                    client_sockaddr_in_len);

    if (result == -1) {
        logify_log(ERROR, "NETIFY::Failed to accept connection.");
        return -1;
    }

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_sockaddr_in->sin_addr), ip_str,
              INET_ADDRSTRLEN);

    logify_log(INFO, "NETIFY::INFO::Connection accepted from ip: %s.", ip_str);

    return result;
}

int netify_connection_read(int connectionfd, char *req_buffer,
                           int req_buffer_len) {
    int result = read(connectionfd, req_buffer, req_buffer_len);
    if (result == -1) {
        logify_log(ERROR, "NETIFY::Connection read failed.");
    } else if (result == 0) {
        logify_log(INFO, "NETIFY::Connection read successfully.");
    }

    return result;
}

int netify_connection_write(int connectionfd, char *res_buffer,
                            int res_buffer_len) {
    int result = write(connectionfd, res_buffer, res_buffer_len);
    if (result == -1) {
        logify_log(ERROR, "NETIFY::Failed to write to connection.");
    } else if (result > 0) {
        logify_log(INFO, "NETIFY::Successfuly written to connection.");
    }

    return result;
}

int netify_connection_close(int connectionfd) {
    close(connectionfd);

    logify_log(INFO, "NETIFY::Successfuly closed connection.");

    return 0;
}

/*
    TODO:
    - Add validation for len parameters;
    - Add correct amount of new lines (after status_code line, after headers)
*/
int netify_response_send(int connectionfd, int status_code,
                         char *headers_buffer, int headers_buffer_len,
                         char *message_buffer, int message_buffer_len) {

    char status_code_buffer[20];
    sprintf(status_code_buffer, "HTTP/1.1 %d %s\n", status_code,
            http_status_to_string(status_code));
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

    logify_log(INFO,
               "NETIFY::INFO::Sending response with %d bytes. Response:\n%s\n",
               res_buffer_len, res_buffer);

    return netify_connection_write(connectionfd, res_buffer, res_buffer_len);
}
