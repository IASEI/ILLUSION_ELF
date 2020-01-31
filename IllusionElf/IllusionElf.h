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

	virtual void init()
	{
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		SysVar.renderBasic.use();
		SysVar.renderBasic.setBool("animat", true);
		SysVar.renderBasic.setVec3("carmePos", look.getView());
		SysVar.renderBasic.setMat4("model", glm::rotate(translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)), glm::vec3(0., 0., 0.)), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		SysVar.renderBasic.setMat4("lightSpaceMatrix", shadow.getLightSpaceMatrix());
		IE_MODEL.mshader = &SysVar.renderBasic;
		//IE_MODEL.load("miku.fbx");

		model = new PQE::PQEModel("miku.fbx");
		this->model->SetBoneSSAOId(SysVar.renderBasic.createShaderStorageBufferObject(1, sizeof(glm::mat4)*this->model->GetModel()->matrixNum, NULL, GL_DYNAMIC_COPY));
		this->model->SetShapeSSAOId(SysVar.renderBasic.createShaderStorageBufferObject(2, sizeof(glm::vec3) * 1000000, NULL, GL_DYNAMIC_COPY));
	}

	virtual void renderBasis()
	{
		if (i > 360.f)
			i = 0;
		i += 0.05f;
		SysVar.renderBasic.use();
		SysVar.renderBasic.setMat4("view", look.getViewMat());
		SysVar.renderBasic.setVec3("carmePos", glm::vec3(0.0f));
		SysVar.renderBasic.setMat4("model", glm::rotate(glm::rotate(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)), glm::vec3(0.0f, -100.0f, -100.0f)), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		model->Render(&SysVar.renderBasic);
		//look.useMouse();

	}
private:
	QdeBiological IE_MODEL;
	Camera look;
	Shadow shadow;
	PQE::PQEModel *model;
	float i = 0;
}