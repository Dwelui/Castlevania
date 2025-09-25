#include "daify.h"
#include "logify.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct StringArray *daify_string_array_create() {
    struct StringArray *string_array = malloc(sizeof *string_array);
    if (!string_array) return NULL;

    string_array->strings = malloc(DAIFY_STRING_ARRAY_CAPACITY * sizeof *string_array->strings);
    if (!string_array->strings) {
        free(string_array);
        return NULL;
    }
    string_array->count = 0;

    return string_array;
}

void daify_string_array_delete(struct StringArray *string_array) {
    if (!string_array) { return; }

    for (size_t i = 0; i < string_array->count; i++) {
        free(string_array->strings[i]);
    }

    free(string_array->strings);
    free(string_array);
}

char *daify_string_copy(const char *src) {
    if (!src) {
        logify_log(DEBUG, "DAIFY::daify_string_copy::null pointer passed");
        return NULL;
    }

    size_t len = strlen(src);

    char *copy = malloc(len + 1);
    if (!copy) { return NULL; }

    memcpy(copy, src, len);
    copy[len] = '\0';

    return copy;
}

char *daify_string_copy_n(const char *src, const size_t len) {
    if (!src) {
        logify_log(DEBUG, "DAIFY::daify_string_copy_n::null pointer passed");
        return NULL;
    }

    char *copy = malloc(len + 1);
    if (!copy) { return NULL; }

    memcpy(copy, src, len);
    copy[len] = '\0';

    return copy;
}

int daify_string_array_push(struct StringArray *string_array, const char *string) {
    if (!string_array || !string) {
        logify_log(DEBUG, "DAIFY::daify_string_array_push::null pointer passed");
        return -1;
    }

    char *copy = daify_string_copy(string);
    if (!copy) {
        logify_log(DEBUG, "DAIFY::daify_string_array_push::failed to copy string");
        return -1;
    }

    string_array->strings[string_array->count++] = copy;

    return 1;
}

int daify_string_array_push_n(struct StringArray *string_array, const char *string, const size_t len) {
    if (!string_array || !string) {
        logify_log(DEBUG, "DAIFY::daify_string_array_push_n::null pointer passed");
        return -1;
    }

    char *copy = daify_string_copy_n(string, len);
    if (!copy) {
        logify_log(DEBUG, "DAIFY::daify_string_array_push_n::failed to copy string");
        return -1;
    }

    string_array->strings[string_array->count++] = copy;

    return 1;
}

int daify_string_array_merge(struct StringArray *dest, const struct StringArray *src) {
    if (!dest || !src) { return 0; }

    if (src->count == 0) { return 0; }

    for (size_t i = 0; i < src->count; i++) {
        daify_string_array_push(dest, src->strings[i]);
    }

    return 1;
}

struct StringArray *daify_string_explode(const char *target, const char *separator) {
    if (!target || !separator) {
        logify_log(DEBUG, "DAIFY::daify_string_explode::null pointer passed");
        return NULL; 
    }

    size_t target_len = strlen(target);
    size_t separator_len = strlen(separator);
    if (separator_len > target_len) {
        logify_log(WARNING, "DAIFY::daify_string_explode::separator is larget than target");
        return NULL;
    }

    char *target_copy = daify_string_copy(target);
    struct StringArray *string_array = daify_string_array_create();
    if (!target_copy || !string_array) { return NULL; }

    char *separator_ptr;
    while ((separator_ptr = strstr(target_copy, separator)) != NULL) {
        size_t len = (size_t)(separator_ptr - target_copy);

        if (daify_string_array_push_n(string_array, target_copy, len) == -1) {
            logify_log(WARNING, "DAIFY::daify_string_explode::failed to push string");
            return NULL;
        }

        target_copy = separator_ptr + separator_len;
    }

    if (daify_string_array_push(string_array, target_copy) == -1) {
        logify_log(WARNING, "DAIFY::daify_string_explode::failed to push last string");
        return NULL;
    }

    return string_array;
}

