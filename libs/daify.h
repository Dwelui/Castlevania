#pragma once

#include <stddef.h>

#define DAIFY_STRING_ARRAY_CAPACITY 128

struct StringArray {
    char **strings;
    size_t count;
};

struct StringArray *daify_string_explode(const char *target, const char *separator);
