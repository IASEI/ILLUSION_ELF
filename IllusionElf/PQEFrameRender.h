#ifndef __PQEFRAMERENDER_H__
#define __PQEFRAMERENDER_H__

#include "PQEModel.h"
#include "PQEModelRender.h"
#include "shader.h"
namespace PQE
{

	class FrameAttribute
	{
	public:
		FrameAttribute(PQEModelRender *mModelRender, shader *mShader);
		~FrameAttribute();
	public:
		PQEModelRender *mModelRender;
		shader *mShader;

		unsigned int mFrameId;
		unsigned int mTextureId;
		unsigned int mRenderBufferId;

	};

	class PQEFrameRender
	{
	public:
		
	public:
		PQEFrameRender();
		
		void CreateFrame();
		void StartRender();
		void EndRender();

		unsigned int Render();

		unsigned int operator>>(FrameAttribute mRenderShader);
		~PQEFrameRender();

	private:
		unsigned int mViewWidth, mViewHeight;
		unsigned int mShadowSize;
		FrameAttribute *mBuffer;
		std::vector<FrameAttribute> mModelRenderShader;
	};
}

#endif

