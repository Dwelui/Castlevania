#include "libs/netify.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    int socketfd = netify_socket_bind(8080);
    if (socketfd == -1) {
        return 0;
    }

    int request_buffer_len = sizeof(char) * NETIFY_MAX_MESSAGE_SIZE;
    char *request_buffer = (char *)malloc(request_buffer_len);
    while (1) {
        request_buffer =
            netify_listen_socket(socketfd, request_buffer, request_buffer_len);

        printf("%s\n", request_buffer);
    }

    netify_socket_close(socketfd);

    return 0;
}
