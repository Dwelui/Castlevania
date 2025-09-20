#pragma once

#include <stddef.h>

#define DAIFY_STRING_ARRAY_CAPACITY 128

struct StringArray {
    char **strings;
    size_t count;
};

struct StringArray *daify_create_string_array();
int daify_string_array_push(struct StringArray *string_array, const char *string);

struct StringArray *daify_string_explode(const char *target, const char *separator);
char *daify_string_implode(const struct StringArray *string_array, const char *separator);
char *daify_string_array_find_by_substring(const struct StringArray *haystack, const char *needle);
