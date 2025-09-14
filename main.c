#include "libs/logify.h"
#include "libs/netify.h"

#include <signal.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static volatile sig_atomic_t g_stop = 0;

static void on_signal(int sig) {
    (void)sig;

    g_stop = 1;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, on_signal);

    if (argc < 2) {
        logify_log(ERROR, "Must provide port argument");
        return -1;
    }

    int port = atoi(argv[1]);
    if (port < 0 || port > 65535) {
        logify_log(ERROR, "Invalid port provided");
        return -1;
    }

    int socketfd = netify_socket_bind(port);
    if (socketfd == -1) {
        return 0;
    }

    int result, connectionfd;

    unsigned int req_resouce_buf_len = sizeof(char) * NETIFY_MAX_RESOURCE_SIZE;
    char *req_resource_buf = (char *)malloc(req_resouce_buf_len);

    unsigned int req_header_buf_len = sizeof(char) * NETIFY_MAX_HEADER_SIZE;
    char *req_header_buf = (char *)malloc(req_header_buf_len);

    unsigned int req_body_buf_len = sizeof(char) * NETIFY_MAX_BODY_SIZE;
    char *req_body_buf = (char *)malloc(req_body_buf_len);

    char *res_body_buf = (char *)malloc(sizeof(char) * NETIFY_MAX_BODY_SIZE);

    while (!g_stop) {
        struct sockaddr_in client_sockaddr_in;
        socklen_t client_sockaddr_in_len = sizeof(client_sockaddr_in);

        connectionfd = netify_connection_accept(socketfd, &client_sockaddr_in, &client_sockaddr_in_len);
        if (connectionfd == -1) {
            break;
        }

        result = netify_request_read(connectionfd, req_resource_buf, req_resouce_buf_len, req_header_buf, req_header_buf_len, req_body_buf,
                                     req_body_buf_len);
        if (result == -1) {
            netify_connection_close(connectionfd);
            break;
        }

        logify_log(INFO, "REQUEST\nresource:\n%s\nheader:\n%s\nbody:\n%s", req_resource_buf, req_header_buf, req_body_buf);

        char *req_header_computer_name_buf = netify_request_header_get("x-computer-name", req_header_buf);

        sprintf(res_body_buf, "Hello %s!", req_header_computer_name_buf);
        result = netify_response_send(connectionfd, HTTP_OK, "", res_body_buf);
        if (result == -1) {
            netify_connection_close(connectionfd);
            break;
        }

        netify_connection_close(connectionfd);
    }

    netify_socket_close(socketfd);
    free(req_resource_buf);
    free(req_header_buf);
    free(req_body_buf);
    free(res_body_buf);

    return 0;
}
