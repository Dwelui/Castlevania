#include "netify.h"
#include "daify.h"
#include "logify.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const char *http_status_string(const struct HttpResponse *response) {
    switch (response->status) {
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
        logify_log(WARNING, "NETIFY::Failed to parse response status %d", response->status);
        return "Unknown Status";
    }
}

enum HttpMethod http_method_to_enum(const char *method) {
    if (strcmp("POST", method) == 0) {
        return METHOD_POST;
    } else if (strcmp("GET", method) == 0) {
        return METHOD_GET;
    }

    logify_log(WARNING, "NETIFY::Failed to parse http method to enum %s", method);
    return METHOD_GET;
}

struct HttpResponse *netify_response_create(const enum HttpStatus status) {
    struct HttpResponse *response = malloc(sizeof *response);
    response->status = status;
    response->headers = daify_string_array_create();

    return response;
}

void netify_response_delete(struct HttpResponse *response) {
    if (!response) {
        return;
    }

    daify_string_array_delete(response->headers);

    free(response);
}

void netify_request_delete(struct HttpRequest *request) {
    if (!request) {
        return;
    }

    daify_string_array_delete(request->headers);

    free(request);
}

int netify_socket_bind(int port) {
    struct sockaddr_in server_sockaddr_in;

    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr_in.sin_port = htons(port);

    int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    int result = bind(socket_file_descriptor, (struct sockaddr *)&server_sockaddr_in, sizeof(server_sockaddr_in));

    if (result == -1) {
        logify_log(ERROR, "NETIFY::Socket bind failed for port: %d.", port);
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

int netify_connection_accept(int socketfd, struct sockaddr_in *client_sockaddr_in, socklen_t *client_sockaddr_in_len) {
    int result = listen(socketfd, NETIFY_MAX_QUEUED_REQUEST_COUNT);
    if (result == -1) {
        logify_log(ERROR, "NETIFY::Failed to establish connection.");
        return -1;
    } else if (result == 0) {
        logify_log(INFO, "NETIFY::Connection established successfully.");
    }

    result = accept(socketfd, (struct sockaddr *)client_sockaddr_in, client_sockaddr_in_len);
    if (result == -1) {
        logify_log(ERROR, "NETIFY::Failed to accept connection.");
        return -1;
    }

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_sockaddr_in->sin_addr), ip_str, INET_ADDRSTRLEN);

    logify_log(INFO, "NETIFY::INFO::Connection accepted from ip: %s.", ip_str);

    return result;
}

int netify_connection_read(int connectionfd, char *buffer, int buffer_len) {
    int result = read(connectionfd, buffer, buffer_len);
    if (result == -1) {
        logify_log(ERROR, "NETIFY::Connection read failed.");
    } else if (result == 0) {
        logify_log(INFO, "NETIFY::Connection read successfully.");
    }

    return result;
}

int netify_connection_write(int connectionfd, char *buffer, int buffer_len) {
    int result = write(connectionfd, buffer, buffer_len);
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

struct HttpRequest *netify_request_create() {
    struct HttpRequest *request = malloc(sizeof *request);
    if (!request) {
        return NULL;
    }

    request->headers = daify_string_array_create();
    if (!request->headers) {
        return NULL;
    }

    return request;
}

struct HttpRequest *netify_request_read(int connectionfd) {
    struct HttpRequest *request = netify_request_create();
    if (!request) {
        return NULL;
    }

    int req_buf_len = DAIFY_STRING_ARRAY_CAPACITY;
    char *req_buf = (char *)malloc(req_buf_len + 1);
    if (!req_buf) {
        return NULL;
    }

    int result;
    struct StringArray *request_chunks = daify_string_array_create();
    while ((result = netify_connection_read(connectionfd, req_buf, req_buf_len)) != 0) {
        if (result == -1) {
            return NULL;
        }

        req_buf[req_buf_len] = '\0';
        daify_string_array_push(request_chunks, req_buf);
        memset(req_buf, 0, req_buf_len);

        if (result < req_buf_len) {
            break;
        }
    }

    free(req_buf);

    char *assembled_request_buf = daify_string_implode(request_chunks, "");

    logify_log(DEBUG, "Raw request string %s", assembled_request_buf);

    struct StringArray *request_separator = daify_string_explode(assembled_request_buf, "\r\n\r\n");

    int body_len = strlen(request_separator->strings[1]) + 1;
    if (body_len > NETIFY_MAX_BODY_SIZE) {
        logify_log(ERROR, "NETIFY::Request body too big");
        body_len = NETIFY_MAX_BODY_SIZE;
    }

    request_separator->strings[1][body_len] = '\0';
    strncpy(request->body, request_separator->strings[1], body_len);

    struct StringArray *header_lines = daify_string_explode(request_separator->strings[0], "\n");

    struct StringArray *resource_line = daify_string_explode(header_lines->strings[0], " ");
    request->method = http_method_to_enum(resource_line->strings[0]);

    int path_len = strlen(resource_line->strings[1]) + 1;
    if (path_len > NETIFY_MAX_RESOURCE_SIZE) {
        logify_log(ERROR, "NETIFY::Request resource path too big");
        path_len = NETIFY_MAX_RESOURCE_SIZE;
    }

    resource_line->strings[1][path_len] = '\0';
    strncpy(request->path, resource_line->strings[1], path_len);

    request->headers = daify_string_array_create();
    for (size_t i = 1; i < header_lines->count; i++) {
        char *header = daify_string_trim(header_lines->strings[i]);
        if (daify_string_array_push(request->headers, header) == -1) {
            logify_log(ERROR, "NETIFY::Failed to push header");
        }
    }

    free(assembled_request_buf);
    daify_string_array_delete(request_separator);
    daify_string_array_delete(header_lines);
    daify_string_array_delete(resource_line);

    return request;
}

char *netify_request_header_get(const char *target_buf, const struct HttpRequest *request) {
    char *header = daify_string_array_find_by_substring(request->headers, target_buf);
    if (!header) {
        return NULL;
    }

    struct StringArray *header_parts = daify_string_explode(header, ": ");
    size_t header_value_len = strlen(header_parts->strings[1]) + 1;
    char *header_value_buf = malloc(header_value_len);
    memcpy(header_value_buf, header_parts->strings[1], header_value_len);

    daify_string_array_delete(header_parts);
    free(header);

    return header_value_buf;
}

// TODO: Fix spacing and new lines in response
int netify_response_send(int connectionfd, const struct HttpResponse *response) {
    char status_buf[20] = {0};
    sprintf(status_buf, "HTTP/1.1 %d %s", response->status, http_status_string(response));

    struct StringArray *full_response_arr = daify_string_array_create();
    daify_string_array_push(full_response_arr, status_buf);
    daify_string_array_merge(full_response_arr, response->headers);

    daify_string_array_push(full_response_arr, "\r\n");
    daify_string_array_push(full_response_arr, response->body);
    daify_string_array_push(full_response_arr, "\n");

    char *response_buf = daify_string_implode(full_response_arr, "\r\n");
    size_t response_len = strlen(response_buf);

    logify_log(INFO, "NETIFY::INFO::Sending response with %d bytes. Response:\n%s\n", response_len, response_buf);

    return netify_connection_write(connectionfd, response_buf, response_len);
}
