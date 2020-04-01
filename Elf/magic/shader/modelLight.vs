#version 430 core

layout (location = 0) in vec4  position;
layout (location = 1) in vec3  Normal;
layout (location = 2) in vec2  texCoord;

layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4  Weights;
layout (location = 5) in uint  ShapeVertexIndex;

layout(std430,binding = 0,row_major) buffer ShapeWeight
{
    float shapeWeight[];
};

layout(std430,binding = 1,row_major) buffer Matrix
{
    mat4 BM[];
};

layout(std430,binding = 2,row_major) buffer ShapeVertex 
{
    vec4 data[];
};

struct ShapeData
{
	int shapeNodeVertexCount;
	int vertexCount;
	int weightCount;
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
	if(true)
	{
		vec3 pos1=position.xyz;
		mat4 boneTransform=BM[BoneIDs[0]]*Weights[0];
		boneTransform+=BM[BoneIDs[1]]*Weights[1];
		boneTransform+=BM[BoneIDs[2]]*Weights[2];
		boneTransform+=BM[BoneIDs[3]]*Weights[3];
		for(int i=0;i<shape_data.weightCount;i++)
		{
			vec4 influence=(data[i*shape_data.vertexCount+int(position.w)]-position)*shapeWeight[i];
		 	pos1+=influence.xyz;
		}
		pos=boneTransform*vec4(pos1,1.0f);
		//pos=vec4(pos1,1.0f);
	}
	else
	{
		vec3 pos1=position.xyz;
		pos1=data[int(position.w)].xyz;
		pos=vec4(pos1,1.0f);
	}
    mNormals = mat3(transpose(inverse(model))) * Normal;  
    mTexCoords = texCoord;
	mFragPosLightSpace = lightSpaceMatrix * vec4(vec3(model * pos), 1.0);
	gl_Position = projection  * view * model * pos;							//Êä³ö¶¥µã
}