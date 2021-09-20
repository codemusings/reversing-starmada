#include <stdio.h>
#include <stdlib.h>

#include "sod.h"

void sod_read_animation_channel(SODAnimationChannel *channel, FILE *file)
{
    channel->node_id = sod_read_identifier(file);

    fread(&channel->nkeyframes, sizeof(channel->nkeyframes), 1, file);
    fread(&channel->period, sizeof(channel->period), 1, file);

    /* skip unused uint16 field */
    fseek(file, sizeof(uint16_t), SEEK_CUR);

    channel->keyframes = malloc(sizeof(*channel->keyframes) * channel->nkeyframes);
    for (int i = 0; i < channel->nkeyframes; i++) {
        SODMatrix34 *keyframe = sod_new_matrix34();
        sod_read_matrix34(keyframe, file);
        channel->keyframes[i] = keyframe;
    }
}

void sod_read_animation_channels(SODAnimationChannels *channels, FILE *file)
{
    fread(&channels->count, sizeof(channels->count), 1, file);
    channels->array = malloc(sizeof(*channels->array) * channels->count);
    for (int i = 0; i < channels->count; i++) {
        SODAnimationChannel *channel = sod_new_animation_channel();
        sod_read_animation_channel(channel, file);
        channels->array[i] = channel;
    }
}

void sod_read_animation_reference(SODAnimationReference *reference, FILE *file)
{
    fread(&reference->type, sizeof(reference->type), 1, file);

    reference->node_id      = sod_read_identifier(file);
    reference->animation_id = sod_read_identifier(file);

    fread(&reference->playback_offset, sizeof(reference->playback_offset), 1, file);
}

void sod_read_animation_references(SODAnimationReferences *references, FILE *file)
{
    fread(&references->count, sizeof(references->count), 1, file);
    references->array = malloc(sizeof(*references->array) * references->count);
    for (int i = 0; i < references->count; i++) {
        SODAnimationReference *reference = sod_new_animation_reference();
        sod_read_animation_reference(reference, file);
        references->array[i] = reference;
    }
}

void sod_read_color(SODColor *color, FILE *file)
{
    fread(&color->red,   sizeof(color->red),   1, file);
    fread(&color->green, sizeof(color->green), 1, file);
    fread(&color->blue,  sizeof(color->blue),  1, file);
}

void sod_read_face(SODFace *face, FILE *file)
{
    face->face_vertices[0] = sod_new_face_vertex();
    sod_read_face_vertex(face->face_vertices[0], file);

    face->face_vertices[1] = sod_new_face_vertex();
    sod_read_face_vertex(face->face_vertices[1], file);

    face->face_vertices[2] = sod_new_face_vertex();
    sod_read_face_vertex(face->face_vertices[2], file);
}

void sod_read_face_vertex(SODFaceVertex *vertex, FILE *file)
{
    fread(&vertex->index_vertices,  sizeof(vertex->index_vertices),  1, file);
    fread(&vertex->index_texcoords, sizeof(vertex->index_texcoords), 1, file);
}

void sod_read_file(SODFile *dataset, FILE *file)
{
    dataset->header = sod_new_file_header();
    sod_read_file_header(dataset->header, file);

    dataset->lighting_materials = malloc(sizeof(*dataset->lighting_materials));
    sod_read_lighting_materials(dataset->lighting_materials, file);

    dataset->nodes = malloc(sizeof(*dataset->nodes));
    sod_read_nodes(dataset->nodes, file);

    dataset->animation_channels = malloc(sizeof(*dataset->animation_channels));
    sod_read_animation_channels(dataset->animation_channels, file);

    dataset->animation_references = malloc(sizeof(*dataset->animation_references));
    sod_read_animation_references(dataset->animation_references, file);
}

void sod_read_file_header(SODFileHeader *header, FILE *file)
{
    fread(header->id, sizeof(*header->id), 10, file);
    fread(&header->version, sizeof(header->version), 1, file);
}

char * sod_read_identifier(FILE *file)
{
    uint16_t length;
    fread(&length, sizeof(length), 1, file);
    if (length == 0) {
        return NULL;
    }

    char *text = malloc(sizeof(*text) * (length + 1));
    fread(text, sizeof(*text), length, file);
    text[length] = '\0';

    return text;
}

void sod_read_lighting_material(SODLightingMaterial *material, FILE *file)
{
    material->identifier = sod_read_identifier(file);

    material->ambient = sod_new_color();
    sod_read_color(material->ambient, file);

    material->diffuse = sod_new_color();
    sod_read_color(material->diffuse, file);

    material->specular = sod_new_color();
    sod_read_color(material->specular, file);

    fread(&material->specular_exponent, sizeof(material->specular_exponent), 1, file);
    fread(&material->lighting_model, sizeof(material->lighting_model), 1, file);
}

void sod_read_lighting_materials(SODLightingMaterials *materials, FILE *file)
{
    fread(&materials->count, sizeof(materials->count), 1, file);
    materials->array = malloc(sizeof(*materials->array) * materials->count);
    for (int i = 0; i < materials->count; i++) {
        SODLightingMaterial *material = sod_new_lighting_material();
        sod_read_lighting_material(material, file);
        materials->array[i] = material;
    }
}

void sod_read_matrix34(SODMatrix34 *matrix, FILE *file)
{
    matrix->right = sod_new_vector3();
    sod_read_vector3(matrix->right, file);

    matrix->up = sod_new_vector3();
    sod_read_vector3(matrix->up, file);

    matrix->front = sod_new_vector3();
    sod_read_vector3(matrix->front, file);

    matrix->position = sod_new_vector3();
    sod_read_vector3(matrix->position, file);
}

