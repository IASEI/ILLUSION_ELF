#ifndef __PURPLEDREAMELF_H__
#define __PURPLEDREAMELF_H__

#include "fantastic.h"
#include "PQE\shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>
#include <map>
#include <math.h>
#include <stdint.h>
#include "QdeRender.h"

#include<algorithm>

#define ZERO_MEM(a) memset(a,0,sizeof(a))
#define NUM_BONES_PER_VEREX 4
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0])) 
#define INVALID_MATERIAL 0xFFFFFFFF
#define SNPRINTF _snprintf_s
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))

/*//========================================//

1.m是model的缩写
2.PDE是PurpleDreamElf的缩写

*///========================================//
namespace PDE
{
	enum lightType { SUNTYPE, SPOTLIGHTTYPE, PARALLELTYPE };
	enum springDistanceType { OneDistance, TwoDistance, HypotenuseDistance };

	struct bone
	{
		unsigned int boneIndex[4];
		float weight[4];

		void reset()
		{
			memset(boneIndex, 0, sizeof(boneIndex));
			memset(weight, 0, sizeof(weight));
		}

		void add(unsigned int id, float mWeight)
		{
			for (unsigned int i = 0; i <4; i++) {
				if (weight[i] == 0.0) {
					boneIndex[i] = id;
					weight[i] = mWeight;
					return;
				}
			}
		}
	};

	struct material
	{
		int path1;
		int path2;
		int path3;
		int path4;
	};

	struct texturePath
	{
		char path[255];
		int id;
		texturePath()
		{
			path[0] = '\0';
			id = 0;
		}
	};

