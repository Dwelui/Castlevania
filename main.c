#include "controller/controller_turtle.h"
#include "controller/controller_web.h"
#include "libs/logify.h"
#include "libs/netify.h"
#include "state.h"

#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static volatile sig_atomic_t g_stop = 0;

static void on_signal(int sig) {
    (void)sig;

    g_stop = 1;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, on_signal);
    state_init();

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

    while (!g_stop) {
        struct sockaddr_in client_sockaddr_in;
        socklen_t client_sockaddr_in_len = sizeof(client_sockaddr_in);

        connectionfd = netify_connection_accept(socketfd, &client_sockaddr_in, &client_sockaddr_in_len);
        if (connectionfd == -1) {
            break;
        }

        struct HttpRequest *request = netify_request_read(connectionfd);
        if (!request) {
            logify_log(ERROR, "Failed to read request");
            netify_connection_close(connectionfd);
            break;
        }

        g_state.total_requests++;
        struct HttpResponse *response = netify_response_create(HTTP_NOT_FOUND);
        if (strcmp(request->path, "/api/turtle/chopper/v1") == 0) {
            response = controller_turtle_chopper_handler(request);
        } else if (strcmp(request->path, "/api/state") == 0) {
            response = controller_web_state_index(request);
        } else {
            logify_log(ERROR, "Unsupported resource path provided: %s", request->path);
            continue;
        }

        result = netify_response_send(connectionfd, response);
        if (result == -1) {
            netify_connection_close(connectionfd);
            break;
        }

        free(response);
        free(request); // TODO: Create clean up function for structures.

        netify_connection_close(connectionfd);
    }

    state_destroy();
    netify_socket_close(socketfd);

    return 0;
}
