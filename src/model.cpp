#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "model.hpp"

Model::Model(const SODNode *meshNode, const char *textureFilePath)
{
    SODNodeDataMesh *data = meshNode->data.mesh;
    int stride = 3 + 3 + 2; // coords + color + tex_coords
    numberOfVertices = 0;
    for (int i = 0; i < data->ngroups; i++) {
        SODVertexLightingGroup *group = data->lighting_groups[i];
        int group_size = 0;
        for (int j = 0; j < group->nfaces; j++) {
            group_size = group->nfaces * 3 * stride;
        }
        numberOfVertices += group_size;
    }

    vertices = new float[numberOfVertices];
    int lgroup_offset = 0;
    for (int i = 0; i < data->ngroups; i++) {
        SODVertexLightingGroup *group = data->lighting_groups[i];
        for (int j = 0; j < group->nfaces; j++) {
            // 3 * (3 + 3 + 2) = 24
            int face_offset = lgroup_offset + j * 3 * stride;
            for (int k = 0; k < 3; k++) {
                SODFaceVertex *vertex = group->faces[j]->face_vertices[k];
                uint16_t v = vertex->index_vertices;
                uint16_t t = vertex->index_texcoords;
                // coords
                vertices[face_offset + k * stride + 0] = data->vertices[v]->x;
                vertices[face_offset + k * stride + 1] = data->vertices[v]->y;
                vertices[face_offset + k * stride + 2] = data->vertices[v]->z;
                // color
                vertices[face_offset + k * stride + 3] = 1.0f;
                vertices[face_offset + k * stride + 4] = 1.0f;
                vertices[face_offset + k * stride + 5] = 1.0f;
                // texture coords
                vertices[face_offset + k * stride + 6] = data->texcoords[t]->u;
                vertices[face_offset + k * stride + 7] = data->texcoords[t]->v;
            }
        }
        lgroup_offset += group->nfaces * 3 * stride;
    }

    textureData = stbi_load(textureFilePath, &textureWidth, &textureHeight, NULL, 0);
    if (!textureData) {
        throw std::runtime_error(std::string("Unable to load texture: ") + textureFilePath);
    }
}

Model::~Model()
{
    delete vertices;
    stbi_image_free(textureData);
}
