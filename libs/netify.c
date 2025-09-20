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

/*
 * TODO: Refactor to pack request buffer into request structure.
 */
int netify_request_read(int connectionfd, char *resource_buf, char *header_buf, char *body_buf) {
    unsigned int req_buf_len =
        NETIFY_MAX_RESOURCE_SIZE + NETIFY_MAX_HEADER_SIZE + NETIFY_MAX_BODY_SIZE + 3; /* added padding for look ahead check. */
    char *req_buf = (char *)malloc(req_buf_len);

    int result = netify_connection_read(connectionfd, req_buf, req_buf_len);
    if (result == -1) {
        return -1;
    }

    unsigned int i, resource_idx = 0, header_idx = 0, body_idx = 0, line_count = 0;
    unsigned short request_part = 1; /* 1 - resource, 2 - header, 3 - body */
    for (i = 0; i < req_buf_len; i++) {
        if (req_buf[i] == '\r' && req_buf[i + 1] == '\n' && req_buf[i + 2] == '\r' && req_buf[i + 3] == '\n') {
            request_part = 3;
            i += 3;
            line_count++;
        } else if (req_buf[i] == '\r' && req_buf[i + 1] == '\n') {
            request_part = request_part > 2 ? request_part : 2;
            i++;
            line_count++;
        } else if ((int)req_buf[i] == 0) {
            break;
        }

        if (request_part == 1 && resource_idx < NETIFY_MAX_RESOURCE_SIZE) {
            resource_buf[resource_idx++] = req_buf[i];
        }

        if (request_part == 2 && header_idx < NETIFY_MAX_HEADER_SIZE) {
            if (header_idx == 0 && req_buf[i] == '\n')
                continue;
            header_buf[header_idx++] = req_buf[i];
        }

        if (request_part == 3 && body_idx < NETIFY_MAX_BODY_SIZE) {
            if (body_idx == 0 && req_buf[i] == '\n')
                continue;
            body_buf[body_idx++] = req_buf[i];
        }
    }

    resource_buf[resource_idx] = '\0';
    header_buf[header_idx] = '\0';
    body_buf[body_idx] = '\0';

    return result;
}

char *netify_request_header_get(const char *target_buf, const char *header_buf) {
    char *target_start_ptr = strstr(header_buf, target_buf);
    if (target_start_ptr == NULL) {
        return NULL;
    }

    int target_key_buf_len = strlen(target_buf) + 2; /* add 2 for `: ` in header line connecting key and value */
    target_start_ptr += target_key_buf_len;

    int result_buf_len = sizeof(char) * NETIFY_MAX_HEADER_SINGLE_SIZE;
    char *result_buf = (char *)malloc(result_buf_len);
    int i, j;
    for (i = 0, j = 0; i < result_buf_len; i++) {
        if (target_start_ptr[j] == '\n' || target_start_ptr[j] == '\0') {
            break;
        }

        result_buf[i] = target_start_ptr[j++];
    }

    result_buf[i] = '\0';

    return result_buf;
}

int netify_response_send(int connectionfd, int status_code, char *headers_buf, char *body_buf) {
    char status_code_buffer[20];
    sprintf(status_code_buffer, "HTTP/1.1 %d %s\r\n", status_code, http_status_to_string(status_code));

    /* Padding for header and body gap aswell as last LF at the end of body*/
    int message_padding_len = 5;

    int res_buf_len = strlen(status_code_buffer) + strlen(headers_buf) + message_padding_len + strlen(body_buf);
    char *res_buf = (char *)malloc(res_buf_len);
    int i, j = 0;

    for (i = 0; status_code_buffer[i] != '\0'; i++, j++) {
        res_buf[j] = status_code_buffer[i];
    }

    for (i = 0; headers_buf[i] != '\0'; i++, j++) {
        res_buf[j] = headers_buf[i];
    }

    for (i = 0; i < message_padding_len / 2; i++) {
        res_buf[j++] = '\r';
        res_buf[j++] = '\n';
    }

    for (i = 0; body_buf[i] != '\0'; i++, j++) {
        res_buf[j] = body_buf[i];
    }

    res_buf[j++] = '\n';
    res_buf[j] = '\0';

    logify_log(INFO, "NETIFY::INFO::Sending response with %d bytes. Response:\n%s\n", res_buf_len, res_buf);

    return netify_connection_write(connectionfd, res_buf, res_buf_len);
}

char *netify_request_resource_get_route(const char *resource_buf) {
    int len = strlen(resource_buf);
    char *result = (char *)malloc(sizeof(char) * len);
    if (!result)
        return NULL;

    /* TODO: Create stringify lib with stringify_explode function that returns StringArray object. Create DataStructify lib for Handling
     * StringArray */
    int i, j = 0, space_char_count = 0;
    for (i = 0; i < len; i++) {
        if (resource_buf[i] == '\0') {
            break;
        }

        if (resource_buf[i] == ' ') {
            space_char_count++;
            i++;
        }

        if (space_char_count == 1) {
            result[j++] = resource_buf[i];
        }

        if (space_char_count > 1) {
            break;
        }
    }

    result[j] = '\0';

    return result;
}
