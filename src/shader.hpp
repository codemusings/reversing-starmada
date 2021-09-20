#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>

class Shader
{
    private:
        unsigned int shaderProgramId;
        unsigned int compileShader(const char* shader, const GLenum shaderType);
    public:
        Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
        void use();
        void setInt(const char *name, const int value) const;
        void setMat4(const char *name, const glm::mat4 matrix) const;
};

#endif//__SHADER_H__
