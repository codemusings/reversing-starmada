#ifndef __MODEL_H__
#define __MODEL_H__

#include "sod.h"

class Model {
    private:
        float *vertices;
        int numberOfVertices;
        unsigned char *textureData;
        int textureWidth;
        int textureHeight;
    public:
        Model(const SODNode *meshNode, const char *textureFilePath);
        ~Model();
        float * getVertices() { return vertices; }
        int getNumberOfVertices() { return numberOfVertices; }
        unsigned char * getTextureData() { return textureData; }
        int getTextureWidth() { return textureWidth; }
        int getTextureHeight() { return textureHeight; }
};

#endif//__MODEL_H__
