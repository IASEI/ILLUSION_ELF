#ifndef __DIMMENSIONWORLDCONSOLE_H__
#define __DIMMENSIONWORLDCONSOLE_H__

#include "fantastic.h"
#include "PurpleDreamElf.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

class DimmensionWorldConsole
{
public:

	DimmensionWorldConsole() :mExit(false),mName(""),mClearColor(glm::vec4(0.1, 0.1, 0.1,1.0f)),mScreenWidth(1920),mScreenHeight(1079)
	{
	}
	~DimmensionWorldConsole()
	{
	
	}
	static DimmensionWorldConsole *create(std::string classname);

	int Main()
	{
		
		initSystem();
		startSystemThread();
		while (!this->mExit)
		{
			clear();
			render();
			imGuiRenderSystem();
			SwapBuffersSystem();
		}
		end();

		return 1;
	}
	inline virtual void initSystem()
	{
		//初始化glfw窗口
		glfwInit();
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_SAMPLES, 16);

		mWindow = glfwCreateWindow(mScreenWidth, mScreenHeight, mName.c_str(), NULL, NULL);
		glfwSetWindowPos(mWindow, 0, 0);
		glfwMakeContextCurrent(mWindow);
		glfwSwapInterval(1);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))return;
		glViewport(0, 0, mScreenWidth, mScreenHeight);
		glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a);
		//初始化imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();(void)io;
		ImGui::StyleColorsDark;
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL2_Init();
		initBasic();
		//其他 初始化
		init();
	}
	inline virtual void imGuiRenderSystem()
	{
		ImGui::Render();
		GLint last_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		glUseProgram(0);
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		glUseProgram(last_program);
	}
	//交换渲染缓存数据
	inline virtual void SwapBuffersSystem()
	{
		glfwPollEvents();
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}
	//刷新屏幕
	inline virtual void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	inline virtual void newImgGuiFrame()
	{
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	//渲染画面
	inline virtual void render()
	{
		renderBasis();
		newImgGuiFrame();
		renderImgUI();
	}
	inline virtual void renderBasis()
	{
	
	}
	inline virtual void renderImgUI()
	{
	
	}
	inline virtual void startSystemThread(){}
	//初始化普通数据
	inline virtual void initBasic()      
	{
		this->mShader = new shader("../Elf/magic/shader/modelLight.vs", "../Elf/magic/shader/modelLight.fs");
		this->mShadow = new shader("../Elf/magic/shader/shadowTexture.vs", "../Elf/magic/shader/shadowTexture.fs");
		this->mShader->use();
		this->mShader->setBool("animat", false);
		this->mShader->setMat4("projection", glm::perspective(glm::radians(45.0f), this->mScreenWidth / this->mScreenHeight, 0.1f, 10000.0f));
		//this->mShader->setMat4("projection", glm::ortho(-1920.0f/1080, 1920.0f / 1080, -1.0f, 1.0f, -1.0f, 100.0f));
		this->mShader->setInt("mShadowMap", 1);

		this->mShader->setBool("mConfig.isDiffuseSampler", false);
		this->mShader->setBool("mConfig.isSpecularSampler", false);
		this->mShader->setBool("mConfig.isLightdirction", true);
		this->mShader->setInt("mConfig.lightPointNum", 0);
		this->mShader->setInt("mConfig.lightSpotNum",0);
		
		this->mShader->setVec3("mMaterial.diffuse", glm::vec3(0.f));
		this->mShader->setVec3("mMaterial.specular", glm::vec3(0.f));
		this->mShader->setInt("mMaterial.diffuse", 0);
		this->mShader->setInt("mMaterial.specular", 1);
		this->mShader->setFloat("mMaterial.shininess", 2);
	
		this->mShader->setVec3("mDirLight.direction", glm::vec3(-0.5f, -1.0f, -0.8f));
		this->mShader->setVec3("mDirLight.ambient", glm::vec3(0.6f));
		this->mShader->setVec3("mDirLight.diffuse", glm::vec3(0.6f));
		this->mShader->setVec3("mDirLight.specular", glm::vec3(0.0f));
	}
	inline virtual void init(){}
	inline virtual void end(){}
	void Exit()
	{
		this->mExit = true;
	}
protected:
	bool mExit;
	float mScreenWidth;
	float mScreenHeight;
	std::string mName;
	glm::vec4 mClearColor;
	GLFWwindow *mWindow;
	shader *mShader;
	shader *mShadow;

};

#endif

