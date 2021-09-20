#include <stdlib.h>

#include "zfs.h"

void zfs_free_directory(ZFSDirectory *dir)
{
    for (uint32_t i = 0; dir->entries[i] != NULL; i++) {
        free(dir->entries[i]);
    }
    free(dir->entries);
    free(dir->data);
}

void zfs_free_file(ZFSFile *file)
{
    for (int i = 0; file->dirs[i] != NULL; i++) {
        printf("Freeing dir #%d\n", i + 1);
        zfs_free_directory(file->dirs[i]);
    }
    free(file->dirs);
}
