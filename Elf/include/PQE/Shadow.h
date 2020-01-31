#ifndef __SHADOW_H__
#define __SHADOW_H__

#include "PQE\PurpleDreamElf.h"
#include "shader.h"

#include "QdeRender.h"
class Shadow :public QdeRender
{
public:
	Shadow():near_plane(0.1f),far_plane(100.0f)
	{	
		
	}

	~Shadow()
	{
	
	}

	void init(unsigned int mShadowWidth=2048, unsigned int mShadowHeight=2048, unsigned int mScrWidth=1920, unsigned int mScrHeight=1080)
	{
		shadowWidth = mShadowWidth;shadowHeight = mShadowHeight;scrWidth = mScrWidth;scrHeight= mScrHeight;
		glGenFramebuffers(1, &shadowDepthMapFrame.fbo);
		glGenTextures(1, &shadowDepthMapFrame.texture);
		glBindTexture(GL_TEXTURE_2D, shadowDepthMapFrame.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthMapFrame.fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthMapFrame.texture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);/**/
	}

	void BeginShadowFrameRender(shader *mShader,glm::vec3 mlightPos = glm::vec3(1.0f, 10.0f, 0.0f))
	{
		
		lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		lightView = glm::lookAt(mlightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		mShader->use();
		mShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthMapFrame.fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		
	}

	void EndShadowFrameRender()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, scrWidth, scrHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}

	void ShadowTexture()
	{
		glBindTexture(GL_TEXTURE_2D, shadowDepthMapFrame.texture);
		
	}

	glm::mat4 &getLightSpaceMatrix()
	{
		return lightSpaceMatrix;
	}

public:
	PDE::SpecialID shadowDepthMapFrame;
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane, far_plane;
	unsigned int shadowWidth, shadowHeight, scrWidth, scrHeight;
};

#endif

