#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
out vec2 sce;
uniform sampler2D screenTexture;

void main()
{
    vec4 col = vec4(vec3(texture(screenTexture, TexCoords)), 1.0);
    FragColor = col;
} 