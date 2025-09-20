#include "daify.h"
#include "logify.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct StringArray *daify_create_string_array() {
    struct StringArray *string_array = malloc(sizeof *string_array);
    if (!string_array)
        return NULL;

    string_array->strings = malloc(DAIFY_STRING_ARRAY_CAPACITY * sizeof *string_array->strings);
    if (!string_array->strings) {
        free(string_array);
        return NULL;
    }
    string_array->count = 0;

    return string_array;
}

int daify_string_array_push(struct StringArray *string_array, const char *string) {
    char *string_copy = (char *)malloc(strlen(string) + 1);
    if (!string_copy)
        return -1;
    strcpy(string_copy, string);

    string_array->strings[string_array->count++] = string_copy;

    return 1;
}

struct StringArray *daify_string_explode(const char *target, const char *separator) {
    if (!target || !separator) {
        return NULL;
    }

    char *target_copy = malloc(strlen(target) + 1);
    struct StringArray *string_array = daify_create_string_array();
    if (!target_copy || !string_array) {
        return NULL;
    }

    strcpy(target_copy, target);

    char *separator_ptr;
    while ((separator_ptr = strstr(target_copy, separator)) != NULL) {
        ptrdiff_t string_len = separator_ptr - target_copy + 1;

        char *string = (char *)malloc(string_len);
        strncpy(string, target_copy, string_len - 1);
        string[string_len] = '\0';

        daify_string_array_push(string_array, string);
        free(string);

        target_copy = &target_copy[string_len + strlen(separator) - 1];
    }

    int string_len = 0;
    for (; target_copy[string_len] != '\0'; string_len++) {
    }

    char *string = (char *)malloc(string_len);
    strncpy(string, target_copy, string_len);
    string[string_len] = '\0';

    daify_string_array_push(string_array, string);
    free(string);

    return string_array;
}
