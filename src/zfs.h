#ifndef __ZFS_H__
#define __ZFS_H__

#include <stdint.h>
#include <stdio.h>


/*
 * https://github.com/civctp2/civctp2
 * https://wiki.xentax.com/index.php/Interstate_76
 * https://fileinfo.com/extension/odf
 * https://fileinfo.com/extension/tt
 */

typedef struct {
    char     name[16];
    uint32_t offset;
    uint32_t record_id;
    uint32_t size;
    uint32_t time;
    uint32_t flags;
} ZFSFileEntry;

typedef struct {
    uint32_t        offset; // file offset to the next directory
    ZFSFileEntry  **entries;
    unsigned char  *data;
} ZFSDirectory;

typedef struct {
    char     id[4];
    uint32_t version;
    uint32_t max_recordname_length;
    uint32_t num_files_dir;
    uint32_t num_files_total;
    uint32_t encryption_key;
    uint32_t dir_table_head;
} ZFSFileHeader;

typedef struct {
    ZFSFileHeader   header;
    ZFSDirectory  **dirs;
} ZFSFile;


/**
 * F U N C T I O N S
 */
void zfs_free_directory(ZFSDirectory *);
void zfs_free_file(ZFSFile *);

void zfs_read_directory(ZFSDirectory *, FILE *, uint32_t, uint32_t);
void zfs_read_file(ZFSFile *, FILE *);
void zfs_read_file_entry(ZFSFileEntry *, FILE *);
void zfs_read_file_header(ZFSFileHeader *, FILE *);

#endif//__ZFS_H__