	struct vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 coord;
		vertex(glm::vec3 mPosition, glm::vec3 mNormal, glm::vec2 mCoord)
		{
			position = mPosition;
			normal = mNormal;
			coord = mCoord;

		}
		vertex()
		{

		}
	};

	struct mesh
	{
		int materialIndex;
		int materixIndex;
		std::vector<unsigned int> vertexIndex;
		mesh()
		{
			materialIndex = -1;
			materixIndex = -1;
		}
	};

	struct dateNum
	{
		unsigned int boneMapSize;
		unsigned int materialSize;
		unsigned int materixSize;
		unsigned int vertexSize;
		unsigned int meshSize;
		unsigned int meshIndexSize;
		unsigned int boneSize;
		unsigned int texturePathSize;

		int boneMapNum;
		int materialNum;
		int matrixNum;
		int vertexNum;
		int meshNum;
		int texturePathNum;
		int boneNum;
		int mMatrixNum;

	};

	struct matrix
	{
		int parent;
		glm::mat4 finish;
		glm::mat4 self;
		glm::mat4 offset;
		std::vector<int> child;
		matrix(glm::mat4 mOffset)
		{
			parent = 0;
			offset = mOffset;
		}
		matrix()
		{
			parent = 0;
		}

	};

	struct root
	{
		dateNum mDateNum;
		glm::mat4 globalInverse;
		std::vector<int> meshIndex;//每个mesh的顶点索引数量
		std::vector<int> mMatrixNum;//每个mesh的顶点索引数量
		map<string, GLuint> boneMapping;
		std::vector<material> mMaterial;
		std::vector<matrix> mMatrix;//所有矩阵
		std::vector<vertex> mVertex;//所有顶点
		std::vector<mesh> mMesh;//所有的面（一纹理划分）
		std::vector<texturePath> mTexturePath;
		std::vector<bone> mBone;

	};

	struct otherWindowInformation
	{
		HWND hwnd;
		int width;
		int height;
		DWORD *size;
		unsigned int TexId;
		otherWindowInformation()
		{
			hwnd = 0;
			width = 0;
			height = 0;
			size = NULL;
			TexId = 0;
		}
	};

	struct PIDInformation
	{
		DWORD pid;
		string name;
		vector<otherWindowInformation> hwndInformation;
		PIDInformation(DWORD id, string mName)
		{
			pid = id;
			name = mName;
		}
		PIDInformation()
		{

		}
	};

	struct SpecialID
	{
		unsigned int id;
		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;
		unsigned int texture;
		unsigned int hdrTexture[2];
		unsigned int fbo;
		unsigned int pingpongFbo[2];
		unsigned int rbo;
		unsigned int ubo;
		unsigned int uboIndex;
		SpecialID()
		{
			id = vao = vbo = ebo = texture = fbo = rbo = ubo = uboIndex = 0;
		}
	};

	struct light_pde
	{
		lightType type;
		glm::vec3 pos;
		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;
	};

	//弹簧
	struct spring
	{
		unsigned int frontPointIndex;//前端
		unsigned int backPointIndex;//后端
		unsigned int initialLength;//初始长度
		springDistanceType type;
		unsigned int modulusElasticity;//弹性系数
		spring()
		{
			frontPointIndex = backPointIndex = -1;
			initialLength = 0;
			modulusElasticity = 0.8;
		}
		spring(unsigned int mfrontPointIndex, unsigned int mbackPointIndex, unsigned int minitialLength)
		{
			frontPointIndex = mfrontPointIndex;
			backPointIndex = mbackPointIndex;
			initialLength = minitialLength;
			modulusElasticity = 0.8;
		}


	};
	//弹簧布料顶点

	struct  springVertex
	{
		unsigned int original;
		vector<unsigned int> singleVertex;
		vector<unsigned int> doubleVertex;
		vector<spring>pointSpring;
		vector<spring>pointSprings;

		void addSingleVertex(unsigned int index)
		{
			for (int i = 0;i < singleVertex.size();i++)
			{
				if (index == singleVertex[i])
					return;
			}

			singleVertex.push_back(index);
		}

		void addDoubleVertex(unsigned int index)
		{
			for (int i = 0;i < doubleVertex.size();i++)
			{
				if (index == doubleVertex[i])
					return;
			}

			doubleVertex.push_back(index);
		}

		void addPointSpring(spring mspring)
		{
			pointSpring.push_back(mspring);
		}

		bool one(unsigned int m)
		{
			for (int i = 0;i < pointSpring.size();i++)
			{
				if (pointSpring[i].backPointIndex == m)
					return false;
			}
			return true;
		}

		bool two(unsigned int m, unsigned int m1)
		{
			for (int i = 0;i < pointSpring.size();i++)
			{
				if (((pointSpring[i].backPointIndex == m) && (pointSpring[i].frontPointIndex == m1)) || ((pointSpring[i].backPointIndex == m1) && (pointSpring[i].frontPointIndex == m)))
					return false;
			}
			return true;
		}
	};

	struct ExePath
	{
		std::string name;
		std::string path;

		ExePath(std::string mName, std::string mPath)
		{
			name = mName;
			path = mPath;
		}

		ExePath()
		{

		}
	};

	struct model
	{
		root mRoot;
		SpecialID rootId;
		SpecialID boneId;
		std::vector<SpecialID> meshId;
	};

	struct frameBuffer
	{
		SpecialID frame;
		SpecialID MSAAframe;
		glm::vec4 rgb;
	};

	struct shadow
	{
		SpecialID shadowDepthMapFrame;
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane, far_plane;
		glm::vec3 lightPos;
	};

	struct SystemVariable
	{
		int screenWidth;
		int screenHeight;
		int shadowWidth;
		int shadowHeight;

		shader shadow;
		shader renderBasic;
		shader frameScreen;

		glm::vec4 clearColor;
		std::vector<model> mModels;
		std::vector<model> mFrameBuffer;
	};

	
	extern bool	arbMultisampleSupported;
	extern int arbMultisampleFormat;

	bool initMultisample(HINSTANCE hInstance, HWND hWnd, PIXELFORMATDESCRIPTOR pfd);

	char *tcharTochar(const TCHAR *str, char *cstr);
	TCHAR *charToTchar(const char *str, TCHAR *Tstr);
	char *tcharTochar(const TCHAR *str);
	TCHAR *charToTchar(const char *str);

	int createTextures(const char *path);
	int createTextures(const TCHAR *path);
	int  createBitTextures(const char *path);
	int  createBitTextures(const TCHAR *path);
	unsigned int TextureFromFile(const char *path);
	unsigned int loadCubemap(vector<std::string> faces);

	SpecialID generateSpecialID(unsigned long int indexsize, unsigned long int datasieze, const unsigned int *index, const float *data);
	SpecialID generateSpecialIDReturnVAOVBO(unsigned long int datasieze,float *data);
	glm::mat4x4 aiMatrix4x4ToMat4x4(const aiMatrix4x4& AssimpMatrix);
	void GaussianBlur(unsigned char* img, unsigned int width, unsigned int height, unsigned int channels, unsigned int radius);//高斯模糊算法

	long long GetCurrentTimeMillis();
	float ComputeAngleUseVector(glm::vec3 n1,glm::vec3 n2);
	

}
#endif 