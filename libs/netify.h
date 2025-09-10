#ifndef NETIFY_H
#define NETIFY_H

#define NETIFY_MAX_MESSAGE_SIZE 512
#define NETIFY_MAX_QUEUED_REQUEST_COUNT 5

/* Creates TCP socket and binds to provided port. Returns listening fd on success and -1 on error. */
int netify_socket_bind(int port);
/* Closes TCP socket. */
int netify_socket_close(int socketfd);

/* Accepts connection and reads into buffer (buffer_len bytes). */
char *netify_listen_socket(int socketfd, char *buffer, int buffer_len);

#endif
