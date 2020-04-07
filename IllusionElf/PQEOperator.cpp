#include "PQEOperator.h"
namespace PQE
{
	PQE_MODEL *PQE_MODEL::operator=(PQEImport& mImport)
	{
		return mImport.GetModel();
	}

	PQEModelRender *PQEModelRender::operator>>(PQEImport *mImport)
	{
		mImport->GetModel()->GenSpesicalID();
		mImport->GetModel()->GenTextureID();
		this->mModel.push_back(mImport->GetModel());
		return this;
	}

	PQEModelRender *PQEModelRender::operator>>(PQE_MODEL *mModel)
	{
		mModel->GenSpesicalID();
		mModel->GenTextureID();
		this->mModel.push_back(mModel);
		return this;
	}

	unsigned int PQEFrameRender::operator>>(FrameAttribute mRenderShader)
	{
		this->mModelRenderShader.push_back(mRenderShader);
	}
}
