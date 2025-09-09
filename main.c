#include <stdio.h>

int main() {
    FILE *fileptr = fopen("./messages.txt", "r");

    char byte[9];
    int byteCount = 0;
    char letter;
    while (1) {
        letter = getc(fileptr);
        if (letter == EOF) {
            byte[byteCount] = '\0';
            printf("read: %s", byte);

            break;
        }

        byte[byteCount] = letter;
        byteCount++;

        if (byteCount == 8) {
            byte[byteCount] = '\0';
            printf("read: %s\n", byte);

            byteCount = 0;
        }
    }

    return 0;
}
