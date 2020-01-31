#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "pde\PurpleDreamElf.h"
#include "QdeRender.h"

class FrameBuffer :public QdeRender
{
public:
	enum FRAMEATTR{ FrameOrdinary,FrameShadow,FrameMSAA };
public:
	FrameBuffer() :near_plane(0.1f), far_plane(100.0f), mShader(NULL), mlightPos(1.0f, 10.0f, 0.0f)
	{
		rootType = FRAME;
		name = "frame";
	}

	~FrameBuffer()
	{
	
	}

	//阴影帧缓存
	void initShadow(int mViewHeight = 1920, int mViewWidth = 1080, unsigned int mShadowWidth = 2048, unsigned int mShadowHeight = 2048)
	{
		shadowWidth = mShadowWidth;shadowHeight = mShadowHeight;
		viewWidth = mViewWidth, viewHeight = mViewHeight;
		glGenFramebuffers(1, &frame.fbo);
		glGenTextures(1, &frame.texture);
		glBindTexture(GL_TEXTURE_2D, frame.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame.texture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);/**/
	}

	//普通帧缓存
	void initFrame(int mViewWidth, int mViewHeight)
	{
		glGenFramebuffers(1, &frame.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
		// create a color attachment texture

		glGenTextures(1, &frame.texture);
		glBindTexture(GL_TEXTURE_2D, frame.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mViewWidth, mViewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame.texture, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

		glGenRenderbuffers(1, &frame.rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, frame.rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mViewWidth, mViewHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frame.rbo); // now actually attach it
																									  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//MASS帧缓存，使用之后需再次使用initFbo,useOffScreenMSAA,useDefultMSAA
	void initFrameScreenMSAA(int mViewWidth, int mViewHeight)
	{
		glGenFramebuffers(1, &MSAAframe.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, MSAAframe.fbo);
		// create a color attachment texture

		glGenTextures(1, &MSAAframe.texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAAframe.texture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, mViewWidth, mViewHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MSAAframe.texture, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

		glGenRenderbuffers(1, &MSAAframe.rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, MSAAframe.rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER,4, GL_DEPTH24_STENCIL8, mViewWidth, mViewHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, MSAAframe.rbo); // now actually attach it
																											// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//使用之后需再次使用
	void initFrameHDR(int mViewHeight=1920, int mViewWidth=1080)
	{
		glGenFramebuffers(1, &frame.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
		// create a color attachment texture

		glGenTextures(2, frame.hdrTexture);
		for (int i = 0;i < 2;i++)
		{
			glBindTexture(GL_TEXTURE_2D, frame.hdrTexture[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mViewWidth, mViewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, frame.hdrTexture[i], 0);
			// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

		}
		
		glGenRenderbuffers(1, &frame.rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, frame.rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mViewWidth, mViewHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frame.rbo); // now actually attach it
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);																								// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ping-pong-framebuffer for blurring
		glGenFramebuffers(2, pingpong.pingpongFbo);
		glGenTextures(2, pingpong.hdrTexture);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpong.pingpongFbo[i]);
			glBindTexture(GL_TEXTURE_2D, pingpong.hdrTexture[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mViewWidth, mViewHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpong.hdrTexture[i], 0);
			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}
	}

	void initFbo(int mViewHeight, int mViewWidth)
	{
		glGenFramebuffers(1, &frame.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
		// create a color attachment texture

		glGenTextures(1, &frame.texture);
		glBindTexture(GL_TEXTURE_2D, frame.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mViewHeight, mViewWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame.texture, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void initRbo(int mViewHeight, int mViewWidth)
	{
		glGenRenderbuffers(1, &frame.rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, frame.rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mViewHeight, mViewWidth); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frame.rbo); // now actually attach it

	}

	void BeginFrameOrdinary()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void EndFrameOrdinary()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void BeginFrameHDR()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void EndFrameHDR()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void BeginPingPongFrame(unsigned int horizontal)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpong.pingpongFbo[horizontal]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void EndPingPongFrame()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void EndFrameScreenMSAA(int mViewWidthOld=1920,int mViewHeightOld=1080,int mViewWidthNew=1920,int mViewHeightNew=1080)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAAframe.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame.fbo);
		glBlitFramebuffer(0, 0, mViewWidthOld, mViewHeightOld, 0, 0, mViewWidthNew, mViewHeightNew, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void BeginFrameScreenMSAA()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, MSAAframe.fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	}

	void BeginShadowFrameRender()
	{

		lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		lightView = glm::lookAt(mlightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		mShader->use();
		mShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, frame.fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

	}

	void EndShadowFrameRender()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, viewWidth, viewHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}

	void read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frame.fbo);
	}

	void write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame.fbo);
	}

	unsigned int getTextureID()
	{
		return frame.texture;
	}

	unsigned int* getHdrTextureID()
	{
		return frame.hdrTexture;
	}

	unsigned int* getPingPongTextureID()
	{
		return pingpong.hdrTexture;
	}

	unsigned int getFbo()
	{
		return frame.fbo;
	}
	
	virtual void render()
	{
		
	}

	void init(FRAMEATTR mType, int mViewWidth=1920, int mViewHeight=1080, int mViewWidth1=2048, int mViewHeight1=2048)
	{
		switch (mType)
		{
		case FrameBuffer::FrameOrdinary:
			initFrame(mViewWidth,mViewHeight);
			break;
		case FrameBuffer::FrameShadow:
			initShadow(mViewWidth, mViewHeight, mViewWidth1, mViewHeight1);
			break;
		case FrameBuffer::FrameMSAA:
			initFrameScreenMSAA(mViewWidth, mViewHeight);
			initFbo(mViewWidth, mViewHeight);
			break;
		default:
			break;
		}
	}

	void BeginFrame()
	{
		switch (type)
		{
		case FrameBuffer::FrameOrdinary:
			BeginFrameOrdinary();
			break;
		case FrameBuffer::FrameShadow:
			BeginShadowFrameRender();
			break;
		case FrameBuffer::FrameMSAA:
			BeginFrameScreenMSAA();
			break;
		default:
			break;
		}
		
	}

	void EndFrame()
	{
		switch (type)
		{
		case FrameBuffer::FrameOrdinary:
			EndFrameOrdinary();
			break;
		case FrameBuffer::FrameShadow:
			EndShadowFrameRender();
			break;
		case FrameBuffer::FrameMSAA:
			EndFrameScreenMSAA();
			break;
		default:
			break;
		}
		
	}

	virtual void *re()
	{
		return this;
	}
private:
	FRAMEATTR type;
	PDE::SpecialID frame, MSAAframe, pingpong;
	glm::mat4 lightProjection, lightView, lightSpaceMatrix;
	shader *mShader;
	glm::vec3 mlightPos;
	float near_plane, far_plane;
	unsigned int shadowWidth, shadowHeight, viewWidth, viewHeight;
};

#endif
