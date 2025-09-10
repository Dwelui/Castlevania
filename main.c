#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_MESSAGE_SIZE 512
#define MAX_QUEUED_REQUESTS 5

int netify_bind_socket(int port);
char *netify_listen_socket(int socketfd, char *buffer, int buffer_len);

int main() {
    int socketfd = netify_bind_socket(8080);
    if (socketfd == -1) {
        return 0;
    }

    int request_buffer_len = sizeof(char) * MAX_MESSAGE_SIZE;
    char *request_buffer = (char *)malloc(request_buffer_len);
    while (1) {
        request_buffer =
            netify_listen_socket(socketfd, request_buffer, request_buffer_len);

        printf("%s\n", request_buffer);
    }

    close(socketfd);

    return 0;
}

int netify_bind_socket(int port) {
    struct sockaddr_in server_sockaddr_in;

    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr_in.sin_port = htons(port);

    int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    int result =
        bind(socket_file_descriptor, (struct sockaddr *)&server_sockaddr_in,
             sizeof(server_sockaddr_in));

    if (result == -1) {
        printf("NETIFY::ERROR::Socket bind failed.\n");

        return -1;
    } else {
        printf("NETIFY::INFO::Socket binded successfully.\n");
    }

    return socket_file_descriptor;
}

char *netify_listen_socket(int socketfd, char *buffer, int buffer_len) {
    listen(socketfd, MAX_QUEUED_REQUESTS);

    int connection_file_descriptor = accept(socketfd, NULL, 0);

    int result = read(connection_file_descriptor, buffer, buffer_len);
    if (result == -1) {
        printf("NETIFY::ERROR::Socket read failed.\n");
    } else if (result == 0) {
        printf("NETIFY::INFO::Socket read successfully.\n");
    }

    char status[] = { 'H','e','l','l','o',' ','W','o','r','l','d','!','\n' };
    write(connection_file_descriptor, &status, sizeof(status));

    close(connection_file_descriptor);

    return buffer;
}
