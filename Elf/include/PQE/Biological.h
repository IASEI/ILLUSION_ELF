#ifndef __BIOLOGICAL_H__
#define __BIOLOGICAL_H__

#include "QdeModel.h"
#include "TimerTask.h"
#include "Physical.h"

class QdeBiological :public QdeModel
{
public:
	QdeBiological() 
	{
		shape = true;
		first = true;
		isfair = true;
		t = 1;
		init = true;
		this->rule = new PQE::PhysicalRule(glm::vec3(0, -9.8f, 0), 0.02f, 0.2f, -10000000.5f);
	}
	~QdeBiological() {}

	//旋转身体
	void opationBodyRotate(float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["body"]], ubo, x, y, z);
	}

	//旋转眼睛
	void opationEyesRotate(float x = 0, float y = 0, float z = 0)
	{
		x = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
		y = limit(glm::radians(-10.f), glm::radians(y), glm::radians(10.f));
		z = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["LeftEye"]], ubo, x, y, z);
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["RightEye"]], ubo, x, y, z);
	}

	//旋转头部
	void opationHeadRotate(float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["Head"]], ubo, x, y, z);
	}

	//旋转颈部
	void opationNeckRotate(float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["Neck"]], ubo, x, y, z);
	}

	//旋转脊柱
	void opationSpineRotate(std::string id=0, float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["No_93_joint_RightHip" + id]], ubo, x, y, z);
	}

	//旋转左臂
	void opationLeftArmRotate(std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		x = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
		y = limit(glm::radians(0.f), glm::radians(y), glm::radians(90.f));
		z = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["LeftArm" + id]], ubo, x, y, z);
	}

	//旋转右臂
	void opationRightArmRotate(std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		x = limit(glm::radians(0.f), glm::radians(x), glm::radians(55.f));
		y = limit(glm::radians(0.f), glm::radians(y), glm::radians(90.f));
		z = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["RightArm" + id]], ubo, x, y, z);
	}

	//旋转左手
	void opationLeftHandRotate(unsigned int index=0, std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		std::string name="LeftHand";
		switch (index)
		{
		case 0:
			name = "";
			id = "";
			break;
		case 1:
			name = "";
			break;
		case 2:
			name = "";
			break;
		case 3:
			name = "";
			break;
		case 4:
			name = "";
			break;
		case 5:
			name = "";
			break;
		default:
			break;
		}
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["No_57_joint_LeftArm" + id]], ubo, x, y, z);
	}

	//旋转右手
	void opationRightHandRotate(unsigned int index = 0, std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		std::string name="RightHand";
		switch (index)
		{
		case 0:
			name = "";
			id = "";
			break;
		case 1:
			name = "";
			break;
		case 2:
			name = "";
			break;
		case 3:
			name = "";
			break;
		case 4:
			name = "";
			break;
		case 5:
			name = "";
			break;
		default:
			break;
		}
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["No_93_joint_RightHip" + id]], ubo, x, y, z);
	}

	//旋转左腿
	void opationLeftLegRotate(std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["LeftLeg" + id]], ubo, x, y, z);
	}

	//旋转右腿
	void opationRightLegRotate(std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["RightLeg" + id] ] , ubo, x, y, z);
	}

	//表情
	void expression()
	{
		if (blink)
			blink=!blinkAutomatic();
		else
			blink = timer.runTimerInterruption(1000);
	}

	//眨眼睛
	bool blinkAutomatic()
	{
		QdeModel::pqeBlendShapeChannel *channel=findShapeChannelMap("");
		setShapeWeight(channel, weightAmount);
		if (channel->weight <= 0 || channel->weight >= 1) { weightAmount *= -1; return true; }
		return false;
	}

	//更新动作
	bool updateAction()
	{
		int cout = model->matrix.size();
		glm::mat4 *date = new glm::mat4[cout];
		ai += 0.1f;
		if (first)
		{
			getChanngeBoneMatrix(&model->matrix[0], glm::mat4(1.0f), date);
			initfair(date);
			v = SampleTestGenShaderPos(sizeof(glm::vec3)*phy.size(), &phy[0]);
			inita = this->phy[0];
			first = false;
		}
		else
		{
			mshader->use();
			updataFair();
			glBindVertexArray(v.x);
			glDrawArrays(GL_POINTS, 0, phy.size());
			glBindVertexArray(0);
		}
		
		delete[] date;
		return true;
	}

	//更新表情
	bool updateExpression()
	{

		if (shape)
		{
			std::vector<glm::vec4> pos;

			for (int i = 0; i < modelData->positionShape.size(); i++)
			{
				pos.insert(pos.end(), modelData->positionShape[i].begin(), modelData->positionShape[i].end());
			}
			mshader->use();
			mshader->setInt("mShape.vertexCout", modelData->position.size());
			mshader->setInt("mShape.weightCout", modelData->weightShape.size());
			ubo1=mshader->createShaderStorageBufferObject(1,sizeof(float)*modelData->weightShape.size(),&modelData->weightShape[0], GL_DYNAMIC_COPY);
			ubo2=mshader->createShaderStorageBufferObject(3, sizeof(glm::vec4)*modelData->position.size()*modelData->positionShape.size(), &pos[0], GL_DYNAMIC_COPY);
		}
		else 
		{
			if (modelData->weightShape[0] > 1.0f || modelData->weightShape[0] < 0.0f)
			{
				t *= -1;
			}
			//modelData->weightShape[0] += 0.01f;
			//modelData->weightShape[12] += 0.02f*t;
			modelData->weightShape[0] += 0.01f*t;
			mshader->use();
			
			setShaderStorageBufferObjectData(ubo1, sizeof(float)*modelData->weightShape.size(), &modelData->weightShape[0]);
		}
		shape = false;
		return true;
	}
	void setShaderStorageBufferObjectData(unsigned int id, GLsizeiptr size, void *date)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
		GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		memcpy(p, date, size);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}
	//渲染
	bool runRender()
	{
		updateAction();
		//updateExpression();
		
		//render();
		return true;
	}

	void initfair(glm::mat4 *m)
	{
		fairBone(&model->matrix[model->nameToBone["No_128_joint_migiHZ7"]], m[model->matrix[model->nameToBone["No_128_joint_migiHZ7"]].parent],NULL);
	}

	void updataFair()
	{
		//this->rule->setRopeConnectionVel(glm::vec3(0));
		this->rule->PhysicalObject[0]->position = inita;
		for(int i=0;i<10;i++)
		this->rule->Operate(0.001f);
		for (int i = 0; i < this->phy.size(); i++)
		{
			this->phy[i] = this->rule->PhysicalObject[i]->position;
		}
		glBindVertexArray(v.x);

		glBindBuffer(GL_ARRAY_BUFFER, v.y);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*phy.size(), &phy[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
private:
	//设置变化的骨骼
	inline void fairBone(pqeMatrix *mt,glm::mat4 parentMat, PQE::Physical *parent)
	{
		std::vector<glm::mat4> &self = modelData->selfMatrix;
		std::vector<glm::mat4> &finish = modelData->finshMatrix;
		std::vector<glm::mat4> &offest = modelData->offestMatrix;
		glm::mat4 GlobalTransformation = finish[mt->finish]* parentMat;
		glm::mat4 m = offest[mt->offset] * GlobalTransformation*model->rootNode->transformation;
		PQE::Physical *p = new  PQE::Physical(0.05f);
		p->position = glm::vec3(GlobalTransformation[0].w, GlobalTransformation[1].w, GlobalTransformation[2].w);
		this->rule->PhysicalObject.push_back(p);
		this->phy.push_back(p->position);
		if (parent != NULL)
		{
			this->rule->PhysicalForces.push_back(new PQE::Elastic(parent, p, 10000, glm::length(parent->position - p->position), 0.2));
		}
		for (GLuint i = 0; i < mt->child.size(); i++)
		{
			fairBone(&model->matrix[mt->child[i]], GlobalTransformation, p);
		}
	}

	//设置变化的骨骼
	inline void getChanngeBoneMatrix(pqeMatrix *mt, glm::mat4 parent, glm::mat4 *m)
	{
		std::vector<glm::mat4> &self = modelData->selfMatrix;
		std::vector<glm::mat4> &finish = modelData->finshMatrix;
		std::vector<glm::mat4> &offest = modelData->offestMatrix;
		glm::mat4 GlobalTransformation = finish[mt->finish] * parent;
		m[mt->id] = offest[mt->offset] * GlobalTransformation*model->rootNode->transformation;
		pos.push_back(glm::vec3(parent[0].w, parent[1].w, parent[2].w));
		//pos.push_back(glm::vec3(m[mt->id][0].w, m[mt->id][1].w, m[mt->id][2].w));
		mat.push_back(m[mt->id]);
		for (GLuint i = 0; i < mt->child.size(); i++)
		{
			getChanngeBoneMatrix(&model->matrix[mt->child[i]], GlobalTransformation, m);
		}
	}

	//判断输入的数接近那个
	float limit(float mix, float var, float max)
	{
		if ((mix < var) && (var < max))
			return var;
		else if (var <= mix)
			return mix;
		else
			return max;
	}

	//初始换相关信息
	void initBiologicalInfo()
	{
		
	}

	glm::int3 SampleTestGenShaderPos(GLsizeiptr size, const void *data)
	{
		GLuint VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
		return glm::int3(VAO, VBO, 0);
	}

public:
	int iid;
	PQE::TimerTask timer;
	PQE::PhysicalRule *rule;
	std::map<unsigned int, unsigned int> fair;
	bool blink;
	bool isfair;
	float weightAmount;
	int id;
	int ubo1, ubo2;
	bool shape;
	int t;
	bool first;
	std::vector<glm::vec4> m;
	std::vector<glm::vec4> mm;
	float ai=0.f;
	bool init;
	std::vector<glm::vec3> phy;
	std::vector<glm::vec3> pos;
	std::vector<glm::mat4> mat;
	glm::vec3 inita;
	shader test;
	glm::int3 v;
};

#endif

