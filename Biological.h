#ifndef __BIOLOGICAL_H__
#define __BIOLOGICAL_H__

#include "QdeModel.h"
#include "TimerTask.h"

class QdeBiological :public QdeModel
{
public:
	QdeBiological() 
	{
		shape = true;
	}
	~QdeBiological() {}

	//��ת����
	void opationBodyRotate(float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["body"]], ubo, x, y, z);
	}

	//��ת�۾�
	void opationEyesRotate(std::string id = 0,float x = 0, float y = 0, float z = 0)
	{
		if (id == "Eyes")
		{
			x = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
			y = limit(glm::radians(-10.f), glm::radians(y), glm::radians(10.f));
			z = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
			setModelBoneMatrixRotate(&model->matrix[model->nameToBone["LeftEye"]], ubo, x, y, z);
			setModelBoneMatrixRotate(&model->matrix[model->nameToBone["RightEye"]], ubo, x, y, z);
		}
		
	}

	//��תͷ��
	void opationHeadRotate(float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["Head"]], ubo, x, y, z);
	}

	//��ת����
	void opationNeckRotate(float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["Neck"]], ubo, x, y, z);
	}

	//��ת����
	void opationSpineRotate(std::string id=0, float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["No_93_joint_RightHip" + id]], ubo, x, y, z);
	}

	//��ת���
	void opationLeftArmRotate(std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		x = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
		y = limit(glm::radians(0.f), glm::radians(y), glm::radians(90.f));
		z = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["LeftArm" + id]], ubo, x, y, z);
	}

	//��ת�ұ�
	void opationRightArmRotate(std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		x = limit(glm::radians(0.f), glm::radians(x), glm::radians(55.f));
		y = limit(glm::radians(0.f), glm::radians(y), glm::radians(90.f));
		z = limit(glm::radians(0.f), glm::radians(x), glm::radians(90.f));
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["RightArm" + id]], ubo, x, y, z);
	}

	//��ת����
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

	//��ת����
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

	//��ת����
	void opationLeftLegRotate(std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["LeftLeg" + id]], ubo, x, y, z);
	}

	//��ת����
	void opationRightLegRotate(std::string id = 0, float x = 0, float y = 0, float z = 0)
	{
		setModelBoneMatrixRotate(&model->matrix[model->nameToBone["RightLeg" + id] ] , ubo, x, y, z);
	}

	//����
	void expression()
	{
		if (blink)
			blink=!blinkAutomatic();
		else
			blink = timer.runTimerInterruption(1000);
	}

	//գ�۾�
	bool blinkAutomatic()
	{
		QdeModel::pqeBlendShapeChannel *channel=findShapeChannelMap("");
		setShapeWeight(channel, weightAmount);
		if (channel->weight <= 0 || channel->weight >= 1) { weightAmount *= -1; return true; }
		return false;
	}

	//���¶���
	bool updateAction()
	{
		int cout = model->matrix.size();
		glm::mat4 *date = new glm::mat4[cout];
		getChanngeBoneMatrix(&model->matrix[0],glm::mat4(1.0f), date);
		mshader->use();
		mshader->setUBOs("boneMat4", date, sizeof(glm::mat4)*cout, 0, ubo);
		delete[] date;
		return true;
	}

	//���±���
	bool updateExpression()
	{
		std::vector<glm::vec4> &pos = modelData->position;

		if (shape)
		{
			mshader->use();
			ubo1=mshader->createShaderStorageBufferObject(1,sizeof(float)*modelData->weightShape.size(),&modelData->weightShape[0], GL_DYNAMIC_COPY);
			ubo2=mshader->createShaderStorageBufferObject(3, sizeof(glm::vec4)*modelData->position.size(),NULL, GL_DYNAMIC_COPY);
		}
		else 
		{
			if (modelData->weightShape[0] > 1.0f)
				modelData->weightShape[0] = 0;
			modelData->weightShape[0] += 0.01f;
			mshader->use();
			setShaderStorageBufferObjectData(ubo1, sizeof(float)*modelData->weightShape.size(), &modelData->weightShape[0]);
			setShaderStorageBufferObjectData(ubo2, sizeof(glm::vec4)*modelData->position.size(), &modelData->positionShape[0][0]);
		


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
	//��Ⱦ
	bool runRender()
	{
		updateAction();
		updateExpression();
		render();
		return true;
	}
private:
	//���ñ仯�Ĺ���
	inline void getChanngeBoneMatrix(pqeMatrix *mt,glm::mat4 parent,glm::mat4 *m)
	{
		std::vector<glm::mat4> &self = modelData->selfMatrix;
		std::vector<glm::mat4> &finish = modelData->finshMatrix;
		std::vector<glm::mat4> &offest = modelData->offestMatrix;
		glm::mat4 GlobalTransformation = finish[mt->finish]*parent;
		m[mt->id] = offest[mt->offset] * GlobalTransformation*model->rootNode->transformation;
		for (GLuint i = 0; i < mt->child.size(); i++)
		{
			getChanngeBoneMatrix(&model->matrix[mt->child[i]], GlobalTransformation, m);
		}
	}

	//�ж���������ӽ��Ǹ�
	float limit(float mix, float var, float max)
	{
		if ((mix < var) && (var < max))
			return var;
		else if (var <= mix)
			return mix;
		else
			return max;
	}

	//��ʼ�������Ϣ
	void initBiologicalInfo()
	{
		
	}

public:
	int iid;
	PQE::TimerTask timer;
	bool blink;
	float weightAmount;
	int id;
	int ubo1, ubo2;
	bool shape;
	std::vector<glm::vec4> m;
	std::vector<glm::vec4> mm;
};

#endif

