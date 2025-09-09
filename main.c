#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_MESSAGE_SIZE 512
#define MAX_QUEUED_REQUESTS 5

void socketListen() {
    struct sockaddr_in server_sockaddr_in;

    const int port = 8081;

    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr_in.sin_port = htons(port);

    int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    bind(socket_file_descriptor, (struct sockaddr *)&server_sockaddr_in, sizeof(server_sockaddr_in));

    while (1) {
        listen(socket_file_descriptor, MAX_QUEUED_REQUESTS);

        int connection_file_descriptor =
            accept(socket_file_descriptor, NULL, 0);

        char buffer[MAX_MESSAGE_SIZE] = {};

        read(connection_file_descriptor, buffer, sizeof(buffer));
        printf("%s", buffer);

        char status = 'I';
        write(connection_file_descriptor, &status, sizeof(status));

        close(connection_file_descriptor);
    }
}

void getLines(FILE *fileptr) {
    char letter;
    int byteCount = 0, lineCount = 0;
    char byte[9];
    char line[100];
    while (1) {
        letter = getc(fileptr);
        if (letter == EOF) {
            break;
        }

        if (letter != '\n') {
            byte[byteCount++] = letter;
        }

        if (byteCount == 8) {
            int i;
            for (i = 0; i < 8; i++) {
                line[lineCount++] = byte[i];
            }

            byteCount = 0;
        }

        if (letter == '\n') {
            int i;
            for (i = 0; i < byteCount; i++) {
                line[lineCount++] = byte[i];
            }

            byteCount = 0;

            line[lineCount] = '\0';
            printf("read: %s\n", line);
            lineCount = 0;
        }
    }
}

int main() {
    socketListen();
    // FILE *fileptr = fopen("./messages.txt", "r");
    // getLines(fileptr);
    // fclose(fileptr);

    return 0;
}
