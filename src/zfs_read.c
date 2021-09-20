#include <stdlib.h>
#include <string.h>

#include "zfs.h"

void zfs_read_directory(ZFSDirectory *dir, FILE *fp, uint32_t num_files, uint32_t padding)
{
    fread(&dir->offset, sizeof(dir->offset), 1, fp);

    dir->entries = malloc((num_files + 1) * sizeof(*dir->entries));
    memset(dir->entries, 0, (num_files + 1) * sizeof(*dir->entries));
    unsigned long long size = 0;
    for (uint32_t i = 0; i < num_files; i++) {
        ZFSFileEntry *entry = malloc(sizeof(*entry));
        memset(entry, 0, sizeof(*entry));
        zfs_read_file_entry(entry, fp);
        dir->entries[i] = entry;
        size += entry->size;
    }

    fseek(fp, padding, SEEK_CUR);

    dir->data = malloc(size);
    fread(dir->data, size, 1, fp);
}

void zfs_read_file(ZFSFile *file, FILE *fp)
{
    zfs_read_file_header(&file->header, fp);

    uint32_t num_files_dir = file->header.num_files_dir;
    uint32_t num_files_total = file->header.num_files_total;
    uint32_t num_files_remaining = num_files_total;

    file->dirs = malloc(sizeof(*file->dirs));
    file->dirs[0] = NULL;

    while (num_files_remaining > num_files_dir) {

        /* resize directory array */
        uint32_t num_dirs;
        for (num_dirs = 0; file->dirs[num_dirs] != NULL; num_dirs++);
        file->dirs = realloc(file->dirs, (num_dirs + 2) * sizeof(*file->dirs));

        /* create new directory */
        ZFSDirectory *dir = malloc(sizeof(*dir));
        memset(dir, 0, sizeof(*dir));
        file->dirs[num_dirs] = dir;
        file->dirs[num_dirs + 1] = NULL;

        /* read file entries */
        zfs_read_directory(dir, fp, num_files_dir, 0);

        num_files_remaining -= num_files_dir;
    }

    /* resize directory array */
    uint32_t num_dirs;
    for (num_dirs = 0; file->dirs[num_dirs] != NULL; num_dirs++);
    file->dirs = realloc(file->dirs, (num_dirs + 2) * sizeof(*file->dirs));

    /* create new directory */
    ZFSDirectory *dir = malloc(sizeof(*dir));
    memset(dir, 0, sizeof(*dir));
    file->dirs[num_dirs] = dir;
    file->dirs[num_dirs + 1] = NULL;

    /* read file entries */
    uint32_t padding;
    padding = (num_files_dir - num_files_remaining) * sizeof(ZFSFileEntry);
    zfs_read_directory(dir, fp, num_files_remaining, padding);
}

void zfs_read_file_entry(ZFSFileEntry *entry, FILE *fp)
{
    fread(entry, sizeof(*entry), 1, fp);
}

void zfs_read_file_header(ZFSFileHeader *header, FILE *fp)
{
    fread(header, sizeof(*header), 1, fp);
}
