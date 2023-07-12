#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "utils.h"

/**
 *
 */
int ENDSWITH(const char *str, const char *suffix)
{
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (suffix_len > str_len)
        return 0;

    return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}