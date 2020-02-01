#ifndef __DIMMENSIONWORLDCONSOLE_H__
#define __DIMMENSIONWORLDCONSOLE_H__

#include "fantastic.h"
#include "PurpleDreamElf.h"
#include "PlugInContainer.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

class DimmensionWorldConsole
{
public:
	static DimmensionWorldConsole *create(std::string classname);

	DimmensionWorldConsole();
	int Main();
	inline virtual void InitSystem();
	inline virtual void ImGuiRenderSystem();
	//������Ⱦ��������
	inline virtual void SwapBuffersSystem();
	//ˢ����Ļ
	inline virtual void Clear();
	inline virtual void NewImgGuiFrame();
	//��Ⱦ����
	inline virtual void Render();
	inline virtual void RenderBasis();
	inline virtual void RenderImgUI();
	inline virtual void StartSystemThread();
	//��ʼ����ͨ����
	inline virtual void InitBasic();
	inline virtual void Init();
	inline virtual void End();
	void Exit();
	~DimmensionWorldConsole();
protected:
	bool					mExit;
	float					mScreenWidth;
	float					mScreenHeight;
	std::string				mName;
	glm::vec4				mClearColor;
	GLFWwindow				*mWindow;
	shader					*mShader;
	shader					*mShadow;
	ASE::PlugInContainer	*mPlugIn;

};

#endif

