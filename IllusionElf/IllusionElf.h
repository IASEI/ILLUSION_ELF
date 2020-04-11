#include "DimmensionWorldConsole.h"
#include "Biological.h"
#include "Camera.h"
#include "Shadow.h"
#include "PQEModel.h"
#include "PQEExport.h"
#include "PQEImport.h"
#include "PQEModelRender.h"
#include "PQEFrameRender.h"
#include "PQEOperator.h"

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
		shadow.init();
		IE_MODEL.mshader = mShader;
		ren >> new PQE::PQEImport("miku.ase");
		frame >>new PQE::FrameAttribute(&ren,mShader,1000,1000);
		printf("-----------------\n");
	}

	void RenderBasis()
	{
		if (i > 360.f)
			i = 0;
		i += 0.1f;
		mShader->use();
		mShader->setMat4("view", look.getViewMat());
		mShader->setVec3("carmePos", glm::vec3(0.0f));
		mShader->setMat4("model", glm::rotate(glm::rotate(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)), glm::vec3(0.0f, -100.0f, -100.0f)), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		//shadow.BeginShadowFrameRender();
		//shadow.EndShadowFrameRender();
		//frame.Render();
		glEnable(GL_DEPTH_TEST);
		ren.Render(mShader);

	}

private:
	QdeBiological IE_MODEL;
	Camera look;
	Shadow shadow;
	PQE::PQEImport *model;
	PQE::PQEModelRender ren;
	PQE::PQEFrameRender frame;
	float i = 0;
};