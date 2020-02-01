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
	//交换渲染缓存数据
	inline virtual void SwapBuffersSystem();
	//刷新屏幕
	inline virtual void Clear();
	inline virtual void NewImgGuiFrame();
	//渲染画面
	inline virtual void Render();
	inline virtual void RenderBasis();
	inline virtual void RenderImgUI();
	inline virtual void StartSystemThread();
	//初始化普通数据
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