void sod_read_node(SODNode *node, FILE *file)
{
    fread(&node->type, sizeof(node->type), 1, file);

    node->identifier = sod_read_identifier(file);
    node->parent     = sod_read_identifier(file);

    node->local_transform = sod_new_matrix34();
    sod_read_matrix34(node->local_transform, file);

    /* only mesh and particle emitter nodes have custom data embedded */
    if (node->type == NODE_TYPE_MESH) {
        SODNodeDataMesh *data = sod_new_node_data_mesh();
        sod_read_node_data_mesh(data, file);
        node->data.mesh = data;
    } else if (node->type == NODE_TYPE_EMITTER) {
        SODNodeDataEmitter *data = sod_new_node_data_emitter();
        data->identifier = sod_read_identifier(file);
        node->data.emitter = data;
    }
}

void sod_read_node_data_mesh(SODNodeDataMesh *data, FILE *file)
{
    /* XXX Texture Material seems to be missing or in
     * a different place for versions 1.6 and lower */
    data->material_id = sod_read_identifier(file);
    data->texture_id  = sod_read_identifier(file);

    fread(&data->nvertices,  sizeof(data->nvertices),  1, file);
    fread(&data->ntexcoords, sizeof(data->ntexcoords), 1, file);
    fread(&data->ngroups,    sizeof(data->ngroups),    1, file);

    data->vertices = malloc(sizeof(*data->vertices) * data->nvertices);
    for (int i = 0; i < data->nvertices; i++) {
        SODVector3 *vertex = sod_new_vector3();
        sod_read_vector3(vertex, file);
        data->vertices[i] = vertex;
    }

    data->texcoords = malloc(sizeof(*data->texcoords) * data->ntexcoords);
    for (int i = 0; i < data->ntexcoords; i++) {
        SODVector2 *texcoords = sod_new_vector2();
        sod_read_vector2(texcoords, file);
        data->texcoords[i] = texcoords;
    }

    data->lighting_groups = malloc(sizeof(*data->lighting_groups) * data->ngroups);
    for (int i = 0; i < data->ngroups; i++) {
        SODVertexLightingGroup *group = sod_new_vertex_lighting_group();
        sod_read_vertex_lighting_group(group, file);
        data->lighting_groups[i] = group;
    }

    fread(&data->cull_type, sizeof(data->cull_type), 1, file);

    /* skip unused uint16 field */
    fseek(file, sizeof(uint16_t), SEEK_CUR);
}

void sod_read_nodes(SODNodes *nodes, FILE *file)
{
    fread(&nodes->count, sizeof(nodes->count), 1, file);
    nodes->array = malloc(sizeof(*nodes->array) * nodes->count);
    for (int i = 0; i < nodes->count; i++) {
        SODNode *node = sod_new_node();
        sod_read_node(node, file);
        nodes->array[i] = node;
    }

    /* create tree data structure for nodes */
    for (int i = 0; i < nodes->count; i++) {
        SODNode *node = nodes->array[i];
        if (!node->parent) {
            continue;
        }
        SODNode *parent = sod_find_node(nodes, node->parent);
        if (parent == NULL) {
            fprintf(stderr, "Couldn't find parent named '%s' for node '%s'\n",
                node->parent, node->identifier);
            continue;
        }
        if (!parent->children) {
            parent->children = malloc(sizeof(*parent->children));
            parent->children[0] = NULL;
        }
        uint16_t num_children;
        for (num_children = 0; parent->children[num_children] != NULL; num_children++)
            ;
        parent->children = realloc(parent->children, sizeof(*parent->children) * (num_children + 2));
        parent->children[num_children] = node;
        parent->children[num_children + 1] = NULL;
    }
}

void sod_read_vector2(SODVector2 *vector, FILE *file)
{
    fread(&vector->u, sizeof(vector->u), 1, file);
    fread(&vector->v, sizeof(vector->v), 1, file);
}

void sod_read_vector3(SODVector3 *vector, FILE *file)
{
    fread(&vector->x, sizeof(vector->x), 1, file);
    fread(&vector->y, sizeof(vector->y), 1, file);
    fread(&vector->z, sizeof(vector->z), 1, file);
}

void sod_read_vertex_lighting_group(SODVertexLightingGroup *group, FILE *file)
{
    fread(&group->nfaces, sizeof(group->nfaces), 1, file);

    group->material_id = sod_read_identifier(file);

    group->faces = malloc(sizeof(*group->faces) * group->nfaces);
    for (int i = 0; i < group->nfaces; i++) {
        SODFace *face = sod_new_face();
        sod_read_face(face, file);
        group->faces[i] = face;
    }
}

const char * sod_str_lighting_model(enum SODLightingModel model)
{
    switch (model) {
        case LIGHTING_MODEL_CONSTANT: return "Constant";
        case LIGHTING_MODEL_LAMBERT:  return "Lambert";
        case LIGHTING_MODEL_PHONG:    return "Phong";
        default:                      return "UNKNOWN";
    }
}

const char * sod_str_node_mesh_cull_type(enum SODNodeMeshCullType type)
{
    switch (type) {
        case CULL_TYPE_NO_CULL:       return "No Culling";
        case CULL_TYPE_BACKFACE_CULL: return "Backface Culling";
        default:                      return "UNKNOWN";
    }
}

const char * sod_str_node_type(enum SODNodeType type)
{
    switch (type) {
        case NODE_TYPE_NULL:    return "Null";
        case NODE_TYPE_MESH:    return "Mesh";
        case NODE_TYPE_SPRITE:  return "Sprite";
        case NODE_TYPE_LOD:     return "LOD Control Node";
        case NODE_TYPE_EMITTER: return "Emitter";
        default:                return "UNKNOWN";
    }
}
