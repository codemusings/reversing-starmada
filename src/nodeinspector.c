#include <stdio.h>
#include <stdlib.h>

#include "sod.h"

void print_node(SODNode *node, int offset)
{
    printf("%*c%s [%s] %s\n",
        offset, ' ',
        node->identifier,
        sod_str_node_type(node->type),
        node->type == 1 ? node->data.mesh->texture_id : "");
    if (node->children) {
        for (uint16_t i = 0; node->children[i] != NULL; i++) {
            SODNode *child = node->children[i];
            print_node(child, offset + 4);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <SOD file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Unable to open SOD input file\n");
        return 1;
    }

    SODFile *file = sod_new_file();
    sod_read_file(file, fp);
    fclose(fp);

    SODNode *root = sod_find_node(file->nodes, "root");
    print_node(root, 0);

    sod_free_file(file);

    return 0;
}
