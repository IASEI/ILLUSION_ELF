#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texCoord;

out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(position, 1.0));
    TexCoords = texCoord;
	vec4 pos = projection * view  *vec4(position, 1.0);
	gl_Position = pos.xyww;
}