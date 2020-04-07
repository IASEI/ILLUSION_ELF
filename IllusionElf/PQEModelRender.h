#ifndef __PQEMODELRENDER_H__
#define __PQEMODELRENDER_H__

#include "PQEModel.h"

namespace PQE
{
	class PQEModelRender
	{
	public:
		PQEModelRender();
		
		virtual void Render(shader *mshader = NULL);
		void SetBoneSSAOId(unsigned int id);
		void SetShapeSSAOId(unsigned int id);
		void SetShapeWeightSSAOId(unsigned int id);
		~PQEModelRender();

	public:
		PQEModelRender *operator>>(PQEImport *mImport);
		PQEModelRender *operator>>(PQE_MODEL *mModel);
	private:
		std::vector<PQE_MODEL *> mModel;
		unsigned int *bone_ssao;
		unsigned int *shape_ssao;
		unsigned int *shape_weight_ssao;
	};
}

#endif // !__PQERENDER_H__


