#ifndef NETIFY_H
#define NETIFY_H

#define NETIFY_MAX_MESSAGE_SIZE 512
#define NETIFY_MAX_QUEUED_REQUEST_COUNT 5

/* Creates TCP socket and binds to provided port. Returns listening fd on success and -1 on error. */
int netify_socket_bind(int port);
/* Closes TCP socket. */
int netify_socket_close(int socketfd);

/* Starts listening and on connection request returns connection fd */
int netify_connection_accept(int socketfd);
/* Reads into req_buffer from connection fd. Returns -1 on error and 0 on success. */
int netify_connection_read(int connectionfd, char *req_buffer, int req_buffer_len);
/* Writes from res_buffer to connection fd. Returns -1 on error and 0 on success. */
int netify_connection_write(int connectionfd, char *res_buffer, int res_buffer_len);
/* Closes connection fd. Retuns 0 on success. */
int netify_connection_close(int connectionfd);

/* Sends response to connection fd. Returns -1 on error and 0 on success. */
int netify_response_send(int connectionfd, int status_code, char *headers_buffer, int headers_buffer_len, char *message_buffer, int message_buffer_len);

#endif
