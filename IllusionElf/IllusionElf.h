#include "DimmensionWorldConsole.h"
#include "Biological.h"
#include "Camera.h"
#include "Shadow.h"
#include "PQEModel.h"

class IllusionElf :public DimmensionWorldConsole
{
public:
	IllusionElf();
	~IllusionElf();

	void Init()
	{
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		mShader->use();
		mShader->setBool("animat", true);
		mShader->setVec3("carmePos", look.getView());
		mShader->setMat4("model", glm::rotate(translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)), glm::vec3(0., 0., 0.)), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		mShader->setMat4("lightSpaceMatrix", shadow.getLightSpaceMatrix());
		IE_MODEL.mshader = mShader;
		//IE_MODEL.load("miku.fbx");

		model = new PQE::PQEModel("miku.fbx");
		this->model->SetBoneSSAOId(mShader->createShaderStorageBufferObject(1, sizeof(glm::mat4)*this->model->GetModel()->matrixNum, NULL, GL_DYNAMIC_COPY));
		this->model->SetShapeSSAOId(mShader->createShaderStorageBufferObject(2, sizeof(glm::vec3) * 1000000, NULL, GL_DYNAMIC_COPY));
		printf("-----------------\n");
	}

	void RenderBasis()
	{
		if (i > 360.f)
			i = 0;
		i += 0.05f;
		mShader->use();
		mShader->use();
		mShader->setMat4("view", look.getViewMat());
		mShader->setVec3("carmePos", glm::vec3(0.0f));
		mShader->setMat4("model", glm::rotate(glm::rotate(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)), glm::vec3(0.0f, -100.0f, -100.0f)), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		model->Render(mShader);
		//look.useMouse();
	}

private:
	QdeBiological IE_MODEL;
	Camera look;
	Shadow shadow;
	PQE::PQEModel *model;
	float i = 0;
};