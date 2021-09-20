#include <stdbool.h>
#include <string.h>
#include "sod.h"

static bool sod_matches_identifier(const char *a, const char *b)
{
    if ((a == NULL && b != NULL) || (a != NULL && b == NULL)) {
        return false;
    } else if (strlen(a) != strlen(b)) {
        return false;
    } else {
        return strcmp(a, b) == 0;
    }
}

SODNode *
sod_find_node(SODNodes *nodes, const char *name)
{
    for (int i = 0; i < nodes->count; i++) {
        SODNode *node = nodes->array[i];
        if (sod_matches_identifier(node->identifier, name)) {
            return node;
        }
    }
    return NULL;
}
