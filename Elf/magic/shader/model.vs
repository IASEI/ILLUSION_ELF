#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 texCoord;

layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

layout(std140,row_major) uniform boneMat4
{
	mat4 BM[500];
};

out vec2 TexCoords;
out vec3 FragPos;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool animat;

void main()
{
	vec4 pos;
	if(animat)
	{
		mat4 boneTransform=BM[BoneIDs[0]]*Weights[0];
		boneTransform+=BM[BoneIDs[1]]*Weights[1];
		boneTransform+=BM[BoneIDs[2]]*Weights[2];
		boneTransform+=BM[BoneIDs[3]]*Weights[3];
		pos=boneTransform*vec4(position,1.0);
	}
	else
	{
		pos=vec4(position,1.0);
	}
	
	gl_Position = projection * view * model *pos;
	TexCoords = texCoord;
}