int daify_string_concat(char *dest, const size_t dest_cap, const char *src) {
    if (!dest || !src) {
        logify_log(DEBUG, "DAIFY::daify_string_concat::null pointer passed");
        return -1;
    }

    size_t src_len = strlen(src);
    if (src_len == 0) { return 1; }

    size_t dest_len = strlen(dest);

    if (dest_len > dest_cap - 1) {
        logify_log(WARNING, "DAIFY::daify_string_concat::dest string no space for terminator");
        return -1;
    }

    size_t avail_len = dest_cap - dest_len - 1;
    if (src_len > avail_len) {
        logify_log(WARNING, "DAIFY::daify_string_concat::not enough space for src string. src len %d, avail len %d", src_len, avail_len);
        return -1;
    }

    memcpy(dest + dest_len, src, src_len);
    dest[dest_len + src_len] = '\0';

    return 0;
}

char *daify_string_implode(const struct StringArray *string_arr, const char *separator) {
    if (!string_arr || !separator) {
        logify_log(DEBUG, "DAIFY::daify_string_implode::null pointer passed");
        return NULL;
    }

    if (string_arr->count == 0) {
        logify_log(WARNING, "DAIFY::daify_string_implode::empty string array");
        return NULL;
    }

    if (string_arr->count == 1) {
        return daify_string_copy(string_arr->strings[0]);
    }

    size_t separator_len = strlen(separator);
    size_t len = string_arr->count * (DAIFY_STRING_ARRAY_CAPACITY + separator_len);

    char *result = malloc(len + 1);
    if (!result) { return NULL; }
    result[0] = '\0';

    for (size_t i = 0; i < string_arr->count; i++) {
        if (daify_string_concat(result, len, string_arr->strings[i]) == -1) {
            logify_log(WARNING, "DAIFY::daify_string_implode::failed to concat string");
            return NULL;
        }

        if (i + 1 != string_arr->count && separator_len != 0) {
            if (daify_string_concat(result, len, separator) == -1) {
                logify_log(WARNING, "DAIFY::daify_string_implode::failed to concat separator");
                return NULL;
            }
        }
    }

    result[len] = '\0';

    return result;
}

char *daify_string_array_find_by_substring(const struct StringArray *haystack, const char *needle) {
    if (!haystack || !needle) { return NULL; }

    for (size_t i = 0; i < haystack->count; i++) {
        if (strstr(haystack->strings[i], needle) != NULL) {
            size_t result_len = strlen(haystack->strings[i]) + 1;
            char *result_buf = malloc(result_len);
            if (!result_buf) { return NULL; }

            memcpy(result_buf, haystack->strings[i], result_len);

            return result_buf;
        }
    }

    return NULL;
}

char *daify_string_trim_start(const char *string) {
    if (!string) { return NULL; }

    for (size_t i = 0; i < strlen(string); i++) {
        if ((int)string[i] > 32) { // Characters above space symbol in ascii
            size_t result_len = strlen(string) - i + 1;
            char *result_buf = malloc(result_len);
            if (!result_buf) { return NULL; }

            strncpy(result_buf, &string[i], result_len);
            return result_buf;
        }
    }

    return NULL;
}

char *daify_string_trim_end(const char *string) {
    if (!string) { return NULL; }

    for (int i = strlen(string); i > 0; i--) {
        if ((int)string[i] > 32) { // Characters above space symbol in ascii
            size_t result_len = i + 1; // nono to + 1
            char *result_buf = malloc(result_len); // bruv use copy with n
            if (!result_buf) { return NULL; }

            strncpy(result_buf, string, result_len);
            return result_buf;
        }
    }

    return NULL;
}

char *daify_string_trim(const char *string) {
    if (!string) { return NULL; }

    size_t string_copy_len = strlen(string) + 1; // bruv wth, use copy // nono to + 1
    char *string_copy_buf = malloc(string_copy_len);
    if (!string_copy_buf) { return NULL; }

    strncpy(string_copy_buf, string, string_copy_len);

    char *result_buf = daify_string_trim_start(string_copy_buf);
    if (!result_buf) { return NULL; }

    result_buf = daify_string_trim_end(result_buf);
    if (!result_buf) { return NULL; }

    return result_buf;
}
