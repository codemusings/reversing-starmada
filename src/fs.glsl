#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoords;

uniform sampler2D texData;

void main()
{
    // FragColor = vec4(color, 1.0f);
    //FragColor = texture(texData, texCoords) * vec4(color, 1.0f);
    FragColor = texture(texData, texCoords);
};
