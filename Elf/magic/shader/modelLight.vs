#version 430 core

layout (location = 0) in vec4  position;
layout (location = 1) in vec3  Normal;
layout (location = 2) in vec2  texCoord;

layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4  Weights;
layout (location = 5) in uint  ShapeVertexIndex;

#define SystemActionShadow		mOrdinary[0][0]			
#define SystemActionBone		mOrdinary[0][1]			
#define SystemActionShape		mOrdinary[0][2]			
#define SystemLightLocation		mSunlight[0]			
#define SystemLightDirection	mSunlight[1]			
#define SystemLightAmbient		mEnvironment[0]			
#define SystemLightDiffuse		mEnvironment[1]
#define SystemLightSpecular		mEnvironment[2]
//系统属性
layout(std430,binding = 0) buffer mSystem 
{
	mat4 mOrdinary;
	mat4 mSunlight;
	mat4 mEnvironment;
	mat4 mSpace;				//空间大小比例矩阵，以miku模型为标准
    mat4 mLightProjection;		//光照空间
	mat4 mLightView;			//光照空间
	
};

layout(std430,binding = 1,row_major) buffer ShapeWeight
{
    float shapeWeight[];
};

layout(std430,binding = 2,row_major) buffer Matrix
{
    mat4 BM[];
};

layout(std430,binding = 3,row_major) buffer ShapeVertex 
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

out vec3 mLightLocation;
out vec3 mLightDirection;	
out vec3 mLightAmbient;	
out vec3 mLightDiffuse;
out vec3 mLightSpecular;

uniform mat4		model;
uniform mat4		view;
uniform mat4		projection;
uniform mat4		lightSpaceMatrix;
uniform ShapeData	shape_data;

void main()
{
	vec4 pos;
	mat4 boneTransform=mat4(1.0f);
	vec3 posCopy=position.xyz;
	if(bool(SystemActionBone))
	{
		boneTransform=BM[BoneIDs[0]]*Weights[0];
		boneTransform+=BM[BoneIDs[1]]*Weights[1];
		boneTransform+=BM[BoneIDs[2]]*Weights[2];
		boneTransform+=BM[BoneIDs[3]]*Weights[3];
	}
	if(bool(SystemActionShape))
	{
		for(int i=0;i<shape_data.weightCount;i++)
		{
			vec4 influence=(data[i*shape_data.vertexCount+int(position.w)]-position)*shapeWeight[i];
		 	posCopy+=influence.xyz;
		}
	}

	mLightLocation=SystemLightLocation.xyz;
	mLightDirection=SystemLightDirection.xyz;	
	mLightAmbient=SystemLightAmbient.xyz;	
	mLightDiffuse=SystemLightDiffuse.xyz;
	mLightSpecular=SystemLightSpecular.xyz;

    mTexCoords = texCoord;
	pos=boneTransform*vec4(posCopy,1.0f);
    mNormals = mat3(transpose(inverse(model))) * Normal;  
	mFragPosLightSpace = mSpace*mLightProjection*mLightView* vec4(vec3(model * pos), 1.0);
	gl_Position =mSpace* projection  * view * model * pos;							//输出顶点
}