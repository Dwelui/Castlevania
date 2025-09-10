#include "libs/netify.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    int socketfd = netify_socket_bind(8081);
    if (socketfd == -1) {
        return 0;
    }

    int result, connectionfd,
        buffer_len = sizeof(char) * NETIFY_MAX_MESSAGE_SIZE;
    char *req_buffer = (char *)malloc(buffer_len);
    char *res_buffer = (char *)malloc(buffer_len);
    res_buffer = "Hello World!";

    while (1) {
        connectionfd = netify_connection_accept(socketfd);
        result = netify_connection_read(connectionfd, req_buffer, buffer_len);
        if (result == -1) {
            netify_connection_close(connectionfd);
            break;
        }

        printf("%s", req_buffer);

        result = netify_connection_write(connectionfd, res_buffer, 11);
        if (result == -1) {
            netify_connection_close(connectionfd);
            break;
        }

        netify_connection_close(connectionfd);
    }

    netify_socket_close(socketfd);
    free(req_buffer);

    return 0;
}
