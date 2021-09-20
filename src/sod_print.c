#include "sod.h"

void sod_print_animation_channel(SODAnimationChannel *channel, uint8_t offset)
{
    sod_print_identifier(channel->node_id, "Node", offset);
    printf("%*cDuration: %f\n", offset, ' ', channel->period);
    printf("%*cKey frames: %u\n", offset, ' ', channel->nkeyframes);
    for (int i = 0; i < channel->nkeyframes; i++) {
        printf("%*c  #%d\n", offset, ' ', i + 1);
        sod_print_matrix34(channel->keyframes[i], offset + 4);
    }
}

void sod_print_animation_channels(SODAnimationChannels *channels)
{
    printf("ANIMATION CHANNELS\n");
    printf("  Count: %d\n", channels->count);
    for (int i = 0; i < channels->count; i++) {
        printf("  #%d\n", i + 1);
        sod_print_animation_channel(channels->array[i], 4);
    }
}

void sod_print_animation_reference(SODAnimationReference *reference, uint8_t offset)
{
    printf("%*cType: %d\n", offset, ' ', reference->type);
    sod_print_identifier(reference->node_id, "Node", offset);
    sod_print_identifier(reference->animation_id, "Animation", offset);
    printf("%*cPlayback offset: %f\n", offset, ' ', reference->playback_offset);
}

void sod_print_animation_references(SODAnimationReferences *references)
{
    printf("ANIMATION REFERENCES\n");
    printf("  Count: %d\n", references->count);
    for (int i = 0; i < references->count; i++) {
        printf("  #%d\n", i + 1);
        sod_print_animation_reference(references->array[i], 4);
    }
}

void sod_print_header(SODFileHeader *header)
{
    printf("FILE HEADER\n");

    printf("  Identifier: ");
    fwrite(header->id, sizeof(*header->id), 10, stdout);
    printf("\n");

    printf("  Version: %f\n", header->version);
}

void sod_print_identifier(const char *id, const char *name, uint8_t offset)
{
    printf("%*c%s: %s\n", offset, ' ',
        name ? name : "Identifier",
        id ? id : "NULL");
}

void sod_print_lighting_material(SODLightingMaterial *material)
{
    sod_print_identifier(material->identifier, NULL, 4);

    SODColor *c = material->ambient;
    printf("    Ambient:  [r: %f, g: %f, b: %f]\n", c->red, c->green, c->blue);

    c = material->diffuse;
    printf("    Diffuse:  [r: %f, g: %f, b: %f]\n", c->red, c->green, c->blue);

    c = material->specular;
    printf("    Specular: [r: %f, g: %f, b: %f]\n", c->red, c->green, c->blue);

    printf("    Specular Exponent: %f\n", material->specular_exponent);
    printf("    Lighting Model: %s\n",
        sod_str_lighting_model(material->lighting_model));
}

void sod_print_lighting_materials(SODLightingMaterials *materials)
{
    printf("LIGHTING MATERIALS\n");
    printf("  Count: %d\n", materials->count);
    for (int i = 0; i < materials->count; i++) {
        printf("  #%d\n", i + 1);
        sod_print_lighting_material(materials->array[i]);
    }
}

void sod_print_matrix34(SODMatrix34 *matrix, uint8_t offset)
{
    printf("%*c[ %.1f, %.1f, %.1f, %.1f ]\n", offset, ' ',
        matrix->right->x,
        matrix->up->x,
        matrix->front->x,
        matrix->position->x);
    printf("%*c[ %.1f, %.1f, %.1f, %.1f ]\n", offset, ' ',
        matrix->right->y,
        matrix->up->y,
        matrix->front->y,
        matrix->position->y);
    printf("%*c[ %.1f, %.1f, %.1f, %.1f ]\n", offset, ' ',
        matrix->right->z,
        matrix->up->z,
        matrix->front->z,
        matrix->position->z);
}

void sod_print_node(SODNode *node)
{
    printf("    Node type: %s\n", sod_str_node_type(node->type));
    sod_print_identifier(node->identifier, NULL, 4);
    printf("    Parent: %s\n", node->parent ? node->parent : "NULL");

    printf("    Local Transformation Matrix:\n");
    sod_print_matrix34(node->local_transform, 6);

    if (node->type == NODE_TYPE_MESH) {
        sod_print_node_data_mesh(node->data.mesh, 4);
    } else if (node->type == NODE_TYPE_EMITTER) {
        char *ident = node->data.emitter->identifier;
        printf("    Particle Emitter Identifier: %s\n", ident ? ident : "NULL");
    }
}

void sod_print_node_data_mesh(SODNodeDataMesh *data, uint8_t offset)
{
    sod_print_identifier(data->material_id, "Texture Material", offset);
    sod_print_identifier(data->texture_id, "Texture", offset);

    printf("%*cVertices: %u\n", offset, ' ', data->nvertices);
    for (int i = 0; i < data->nvertices; i++) {
        printf("%*c  #%d [x: %f, y: %f, z: %f]\n", offset, ' ', i + 1,
            data->vertices[i]->x,
            data->vertices[i]->y,
            data->vertices[i]->z);
    }

    printf("%*cTexture coordinates: %u\n", offset, ' ', data->ntexcoords);
    for (int i = 0; i < data->ntexcoords; i++) {
        printf("%*c  #%d [u: %f, v: %f]\n", offset, ' ', i + 1,
            data->texcoords[i]->u,
            data->texcoords[i]->v);
    }

    printf("%*cVertex lighting groups: %u\n", offset, ' ', data->ngroups);
    for (int i = 0; i < data->ngroups; i++) {
        printf("%*c  #%d\n", offset, ' ', i + 1);
        sod_print_vertex_lighting_group(data->lighting_groups[i], offset + 4);
    }

    printf("%*cCulling type: %s\n", offset, ' ',
        sod_str_node_mesh_cull_type(data->cull_type));
}

void sod_print_nodes(SODNodes *nodes)
{
    printf("NODES\n");
    printf("  Count: %d\n", nodes->count);
    for (int i = 0; i < nodes->count; i++) {
        printf("  #%d\n", i + 1);
        sod_print_node(nodes->array[i]);
    }
}

void sod_print_vertex_lighting_group(SODVertexLightingGroup *group,
                                     uint8_t offset)
{
    sod_print_identifier(group->material_id, "Lighting Material", offset);
    printf("%*cFaces: %u\n", offset, ' ', group->nfaces);
    for (int i = 0; i < group->nfaces; i++) {
        printf("%*c  #%d [v: %d, t: %d], [v: %d, t: %d], [v: %d, t: %d]\n",
            offset, ' ', i + 1,
            group->faces[i]->face_vertices[0]->index_vertices,
            group->faces[i]->face_vertices[0]->index_texcoords,
            group->faces[i]->face_vertices[1]->index_vertices,
            group->faces[i]->face_vertices[1]->index_texcoords,
            group->faces[i]->face_vertices[2]->index_vertices,
            group->faces[i]->face_vertices[2]->index_texcoords);
    }
}
