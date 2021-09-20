#include <stdio.h>
#include "sod.h"

int main(int argc, char *argv[])
{
    FILE *f = fopen(argv[1], "r");
    fseek(f, 0x8b, SEEK_SET);

    SODNodes nodes;
    sod_read_nodes(&nodes, f);
    sod_print_nodes(&nodes);

    fclose(f);
    return 0;
}
