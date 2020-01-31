#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texCoord;

layout (location = 4) in ivec4 BoneIDs;
layout (location = 5) in vec4 Weights;


out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out mat4 modelview;
void main()
{
	FragPos = vec3(model * vec4(position, 1.0));
    TexCoords = texCoord;
	gl_Position = projection * view * model *vec4(position, 1.0);
	modelview=model;
}