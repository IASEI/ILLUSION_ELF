#include "PQEFrameRender.h"
namespace PQE
{
	FrameAttribute::FrameAttribute(PQEModelRender *ModelRender, shader *Shader) :mModelRender(mModelRender), mShader(mShader)
	{
	
	}

	FrameAttribute::~FrameAttribute()
	{
		
	}

	//------------------------------------------------------------------
	PQEFrameRender::PQEFrameRender()
	{
	
	}

	void PQEFrameRender::CreateFrame()
	{
		glGenFramebuffers(1, &mBuffer->mFrameId);
		glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFrameId);

		//创建颜色模板
		glGenTextures(1, &mBuffer->mTextureId);
		glBindTexture(GL_TEXTURE_2D, mBuffer->mTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mViewWidth, mViewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBuffer->mTextureId, 0);

		//创建渲染缓冲模板
		glBindRenderbuffer(GL_RENDERBUFFER, mBuffer->mRenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mViewWidth, mViewHeight); 
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mBuffer->mRenderBufferId);

		//检查是否成功创建帧
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		mBuffer = NULL;
	}

	void PQEFrameRender::StartRender()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFrameId);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void PQEFrameRender::EndRender()
	{
		mBuffer = NULL;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	unsigned int PQEFrameRender::Render()
	{
		for (auto render : mModelRenderShader)
			render.mModelRender->Render(render.mShader);
		return mModelRenderShader[mModelRenderShader.size()-1].mTextureId;
	}

	PQEFrameRender::~PQEFrameRender()
	{
	
	}
}