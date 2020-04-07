#ifndef __PQEMODEL_H__
#define __PQEMODEL_H__

#include "PQE\PurpleDreamElf.h"
#include "fbxsdk.h"
#include<algorithm>
#include<functional>

namespace PQE
{

	class PQE_MODEL;
	class PQE_NODE;
	class PQE_MESH;
	class PQE_MATERIAL;
	class PQE_TEXTURE;
	class PQE_MATRIX;
	class PQE_SHAPE;
	class AABB;

	class PQEExport;
	class PQEImport;


	enum PQE_MODEL_TYPE
	{
		PQE_MODEL_FBX,
		PQE_MODEL_ASE,
		PQE_MODEL_OTHER
	};


	class PQE_MODEL
	{
	public:
		PQE_MODEL();
		void SetMatrix(PQE_MATRIX *matrix, glm::mat4 *mat);
		void SetMatrix(PQE_NODE *node, glm::mat4 *mat);
		void SetMatrix(std::string name, glm::mat4 *mat);
		void GenSpesicalID();
		void GenTextureID();
		void ComputeBoneMatrix(PQE_NODE *node, glm::mat4 parent, glm::mat4 *data);
		PQE_NODE *FindNode(std::string name);
		PQE_MATRIX *FindMatrix(std::string name);
		PQE_MODEL *operator=(PQEImport& mImport);
		~PQE_MODEL();
	public:
		unsigned int        		nodeNum;		//节点数量
		unsigned int				meshNum;		//网格数量
		unsigned int				shapeNum;		//网格数量
		unsigned int        		matrixNum;		//矩阵数量
		unsigned int				textureNum;		//纹理数量
		unsigned int				materialNum;	//材质数量
		std::vector<PQE_NODE>		mNode;			//结点
		std::vector<PQE_MESH>		mMesh;			//网络
		std::vector<PQE_SHAPE>		mShape;			//变形顶点
		std::vector<PQE_MATRIX>		mMatrix;		//矩阵
		std::vector<PQE_TEXTURE>	mTexture;		//纹理
		std::vector<PQE_MATERIAL>	mMaterial;		//材质
	};

	class PQE_NODE
	{
	public:
		enum PQE_NODE_TYPE
		{
			PQE_NODE_BONE,
			PQE_NODE_MESH,
			PQE_NODE_MESH_SHAPE,
			PQE_NODE_MESH_ROOT
		};
	public:
		PQE_NODE();
		~PQE_NODE();

	public:
		PQE_NODE_TYPE				mType;			//节点类型
		char						mName[255];		//节点名称
		long 						mSelfId;
		long						mParentId;
		unsigned int				childNum;		//子节点数量
		unsigned int				meshNum;		//网络数量
		unsigned int				shapeNum;		//变形器数量
		unsigned int				vertexNum;		//顶点数量
		unsigned int       			 mMatrixIndex;	//矩阵索引
		unsigned int				shapeVertexNum;	//变形器所有顶点
		unsigned int				spesicalId[5];
		std::vector<long>			mChildId;
		std::vector<unsigned int>	mMeshIndex;		//网格
		std::vector<unsigned int>	mShapeIndex;	//变形器
		std::vector<glm::int4>		mBoneIndex;		//骨骼索引
		std::vector<glm::vec4>		mBoneWeight;	//权重
		std::vector<glm::vec4>		mPosition;		//顶点
		std::vector<glm::vec3>		mNormal;		//法线
		std::vector<glm::vec2>		mCoord;			//纹理坐标
		AABB						*aabb;			//包围盒
	};

	class PQE_MATRIX
	{
	public:
		PQE_MATRIX();
		~PQE_MATRIX();

	public:
		glm::mat4			mSelf;			//节点初始矩阵
		glm::mat4			mFinsh;			//节点相对矩阵
		glm::mat4			mOffset;		//节点偏移矩阵
	};

	class PQE_MESH
	{
	public:
		enum PQE_MESH_TYPE
		{
			PQE_MESH_ORDINARY,
			PQE_MESH_SHAPE,
			PQE_MESH_SHAPE_INDEX
		};
	public:
		PQE_MESH();
		~PQE_MESH();

	public:
		PQE_MESH_TYPE		mType;			//网络类型
		char				mName[255];		//名称
		unsigned int		vertexNum;		//顶点数量
		unsigned int		faceNum;		//面数量
		unsigned int		boneNum;		//骨骼数量
		unsigned int		mMaterilIndex;	//材质索引
		unsigned int        mShapeWeight;	//形变权重
		unsigned int		vao;
		unsigned int		ebo;
		std::vector<unsigned int>		mFace;			//面
		AABB				*aabb;			//aabb包围盒
	};

	class PQE_SHAPE
	{
	public:
		PQE_SHAPE();
		~PQE_SHAPE();
	public:
		char					mName[255];		//名称
		float					weight;			//权重
		unsigned int			vertexNum;		//顶点数量
		std::vector<glm::vec4>	mPosition;		//变形器顶点
	};

	class PQE_MATERIAL
	{
	public:
		PQE_MATERIAL();
		~PQE_MATERIAL();

	public:
		unsigned int				materialNum;	//材质数量
		unsigned int				textureNum;		//纹理数量
		std::vector<unsigned int>	mTextureIndex;	//纹理索引
	};
	
	class PQE_TEXTURE
	{
	public:
		enum PQE_TEXTURE_TYPE
		{

		};
	public:
		PQE_TEXTURE();
		PQE_TEXTURE(PQE_TEXTURE_TYPE type, std::string path);
		~PQE_TEXTURE();

	public:
		unsigned int		mId;			//纹理id
		PQE_TEXTURE_TYPE	mType;			//纹理类型
		char				mPath[255];		//纹理路径
	};

	

}


#endif