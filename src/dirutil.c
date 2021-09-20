#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "sod.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <SOD file dir>\n", argv[0]);
        return 1;
    }

    DIR *dir = opendir(argv[1]);
    if (!dir) {
        fprintf(stderr, "Unable to read directory: '%s'\n", argv[1]);
        return 1;
    }

    size_t len_dirname = strlen(argv[1]);
    bool has_trailing_slash = argv[1][len_dirname - 1] == '/';

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {

        if (strlen(entry->d_name) == 1 && strncmp(entry->d_name, ".", 1) == 0)
            continue;
        if (strlen(entry->d_name) == 2 && strncmp(entry->d_name, "..", 2) == 0)
            continue;

        /* assemble filepath */
        size_t len_filepath = len_dirname;
        if (!has_trailing_slash)
            len_filepath++;
        len_filepath += strlen(entry->d_name);
        len_filepath++; // null terminator

        char filepath[len_filepath];
        snprintf(filepath, len_filepath, "%s%s%s", argv[1],
            has_trailing_slash ? "" : "/", entry->d_name);

        /* open file */
        FILE *file = fopen(filepath, "r");
        if (!file) {
            fprintf(stderr, "Unable to read file: %s\n", filepath);
            continue;
        }

        /* print header */
        SODFileHeader header;
        sod_read_file_header(&header, file);
        fwrite(header.id, sizeof(*header.id), 10, stdout);
        printf(" %.1f %s\n", header.version, filepath);

        fclose(file);
    }

    closedir(dir);

    return 0;
}
