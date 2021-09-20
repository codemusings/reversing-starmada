#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 colorInput;
layout (location = 2) in vec2 texCoordsInput;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 color;
out vec2 texCoords;

void main()
{
//    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = proj * view * model * vec4(vertex, 1.0);
    color = colorInput;
    texCoords = texCoordsInput;
};
