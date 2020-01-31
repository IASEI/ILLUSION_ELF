#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "pde\PurpleDreamElf.h"
#include "Model.h"
#include "shader.h"

class Light
{
public:
	Light()
	{
		showModel = false;
		model = glm::mat4(1.0f);
	}

	~Light();
	void initLight(PDE::lightType type,glm::vec3 pos,glm::vec3 diffuse,glm::vec3 ambient,glm::vec3 specular)
	{
		light.type = type;
		light.pos = pos;
		light.diffuse = diffuse;
		light.ambient = ambient;
		light.specular = specular;
	}

	void initShader(const char* vertexPath, const char* fragmentPath)
	{
		mShader.initShader(vertexPath, fragmentPath);
	}

	void setPostion(glm::vec3 pos)
	{
		light.pos = pos;
	}

	void setDiffuse(glm::vec3 diffuse)
	{
		light.diffuse = diffuse;
	}

	void setAmbient(glm::vec3 ambient)
	{
		light.ambient = ambient;
	}

	void setSpecular(glm::vec3 specular)
	{
		light.specular = specular;
	}

	void setType(PDE::lightType type)
	{
		light.type = type;
	}

	PDE::light_pde getlight()
	{
		return light;
	}

	glm::vec3 getDiffuse()
	{
		return light.diffuse;
	}

	glm::vec3 getAmbient()
	{
		return light.ambient;
	}

	glm::vec3 getSpecular()
	{
		return light.specular;
	}

	glm::vec3 getPos()
	{
		return light.pos;
	}

	PDE::lightType getType()
	{
		return light.type;
	}

	void draw()
	{
		mShader.use();
		if (showModel)
		{
			if (useDefault)
			{
				mShader.setMat4("model", model);
			}
			else
			{
				//lightModel.setGloal(light.pos);
				//lightModel.setShader(&mShader);
				//lightModel.draw();
			}
			
		}

		mShader.setMat4("model", model);
		mShader.setMat4("view",view);
		mShader.setMat4("projection", projection);

		mShader.setVec3("light.position",light.pos);
		mShader.setVec3("light.ambient", light.ambient);
		mShader.setVec3("light.diffuse", light.diffuse);
		mShader.setVec3("light.specular", light.specular);
	}

	void setView(glm::mat4 mView)
	{
		view = mView;
	}

	void setProjection(glm::mat4 pro)
	{
		projection = pro;
	}

	void setModel(glm::mat4 models)
	{
		model = models;
	}

	shader mShader;
private:
	bool showModel;
	bool useDefault;
	PDE::light_pde light;
//	Model lightModel;
	
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;
};
#endif

