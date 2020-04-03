#include "DimmensionWorldConsole.h"
#include "Biological.h"
#include "Camera.h"
#include "Shadow.h"
#include "PQEModel.h"
#include "PQEExport.h"

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
		//PQE::PQEExport Export("aa.txt");

		model = new PQE::PQEModel("miku.fbx");
		//PQE::PQEExport Export(model->GetModel(),"aa.txt");
		//model->setModel(Export.getm()) ;
		printf("-----------------\n");
	}

	void RenderBasis()
	{
		if (i > 360.f)
			i = 0;
		i += 0.6f;
		mShader->use();
		mShader->use();
		mShader->setMat4("view", look.getViewMat());
		mShader->setVec3("carmePos", glm::vec3(0.0f));
		mShader->setMat4("model", glm::rotate(glm::rotate(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)), glm::vec3(0.0f, -100.0f, -100.0f)), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		model->Render(mShader);
		int a[100];
		std::vector<int> aa;
		for (int i = 0; i < 100; i++)
		{
			a[i] = i;
		}
		aa.resize(100);
		memcpy(&aa[0],a,100*sizeof(int));
		//look.useMouse();
	}

private:
	QdeBiological IE_MODEL;
	Camera look;
	Shadow shadow;
	PQE::PQEModel *model;
	float i = 0;
};