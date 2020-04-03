#ifndef __PQERENDER_H__
#define __PQERENDER_H__

#include "PQEModel.h"

namespace PQE
{
	class PQERender
	{
	public:
		PQERender();
		void GenSpesicalID();
		void GenTextureID();
		virtual void Render(shader *mshader = NULL);
		void SetMatrix(PQE_MATRIX *matrix, glm::mat4 *mat);
		void SetMatrix(PQE_NODE *node, glm::mat4 *mat);
		void SetMatrix(std::string name, glm::mat4 *mat);
		void SetBoneSSAOId(unsigned int id);
		void SetShapeSSAOId(unsigned int id);
		void SetShapeWeightSSAOId(unsigned int id);
		void ComputeBoneMatrix(PQE_NODE *node, glm::mat4 parent, glm::mat4 *data);
		PQE_NODE *FindNode(std::string name);//使用lambda表达式
		PQE_MATRIX *FindMatrix(std::string name);
		~PQERender();
	private:
		PQE_MODEL *mModel;
		unsigned int *bone_ssao;
		unsigned int *shape_ssao;
		unsigned int *shape_weight_ssao;
	};
}

#endif // !__PQERENDER_H__


