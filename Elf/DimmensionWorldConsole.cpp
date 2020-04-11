#include "DimmensionWorldConsole.h"

SystemAttribute::SystemAttribute():
mSpace(glm::scale(glm::vec3(1.0f))), mLightProjection(glm::mat4(1.0f)), mLightView(glm::mat4(1.0f))
{
	SystemActionShadow = 0;
	SystemActionBone = 0;
	SystemActionShape = 1;
	SystemLightLocation = glm::vec4(0.0f);
	SystemLightDirection = glm::vec4(0.5f, 1.0f, 0.8f,0.0f);
	SystemLightAmbient = glm::vec4(0.6f);
	SystemLightDiffuse = glm::vec4(0.6f);
	SystemLightSpecular = glm::vec4(0.6f);
}
bool SystemAttribute::operator ==(SystemAttribute &msys)
{
	if (this->mEnvironment == msys.mEnvironment&&
		this->mLightProjection == msys.mLightProjection&&
		this->mLightView == msys.mLightView&&
		this->mOrdinary == msys.mOrdinary&&
		this->mSpace == msys.mSpace&&
		this->mSunlight == msys.mSunlight)
	{
		return true;
	}
	return false;
}
SystemAttribute::~SystemAttribute()
{

}

DimmensionWorldConsole::DimmensionWorldConsole() :mExit(false), mName(""), mClearColor(glm::vec4(0.1, 0.1, 0.1, 1.0f)),
mScreenWidth(1000), mScreenHeight(900), mShadowOrthoWidth(100), mShadowOrthoNear(0.001f), mShadowOrthoFar(100.f),
mShadowLookAtEyes(glm::vec3(0.0f)),mShadowLookAtDirection(glm::vec3(0.0, 1.0, 0.0))
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
		this->mSys->mSpace = glm::scale(glm::vec3(i += 0.001f));
		if (this->mSys&&!(this->mSysCopy == *this->mSys)&&this->mSystemAttribute_ssao)
		{
			this->mSysCopy = *this->mSys;
			this->mShader->setShaderStorageBufferObjectData(*this->mSystemAttribute_ssao,sizeof(SystemAttribute),mSys);
		}
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
	std::cout << "init glfw opengl finish" << endl;
	//初始化imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();(void)io;
	ImGui::StyleColorsDark;
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	ImGui_ImplOpenGL2_Init();
	std::cout << "init imgui finish" << endl;
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

void DimmensionWorldConsole::SetSystemAttribute(unsigned int id)
{
	if (!this->mSystemAttribute_ssao)
		this->mSystemAttribute_ssao = new unsigned int(id);
	else
		*this->mSystemAttribute_ssao = id;
}

void DimmensionWorldConsole::SetShadowOrthoWidth(float width)
{
	this->mShadowOrthoWidth = width;
}

void DimmensionWorldConsole::SetShadowOrthoNear(float mNear) 
{
	this->mShadowOrthoNear = mNear;
}

void DimmensionWorldConsole::SetShadowOrthoFar(float mFar)
{
	this->mShadowOrthoFar = mFar;
}
void DimmensionWorldConsole::SetShadowShadowLookAtEyes(glm::vec3 eyes)
{
	this->mShadowLookAtEyes = eyes;
}

void DimmensionWorldConsole::SetShadowLookAtDirction(glm::vec3 direction)
{
	this->mShadowLookAtDirection = direction;
}

//初始化普通数据
void DimmensionWorldConsole::InitBasic()
{
	mSys = new SystemAttribute;
	this->mShader = new shader("../Elf/magic/shader/modelLight.vs", "../Elf/magic/shader/modelLight.fs");
	this->mShadow = new shader("../Elf/magic/shader/shadowTexture.vs", "../Elf/magic/shader/shadowTexture.fs");
	this->mPlugIn = new ASE::PlugInContainer;
	this->mShader->use();
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

	this->SetSystemAttribute(this->mShader->createShaderStorageBufferObject(0, sizeof(SystemAttribute), NULL, GL_DYNAMIC_COPY));
	this->mShader->setShaderStorageBufferObjectData(*this->mSystemAttribute_ssao,sizeof(SystemAttribute), (void*)mSys);
	std::cout << "init shader finish" << endl;
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
	DimmensionWorldConsole *dwd = DimmensionWorldConsole::create("dwd");
	if (dwd != 0)
	{
		ret = dwd->Main();
		delete dwd;
	}
	return 0;
}

