#include "PQEFrameRender.h"
namespace PQE
{
	FrameAttribute::FrameAttribute(PQEModelRender *ModelRender, shader *Shader, unsigned int ViewWidth, unsigned int ViewHeight)
		:mModelRender(ModelRender), mShader(Shader),mType(PQE_FRAME_ORDINARY),mViewWidth(ViewWidth),mViewHeight(ViewHeight)
	{
		
	}

	FrameAttribute::FrameAttribute(PQEModelRender *ModelRender, shader *Shader, unsigned int ViewWidth, unsigned int ViewHeight, unsigned int ShadowWidth)
		: mModelRender(ModelRender), mShader(Shader), mType(PQE_FRAME_ORDINARY), mViewWidth(ViewWidth), mViewHeight(ViewHeight),mShadowWidth(ShadowWidth)
	{

	}

	FrameAttribute::FrameAttribute(PQEModelRender *ModelRender, shader *Shader, unsigned int ViewWidth, unsigned int ViewHeight, unsigned int ShadowWidth, PQE_FRAME_TYPE Type)
		: mModelRender(ModelRender), mShader(Shader),mViewWidth(ViewWidth), mViewHeight(ViewHeight), mShadowWidth(ShadowWidth), mType(Type)
	{

	}

	FrameAttribute::~FrameAttribute()
	{
		
	}

	//------------------------------------------------------------------
	PQEFrameRender::PQEFrameRender()
	{
	
	}

	bool PQEFrameRender::CreateFrame()
	{
		switch (mBuffer->mType)
		{
		case PQE::PQE_FRAME_ORDINARY:
			this->CreateFrameOrdinary();
			break;
		case PQE::PQE_FRAME_SHADOW:
			this->CreateFrameShadow();
			break;
		case PQE::PQE_FRAME_MSAA:
			this->CreateFrameMSAA();
			break;
		default:
			break;
		}
		return true;
	}
	void PQEFrameRender::StartRender()
	{
		switch (mBuffer->mType)
		{
		case PQE::PQE_FRAME_ORDINARY:
			this->StartRenderOrdinary();
			break;
		case PQE::PQE_FRAME_SHADOW:
			this->StartRenderShadow();
			break;
		case PQE::PQE_FRAME_MSAA:
			this->StartRenderMSAA();
			break;
		default:
			break;
		}
	}
	void PQEFrameRender::EndRender()
	{
		switch (mBuffer->mType)
		{
		case PQE::PQE_FRAME_ORDINARY:
			this->EndRenderOrdinary();
			break;
		case PQE::PQE_FRAME_SHADOW:
			this->EndRenderShadow();
			break;
		case PQE::PQE_FRAME_MSAA:
			this->EndRenderMSAA();
			break;
		default:
			break;
		}
	}

	void PQEFrameRender::CreateFrameOrdinary()
	{
		glGenFramebuffers(1, &mBuffer->mFrameId);
		glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFrameId);

		//创建颜色模板
		glGenTextures(1, &mBuffer->mTextureId);
		glBindTexture(GL_TEXTURE_2D, mBuffer->mTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mBuffer->mViewWidth, mBuffer->mViewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBuffer->mTextureId, 0);

		//创建渲染缓冲模板
		glGenRenderbuffers(1, &mBuffer->mRenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, mBuffer->mRenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mBuffer->mViewWidth, mBuffer->mViewHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mBuffer->mRenderBufferId);

		//检查是否成功创建帧
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	void PQEFrameRender::CreateFrameShadow()
	{
		glGenFramebuffers(1, &mBuffer->mFrameId);
		glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFrameId);
		
		glGenTextures(1, &mBuffer->mTextureId);
		glBindTexture(GL_TEXTURE_2D, mBuffer->mTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mBuffer->mShadowWidth, mBuffer->mShadowWidth, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mBuffer->mTextureId, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	void PQEFrameRender::CreateFrameMSAA()
	{
		glGenFramebuffers(1, &mBuffer->mFrameId);
		glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFrameId);
		
		glGenTextures(1, &mBuffer->mTextureId);
		glBindTexture(GL_TEXTURE_2D, mBuffer->mTextureId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, mBuffer->mViewWidth, mBuffer->mViewHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mBuffer->mTextureId, 0);
		
		glGenRenderbuffers(1, &mBuffer->mRenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, mBuffer->mRenderBufferId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER,4, GL_DEPTH24_STENCIL8, mBuffer->mViewWidth, mBuffer->mViewHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,mBuffer->mRenderBufferId); 
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
			
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void PQEFrameRender::StartRenderOrdinary()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFrameId);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void PQEFrameRender::StartRenderShadow()
	{
		glViewport(0, 0, mBuffer->mShadowWidth, mBuffer->mShadowWidth);
		glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFrameId);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void PQEFrameRender::StartRenderMSAA()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFrameId);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void PQEFrameRender::EndRenderOrdinary()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void PQEFrameRender::EndRenderShadow()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, mBuffer->mViewWidth, mBuffer->mViewHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void PQEFrameRender::EndRenderMSAA()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	unsigned int PQEFrameRender::Render()
	{
		for (auto render : mModelRenderShader) 
		{
			this->mBuffer = render;
			if (!this->mBuffer->success) continue;
			this->StartRender();
			this->mBuffer->mModelRender->Render(this->mBuffer->mShader);
			this->EndRender();
		}
		this->mBuffer = NULL;
		return mModelRenderShader[mModelRenderShader.size()-1]->mTextureId;
	}

	PQEFrameRender::~PQEFrameRender()
	{
	
	}
}