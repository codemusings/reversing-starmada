#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <errno.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    std::string vertexShaderCode;
    std::string fragmentShaderCode;

    std::stringstream vertexShaderStream;
    std::stringstream fragmentShaderStream;

    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;

    vertexShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
    try {
        vertexShaderFile.open(vertexShaderPath);
        vertexShaderStream << vertexShaderFile.rdbuf();
        vertexShaderFile.close();
        vertexShaderCode = vertexShaderStream.str();
        fragmentShaderFile.open(fragmentShaderPath);
        fragmentShaderStream << fragmentShaderFile.rdbuf();
        fragmentShaderFile.close();
        fragmentShaderCode = fragmentShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "Unable to read shader file: " << strerror(errno) << std::endl;
    }

    unsigned int vertexShader = compileShader(vertexShaderCode.c_str(), GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentShaderCode.c_str(), GL_FRAGMENT_SHADER);

    shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShader);
    glAttachShader(shaderProgramId, fragmentShader);
    glLinkProgram(shaderProgramId);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

unsigned int
Shader::compileShader(const char *shader, const GLenum shaderType)
{
    int success;
    char buffer[1024];
    unsigned int shaderId;

    shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shader, NULL);
    glCompileShader(shaderId);
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cerr << "Shader compilation failed" << std::endl;
        glGetShaderInfoLog(shaderId, sizeof(buffer), NULL, buffer);
        std::cerr << buffer << std::endl;
    }

    return shaderId;
}

void
Shader::setInt(const char *name, const int value) const
{
    unsigned int valueId = glGetUniformLocation(shaderProgramId, name);
    glUniform1i(valueId, value);
}

void
Shader::setMat4(const char *name, const glm::mat4 matrix) const
{
    unsigned int matrixId = glGetUniformLocation(shaderProgramId, name);
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, glm::value_ptr(matrix));
}

void
Shader::use()
{
    glUseProgram(shaderProgramId);
}
