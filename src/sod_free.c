#include <stdlib.h>
#include "sod.h"

void sod_free_animation_channel(SODAnimationChannel *channel)
{
    if (!channel) {
        return;
    }
    free(channel->node_id);
    if (channel->keyframes) {
        for (int i = 0; i < channel->nkeyframes; i++) {
            sod_free_matrix34(channel->keyframes[i]);
        }
        free(channel->keyframes);
    }
    free(channel);
}

void sod_free_animation_reference(SODAnimationReference *reference)
{
    if (!reference) {
        return;
    }
    free(reference->node_id);
    free(reference->animation_id);
    free(reference);
}

void sod_free_face(SODFace *face)
{
    if (!face) {
        return;
    }
    size_t total = sizeof(face->face_vertices);
    size_t ptsize = sizeof(*face->face_vertices);
    size_t nvertices = total / ptsize;
    for (int i = 0; i < nvertices; i++) {
        if (face->face_vertices[i]) {
            free(face->face_vertices[i]);
        }
    }
    free(face);
}

void sod_free_file(SODFile *file)
{
    if (!file) {
        return;
    }
    free(file->header);
    sod_free_list(file->lighting_materials,   sod_free_lighting_material);
    sod_free_list(file->nodes,                sod_free_node);
    sod_free_list(file->animation_channels,   sod_free_animation_channel);
    sod_free_list(file->animation_references, sod_free_animation_reference);
    free(file);
}

void sod_free_lighting_material(SODLightingMaterial *lighting_material)
{
    if (!lighting_material) {
        return;
    }
    free(lighting_material->identifier);
    if (lighting_material->ambient) {
        free(lighting_material->ambient);
    }
    if (lighting_material->diffuse) {
        free(lighting_material->diffuse);
    }
    if (lighting_material->specular) {
        free(lighting_material->specular);
    }
    free(lighting_material);
}

void sod_free_matrix34(SODMatrix34 *matrix)
{
    if (!matrix) {
        return;
    }
    if (matrix->right) {
        free(matrix->right);
    }
    if (matrix->up) {
        free(matrix->up);
    }
    if (matrix->front) {
        free(matrix->front);
    }
    if (matrix->position) {
        free(matrix->position);
    }
    free(matrix);
}

void sod_free_node(SODNode *node)
{
    if (!node) {
        return;
    }
    free(node->identifier);
    free(node->parent);
    if (node->children) {
        free(node->children);
    }
    sod_free_matrix34(node->local_transform);
    if (node->type == NODE_TYPE_MESH) {
        sod_free_node_data_mesh(node->data.mesh);
    } else if (node->type == NODE_TYPE_EMITTER) {
        if (node->data.emitter) {
            free(node->data.emitter->identifier);
            free(node->data.emitter);
        }
    }
    free(node);
}

void sod_free_node_data_mesh(SODNodeDataMesh *data)
{
    if (!data) {
        return;
    }
    free(data->material_id);
    free(data->texture_id);
    if (data->vertices) {
        for (int i = 0; i < data->nvertices; i++) {
            if (data->vertices[i]) {
                free(data->vertices[i]);
            }
        }
        free(data->vertices);
    }
    if (data->texcoords) {
        for (int i = 0; i < data->ntexcoords; i++) {
            if (data->texcoords[i]) {
                free(data->texcoords[i]);
            }
        }
        free(data->texcoords);
    }
    if (data->lighting_groups) {
        for (int i = 0; i < data->ngroups; i++) {
            sod_free_vertex_lighting_group(data->lighting_groups[i]);
        }
        free(data->lighting_groups);
    }
    free(data);
}

void sod_free_vertex_lighting_group(SODVertexLightingGroup *group)
{
    if (!group) {
        return;
    }
    free(group->material_id);
    if (group->faces) {
        for (int i = 0; i < group->nfaces; i++) {
            if (group->faces[i]) {
                sod_free_face(group->faces[i]);
            }
        }
        free(group->faces);
    }
    free(group);
}
