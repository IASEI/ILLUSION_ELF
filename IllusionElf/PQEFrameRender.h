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
		FrameAttribute(PQEModelRender *ModelRender, shader *Shader, unsigned int ViewWidth, unsigned int ViewHeight);
		FrameAttribute(PQEModelRender *ModelRender, shader *Shader, unsigned int ViewWidth, unsigned int ViewHeight, unsigned int ShadowWidth);
		FrameAttribute(PQEModelRender *ModelRender, shader *Shader, unsigned int ViewWidth, unsigned int ViewHeight, unsigned int ShadowWidth, PQE_FRAME_TYPE Type);
		~FrameAttribute();
	public:
		PQE_FRAME_TYPE mType;
		PQEModelRender *mModelRender;
		shader *mShader;

		bool success;

		unsigned int mFrameId;
		unsigned int mTextureId;
		unsigned int mRenderBufferId;
		unsigned int mViewWidth, mViewHeight;
		unsigned int mShadowWidth;
	};

	class PQEFrameRender
	{
	public:
		
	public:
		PQEFrameRender();
		
		bool CreateFrame();
		void StartRender();
		void EndRender();

		void CreateFrameOrdinary();
		void CreateFrameShadow();
		void CreateFrameMSAA();

		void StartRenderOrdinary();
		void StartRenderShadow();
		void StartRenderMSAA();

		void EndRenderOrdinary();
		void EndRenderShadow();
		void EndRenderMSAA();

		unsigned int Render();

		unsigned int operator>>(FrameAttribute *mRenderShader);
		~PQEFrameRender();

	private:
		FrameAttribute *mBuffer;
		std::vector<FrameAttribute*> mModelRenderShader;
	};
}

#endif

