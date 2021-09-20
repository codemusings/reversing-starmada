#include "sod.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <SOD file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Unable to read input file %s\n", argv[1]);
        return 1;
    }

    SODFile *file = sod_new_file();
    sod_read_file(file, fp);
    fclose(fp);

    sod_print_header(file->header);
    sod_print_lighting_materials(file->lighting_materials);
    sod_print_nodes(file->nodes);
    sod_print_animation_channels(file->animation_channels);
    sod_print_animation_references(file->animation_references);

    sod_free_file(file);

    return 0;
}
