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
        memset(string, 0, string_len);
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
    memset(string, 0, string_len);
    strncpy(string, target_copy, string_len);
    string[string_len] = '\0';

    daify_string_array_push(string_array, string);
    free(string);

    return string_array;
}

char *daify_string_implode(const struct StringArray *string_array, const char *separator) {
    int separator_len = strlen(separator);
    int result_len = string_array->count * (DAIFY_STRING_ARRAY_CAPACITY + separator_len) + 1;
    char *result_buf = malloc(result_len);
    memset(result_buf, 0, result_len);

    int string_len;
    char *string;
    int j = 0;
    for (int i = 0; i < string_array->count; i++) {
        string = string_array->strings[i];

        for (int y = 0; y < strlen(string); y++) {
            result_buf[j++] = string[y];
        }

        if (i + 1 == string_array->count) {
            break;
        }

        for (int y = 0; y < strlen(separator); y++) {
            result_buf[j++] = separator[y];
        }
    }

    result_buf[j] = '\0';

    return result_buf;
}

char *daify_string_array_find_by_substring(const struct StringArray *haystack, const char *needle) { 
    for (int i = 0; i < haystack->count; i++) {
        if (strstr(haystack->strings[i], needle) == 0) {
            return haystack->strings[i];
        }
    }

    return NULL; 
}
