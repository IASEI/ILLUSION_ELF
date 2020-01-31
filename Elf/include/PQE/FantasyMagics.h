#ifndef __FANTASYMAGIC_H__
#define __FANTASYMAGIC_H__

#include "fantastic.h"
#include "PurpleDreamElf.h"

namespace FM
{

	class ModelInput_PDE
	{
	public:
		int ii;
		ModelInput_PDE();

		~ModelInput_PDE();

		//公共导入函数
	public:
		//导入模型信息
		bool importFile(const TCHAR *filename);

		bool importFile(const char *filename);

	private:
		//导入信息分辨1
		void importChoose(const char *channge, PDE::model_m_pde *models, FILE *fp);

		//导入信息分辨2
		void importChoose(const char *channge, FILE *fp);

		//导入总的数据
		void importModelRoot(FILE *fp);

		//导入单个模型信息（中转站）
		void importModel(PDE::root_m_pde *modelRoot, FILE *fp);

		//导入单个模型头信息
		void importModelName(PDE::model_m_pde *models, FILE *fp);

		//导入单个模型全部信息
		void importModelInformation(PDE::model_m_pde *models, FILE *fp);

		//导入通道
		void importMapChannel(PDE::model_m_pde *models, FILE *fp);

		//导入顶点
		void importVertex(PDE::model_m_pde *models, FILE *fp);

		//导入面的信息
		void importMesh(PDE::model_m_pde *models, FILE *fp);

		//导入灯光
		void importLight(PDE::model_m_pde *models, FILE *fp);

		//导入修改器
		void importModifier(PDE::model_m_pde *models, FILE *fp);

		//导入材质
		void importMaterial(FILE *fp);

		//导入子材质
		void importMaterialChild(PDE::material_m_pde *material, FILE *fp);

		PDE::material_m_pde *getMaterial(PDE::root_m_pde modelRoots, const char *materialName);

		//PDE::material_m_pde *getModifier(PDE::model_m_pde model, const char *modelName);

		//为纹理分配id
		void genTexxtureID();

		//为纹理分配id续
		void genTextureIDChild(PDE::material_m_pde *material);

		//生成面法线
		void generationMeshNormal(PDE::model_m_pde *model);

		//全部生成面法线
		void generationAllMeshNormal();
	public:

		//设置使用Freeimage，还是bit生成纹理
		void setGenCoordFreeimageOrBit(bool is);

	public:
		PDE::model_m_pde *updateJojintMatrix(PDE::model_m_pde *model);

		void updateVertex(PDE::model_m_pde *model);

		//从骨骼矩阵生成姿势矩阵
		void updateBoneToPostMatrix(PDE::model_m_pde *model, bool isState);

		void updateBoneState(PDE::model_m_pde *model, glm::mat4x4 Rot);

		void initBoneMatrix();
	public:
		void drawUseShader(PDE::model_m_pde *model, Shader *shader);

		void drawAnimation(glm::mat4x4 mat, PDE::model_m_pde *model);

		/** 初始化多重纹理 */
		bool IsExtensionSupported(char* szTargetExtension);
		
		bool initMultiTexture();

		void updateModelToWord(PDE::model_m_pde *model);

		void updateModelToModel(PDE::model_m_pde *model);

		//void updateModelToModel(PDE::model_m_pde *model,glm::mat4x4 mat);

		void updateModelNewJointMatrix(PDE::model_m_pde *model, bool isUpateShowMatrix);

		void updateModelJointMatrix(PDE::model_m_pde *model, glm::mat4x4 Rot);

		void initModelJointMatrix(PDE::model_m_pde *model);

		void initModelJointMatrixUntilTran(PDE::model_m_pde *model);

		void updateBoneId();

		void genVaoVboEbo(PDE::model_m_pde *model);

	

		PDE::root_m_pde *getModelRoot();

		bool BOOLS;
	private:
		FILE *fp;

		bool textureOrNormalEBO;//true 使用纹理索引的作为主要索引，false使用法向量作为主要索引
		bool isGenCoordFreeImageOrBit;//true 使用Freiamge ，false 使用Bit创建纹理

		char buffer[255];
		char change[255];
		PDE::root_m_pde modelRoot;
		float j;


	};

}
#endif