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

    char *req_resource_buf = (char *)malloc(sizeof(char) * NETIFY_MAX_RESOURCE_SIZE);
    char *req_header_buf = (char *)malloc(sizeof(char) * NETIFY_MAX_HEADER_SIZE);
    char *req_body_buf = (char *)malloc(sizeof(char) * NETIFY_MAX_BODY_SIZE);

    char *res_body_buf = (char *)malloc(sizeof(char) * NETIFY_MAX_BODY_SIZE);

    while (!g_stop) {
        struct sockaddr_in client_sockaddr_in;
        socklen_t client_sockaddr_in_len = sizeof(client_sockaddr_in);

        connectionfd = netify_connection_accept(socketfd, &client_sockaddr_in, &client_sockaddr_in_len);
        if (connectionfd == -1) {
            break;
        }

        result = netify_request_read(connectionfd, req_resource_buf, req_header_buf, req_body_buf);
        if (result == -1) {
            netify_connection_close(connectionfd);
            break;
        }

        g_state.total_requests++;
        char *route = netify_request_resource_get_route(req_resource_buf);
        char *response;
        if (strcmp(route, "/api/turtle/chopper/v1") == 0) {
            response = controller_turtle_chopper_handler(req_resource_buf, req_header_buf, req_body_buf);
        } else if (strcmp(route, "/api/state") == 0) {
            struct HttpRequest *request = netify_request_to_http_request(req_resource_buf, req_header_buf, req_body_buf);

            struct HttpResponse *httpResponse = controller_web_state_index(request);
        } else {
            logify_log(ERROR, "Unsupported route provided: %s", route);
            continue;
        }

        result = netify_response_send(connectionfd, HTTP_OK, "Hello World!", response);
        if (result == -1) {
            netify_connection_close(connectionfd);
            break;
        }

        free(response);
        free(route);

        netify_connection_close(connectionfd);
    }

    state_destroy();
    netify_socket_close(socketfd);
    free(req_resource_buf);
    free(req_header_buf);
    free(req_body_buf);
    free(res_body_buf);

    return 0;
}
