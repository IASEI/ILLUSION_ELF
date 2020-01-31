#version 430 core

layout (location = 0) in vec3  position;
layout (location = 1) in vec3  Normal;
layout (location = 2) in vec2  texCoord;

layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4  Weights;
layout (location = 5) in uint  ShapeVertexIndex;

layout(std430,binding = 0,row_major) buffer Matrix
{
    float shapeWeight[];
};

layout(std430,binding = 1,row_major) buffer Matrix
{
    mat4 BM[];
};

layout(std430,binding = 2,row_major) buffer ShapeVertex 
{
    vec3 data[];
};

struct ShapeData
{
	int shapeNodeVertexCount;
	int vertexCout;
	int weightCout;
};

out vec2 mTexCoords;
out vec3 mFragPos;
out vec3 mNormals;
out vec4 mFragPosLightSpace;

uniform mat4		model;
uniform mat4		view;
uniform mat4		projection;
uniform bool		animat;
uniform mat4		lightSpaceMatrix;
uniform ShapeData	shape_data;

void main()
{
	int shapeVertexBegin=0,shapeVertexEnd=0;
	vec4 pos;
	if(animat)
	{
		mat4 boneTransform=BM[BoneIDs[0]]*Weights[0];
		boneTransform+=BM[BoneIDs[1]]*Weights[1];
		boneTransform+=BM[BoneIDs[2]]*Weights[2];
		boneTransform+=BM[BoneIDs[3]]*Weights[3];
		for(int i=0;i<shape_data.weightCout;i++)
		{
			vec3 influence=(data[shape_data.shapeNodeVertexCout+i*shape_data.vertexCout+ShapeVertexIndex]-position)*shapeWeight[i];
		 	mpos+=influence;
		}
		pos=boneTransform*vec4(position,1.0f);
	}
	else
	{
		pos=vec4(position,1.0f);
	}
    mNormals = mat3(transpose(inverse(model))) * Normal;  
    mTexCoords = texCoord;
	mFragPosLightSpace = lightSpaceMatrix * vec4(vec3(model * pos), 1.0);
	gl_Position = projection  * view * model * pos;							//Êä³ö¶¥µã
}