#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "zfs.h"

void write_to_stdout(uint32_t *value, size_t remaining)
{
    for (int i = 0; i < remaining; i++) {
        unsigned char *p = (unsigned char *)value + i;
        write(0, p, sizeof(char));
    }
}

void write_to_file(uint32_t *value, size_t remaining, FILE *fp)
{
    // for (int i = 0; i < remaining; i++) {
    for (size_t i = 0; i < remaining; i++) {
        // unsigned char *p = (unsigned char *)value + i;
        // fwrite(p, sizeof(*p), 1, fp);
        unsigned char p = (char)((*value << (3 - i) * 8) >> 24);
        fwrite(&p, sizeof(p), 1, fp);
    }
}

uint32_t decrypt(unsigned char *buffer, size_t remaining, uint32_t key)
{
    uint32_t mask = 0;
    while (remaining > 0) {
        mask |= *(buffer + remaining - 1) << 8 * (remaining - 1);
        remaining--;
    }
    return mask ^ key;
}

void write_data(ZFSFileEntry *entry, FILE *input, uint32_t key, const char *dir)
{
    FILE *output = NULL;
    if (dir) {
        char *path = strdup(dir);
        size_t dir_len = strlen(path);
        if (path[dir_len - 1] != '/') {
            path = strcat(path, "/");
        }
        path = strcat(path, entry->name);
        output = fopen(path, "w");
    }

    /* read data */
    fseek(input, entry->offset, SEEK_SET);
    unsigned char buffer[entry->size];
    fread(buffer, entry->size, 1, input);

    /* write data */
    unsigned char *p = buffer;
    while (entry->size - (p - buffer) > sizeof(key)) {
        uint32_t value = decrypt(p, sizeof(key), key);
        if (output) {
            write_to_file(&value, sizeof(key), output);
        } else {
            write_to_stdout(&value, sizeof(key));
        }
        p += sizeof(key);
    }

    uint32_t rem = entry->size - (p - buffer);
    if (rem > 0) {
        if (output) {
            for (int i = 0; i < rem; i++) {
                fwrite(p + i, sizeof(*p), 1, output);
            }
        } else {
            for (int i = 0; i < rem; i++) {
                write(0, p + i, sizeof(char));
            }
        }
    }
    // if (output) {
    //     fputc('\n', output);
    // } else {
    //     printf("\n");
    // }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ZFS archive>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Unable to open file: %s\n", argv[1]);
        return 1;
    }

    ZFSFile file;
    zfs_read_file(&file, fp);

    printf("ZFS Archive v%.1f\n", (float)file.header.version);
    printf("  Number of files per directory: %d\n", file.header.num_files_dir);
    printf("  Number of files total: %d\n", file.header.num_files_total);
    printf("  XOR cipher key: %d\n", file.header.encryption_key);

    for (int i = 0; file.dirs[i] != NULL; i++) {
        ZFSDirectory *dir = file.dirs[i];
        printf("  Directory #%d [%d]\n", i + 1, dir->offset);
        for (int j = 0; dir->entries[j] != NULL; j++) {
            ZFSFileEntry *entry = dir->entries[j];
            char date[64];
            ctime_r((time_t *)&entry->time, date);
            printf("    [%d] %s - %d (%d) | %s", entry->record_id,
                entry->name, entry->offset, entry->size, date);

            write_data(entry, fp, file.header.encryption_key, "out/");

            // if (strcmp(entry->name, "f1ob1.txt") == 0) {
            //     FILE *out = fopen("f1ob1.txt", "w");
            //     fseek(fp, entry->offset, SEEK_SET);
            //     char buf[entry->size];
            //     fread(buf, sizeof(*buf), entry->size, fp);
            //     fwrite(buf, sizeof(*buf), entry->size, out);
            //     fclose(out);
            // }
        }
    }

    zfs_free_file(&file);
    fclose(fp);

    return 0;
}
