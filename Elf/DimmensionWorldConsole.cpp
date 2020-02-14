#include "DimmensionWorldConsole.h"

DimmensionWorldConsole::DimmensionWorldConsole() :mExit(false), mName(""), mClearColor(glm::vec4(0.1, 0.1, 0.1, 1.0f)), mScreenWidth(1920), mScreenHeight(1079)
{

}

DimmensionWorldConsole::~DimmensionWorldConsole()
{

}

int DimmensionWorldConsole::Main()
{

	this->InitSystem();
	this->StartSystemThread();
	while (!this->mExit)
	{
		this->Clear();
		this->Render();
		this->ImGuiRenderSystem();
		this->SwapBuffersSystem();
	}
	this->End();

	return 1;
}
void DimmensionWorldConsole::InitSystem()
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
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	ImGui_ImplOpenGL2_Init();
	printf("InittSystem\n");
	this->InitBasic();
	//其他 初始化
	this->Init();
}
void DimmensionWorldConsole::ImGuiRenderSystem()
{
	ImGui::Render();
	GLint last_program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	glUseProgram(0);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	glUseProgram(last_program);
}
//交换渲染缓存数据
void DimmensionWorldConsole::SwapBuffersSystem()
{
	glfwPollEvents();
	glfwMakeContextCurrent(mWindow);
	glfwSwapBuffers(mWindow);
}
//刷新屏幕
void DimmensionWorldConsole::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void DimmensionWorldConsole::NewImgGuiFrame()
{
	ImGui_ImplOpenGL2_NewFrame();

	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
//渲染画面
void DimmensionWorldConsole::Render()
{
	this->RenderBasis();			//自定义opengl渲染
	this->mPlugIn->Run();			//运行插件
	this->NewImgGuiFrame();			//新建ImGui
	this->RenderImgUI();			//自定义ImGui渲染
}
void DimmensionWorldConsole::RenderBasis()
{

}
void DimmensionWorldConsole::RenderImgUI()
{

}
void DimmensionWorldConsole::StartSystemThread() {}
//初始化普通数据
void DimmensionWorldConsole::InitBasic()
{
	
	this->mShader = new shader("../Elf/magic/shader/modelLight1.vs", "../Elf/magic/shader/modelLight1.fs");
	this->mShadow = new shader("../Elf/magic/shader/shadowTexture1.vs", "../Elf/magic/shader/shadowTexture1.fs");
	this->mPlugIn = new ASE::PlugInContainer;
	this->mShader->use();
	this->mShader->setBool("animat", false);
	this->mShader->setMat4("projection", glm::perspective(glm::radians(45.0f), this->mScreenWidth / this->mScreenHeight, 0.1f, 10000.0f));
	//this->mShader->setMat4("projection", glm::ortho(-1920.0f/1080, 1920.0f / 1080, -1.0f, 1.0f, -1.0f, 100.0f));
	this->mShader->setInt("mShadowMap", 1);

	this->mShader->setBool("mConfig.isDiffuseSampler", false);
	this->mShader->setBool("mConfig.isSpecularSampler", false);
	this->mShader->setBool("mConfig.isLightdirction", true);
	this->mShader->setInt("mConfig.lightPointNum", 0);
	this->mShader->setInt("mConfig.lightSpotNum", 0);

	this->mShader->setVec3("mMaterial.vdiffuse", glm::vec3(0.f));
	this->mShader->setVec3("mMaterial.vspecular", glm::vec3(0.f));
	this->mShader->setInt("mMaterial.diffuse", 0);
	this->mShader->setInt("mMaterial.specular", 1);
	this->mShader->setFloat("mMaterial.shininess", 2);

	this->mShader->setVec3("mDirLight.direction", glm::vec3(-0.5f, -1.0f, -0.8f));
	this->mShader->setVec3("mDirLight.ambient", glm::vec3(0.6f));
	this->mShader->setVec3("mDirLight.diffuse", glm::vec3(0.6f));
	this->mShader->setVec3("mDirLight.specular", glm::vec3(0.0f));

	printf("InitBasic\n");
	this->mPlugIn->Init();
}
void DimmensionWorldConsole::Init() {}
void DimmensionWorldConsole::End() {}
void DimmensionWorldConsole::Exit()
{
	this->mExit = true;
}

int main(int argc, char *argv[])
{
	int ret = -1;
	printf("begin");
	DimmensionWorldConsole *dwd = DimmensionWorldConsole::create("dwd");
	if (dwd != 0)
	{
		ret = dwd->Main();
		delete dwd;
	}
	return 0;
}

