#pragma once

#include <netinet/in.h>

#define NETIFY_MAX_RESOURCE_SIZE 256
#define NETIFY_MAX_HEADER_SIZE 1024
#define NETIFY_MAX_HEADER_SINGLE_SIZE 256
#define NETIFY_MAX_BODY_SIZE 2048
#define NETIFY_MAX_MESSAGE_SIZE 512
#define NETIFY_MAX_QUEUED_REQUEST_COUNT 5

enum HttpStatus {
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_NO_CONTENT = 204,

    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,

    HTTP_INTERNAL_SERVER_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501,
    HTTP_BAD_GATEWAY = 502,
    HTTP_SERVICE_UNAVAILABLE = 503
};

enum HttpMethod { METHOD_GET = 1, METHOD_POST = 2 };

struct HttpRequest {
    enum HttpMethod method;
};

struct HttpResponse {
};

/* Creates TCP socket and binds to provided port. Returns listening fd on
 * success and -1 on error. */
int netify_socket_bind(int port);
/* Closes TCP socket. */
int netify_socket_close(int socketfd);

/* Starts listening and on connection request returns connection fd */
int netify_connection_accept(int socketfd, struct sockaddr_in *client_sockaddr_in, socklen_t *client_sockaddr_in_len);
/* Reads into buffer from connection fd. Returns -1 on error and 0 on success.
 */
int netify_connection_read(int connectionfd, char *buffer, int buffer_len);
/* Writes from buffer to connection fd. Returns -1 on error and 0 on success. */
int netify_connection_write(int connectionfd, char *buffer, int buffer_len);
/* Closes connection fd. Retuns 0 on success. */
int netify_connection_close(int connectionfd);

/* Sends response to connection fd. Returns -1 on error and 0 on success. */
int netify_response_send(int connectionfd, int status_code, char *headers_buf, char *body_buf);

/* Reads a request resource, headers, body into resource_buf, header_buf and body_buf respectivly.
 * Returns -1 on error. */
int netify_request_read(int connectionfd, char *resource_buf, char *header_buf, char *body_buf);

/* Returns buffer with target header value or NULL if header does not exist. */
char *netify_request_header_get(const char *target_buf, const char *header_buf);

/* Returns buffer ptr with route path. */
char* netify_request_resource_get_route(const char* resource_buf);
