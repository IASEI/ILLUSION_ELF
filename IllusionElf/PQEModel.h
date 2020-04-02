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

	class PQE_MODEL
	{
	public:
		PQE_MODEL();
		~PQE_MODEL();
	public:
		unsigned int		mRootNodeId;
		unsigned int		meshNum;		//网格数量
		unsigned int		shapeNum;		//网格数量
		unsigned int		materialNum;	//材质数量
		unsigned int		textureNum;		//纹理数量
		unsigned int        matrixNum;		//矩阵数量
		unsigned int        nodeNum;		//节点数量
		std::vector<PQE_NODE>			mNode;
		std::vector < PQE_MATRIX>			mMatrix;		//矩阵
		std::vector < PQE_MESH>			mMesh;		//网络
		std::vector < PQE_MATERIAL>		mMaterial;		//材质
		std::vector < PQE_TEXTURE	>		mTexture;		//纹理
		std::vector < PQE_SHAPE	>		mShape;		//变形顶点
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
		PQE_NODE_TYPE		mType;			//节点类型
		char				mName[255];		//节点名称
		unsigned int		childNum;		//子节点数量
		unsigned int		meshNum;		//网络数量
		unsigned int		shapeNum;		//变形器数量
		unsigned int		vertexNum;		//顶点数量
		unsigned int        mMatrixIndex;	//矩阵索引
		unsigned int		shapeVertexNum;	//变形器所有顶点
		std::vector<unsigned int>	mMeshIndex;	//网格
		std::vector<unsigned int>		mShapeIndex;	//变形器
		unsigned int		spesicalId[5];
		long 				mSelfId;
		long				mParentId;
		std::vector<long>				mChildId;
		std::vector < glm::int4>			mBoneIndex;	//骨骼索引
		std::vector < glm::vec4	>		mBoneWeight;	//权重
		std::vector < glm::vec4>			mPosition;		//顶点
		std::vector < glm::vec3>			mNormal;		//法线
		std::vector < glm::vec2>			mCoord;		//纹理坐标
		std::vector < glm::vec4>			mColor;		//顶点颜色
		AABB				*aabb;			//包围盒
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
		int					*mShapeVertexIndex;
		unsigned int		vertexNum;		//顶点数量
		unsigned int		faceNum;		//面数量
		unsigned int		boneNum;		//骨骼数量
		unsigned int		mMaterilIndex;	//材质索引
		unsigned int        mShapeWeight;	//形变权重
		std::vector<unsigned int>		mFace;			//面
		unsigned int		vao;
		unsigned int		ebo;
		AABB				*aabb;			//aabb包围盒
	};
	

	class PQE_SHAPE
	{
	public:
		PQE_SHAPE();
		~PQE_SHAPE();
	public:
		char				mName[255];		//名称
		float				weight;			//权重
		unsigned int		vertexNum;		//顶点数量
		std::vector<glm::vec4>			mPosition;		//变形器顶点
	};

	class PQE_MATERIAL
	{
	public:
		PQE_MATERIAL();
		~PQE_MATERIAL();

	public:
		unsigned int		materialNum;	//材质数量
		unsigned int		textureNum;	//纹理数量
		std::vector<unsigned int>		mTextureIndex;//纹理索引
		std::vector < PQE_MATERIAL>		mMaterial;	//材质
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


	class PQEModel
	{
	public:
		PQEModel();
		PQEModel(std::string path);
		~PQEModel();

		void LoadNode(const aiScene *scene);		//读取节点
		void LoadMesh(const aiScene *scene);		//读取网格
		void LoadMaterial(const aiScene *scene);	//读取材质
		void LoadShape(std::string path, std::vector<PQE_SHAPE> &shape);							//读取变形顶点

		void LoadBoneChild(const aiScene *scen);
		void LoadNodeChild(const aiScene *scen, std::vector<PQE_MATRIX> &vec, std::vector<PQE_NODE>& pqe_node, aiNode *node, PQE_NODE *pNodeParent, unsigned int &index);

		void LoadMaterialTexture(aiMaterial *material, PQE_MATERIAL *pqematerial, std::vector<PQE_TEXTURE> &texture, unsigned int materialIndex);
		void LoadMaterialTextureChild(aiMaterial *material, PQE_MATERIAL *pqematerial,aiTextureType type, std::vector<PQE_TEXTURE> &path, std::vector<unsigned int> &textureIndex, unsigned int materialIndex);
		bool checkTextureRepeat(PQE_TEXTURE &tex,std::string path);
	
		void GenSpesicalID();
		void GenSpesicalIdChild(PQE_NODE *node);
		void GenTextureID();
		virtual void Render(shader *mshader = NULL);

		void SetMatrix(PQE_MATRIX *matrix,glm::mat4 *mat);
		void SetMatrix(PQE_NODE *node,glm::mat4 *mat);
		void SetMatrix(std::string name, glm::mat4 *mat);
		void SetBoneSSAOId(unsigned int id);
		void SetShapeSSAOId(unsigned int id);
		void SetShapeWeightSSAOId(unsigned int id);

		void ComputeBoneMatrix(PQE_NODE *node, glm::mat4 parent, glm::mat4 *data);

		PQE_NODE *FindNode(std::string name);//使用lambda表达式
		PQE_MATRIX *FindMatrix(std::string name);

		inline void InitFbxSdk(FbxManager *&pManager, FbxScene*&pScene);//初始化fbx sdk
		void LoadFbxNode(FbxNode* pNode, std::vector<PQE_SHAPE> &shape);
		inline void LoadFbxMesh(FbxNode* pNode, std::vector<PQE_SHAPE> &shape);
		inline void LoadFbxShape(FbxMesh* pMesh, PQE_NODE *pqe_node, std::vector<PQE_SHAPE> &shape);
		inline void LoadFbxShapeVertexIndex(PQE_NODE *pqe_node, std::vector<PQE_SHAPE> &shape, std::vector<glm::vec4> &vertex);

		PQE_MODEL *GetModel();
	private:
		PQE_MODEL *mModel;
		std::string mModelPath;
		unsigned int *bone_ssao;
		unsigned int *shape_ssao;
		unsigned int *shape_weight_ssao;
	};
}


