#include "PQEModelRender.h"
namespace PQE
{
	PQEModelRender::PQEModelRender()
	{
	
	}

	void PQEModelRender::Render(shader *mshader)
	{
		for (unsigned int modelNum=0; modelNum < this->mModel.size(); modelNum++)
		{
			PQE_MODEL *model = this->mModel[modelNum];
			glm::mat4 *matrixDate = NULL;
			unsigned int cout = model->meshNum;
			if (!mshader)
				return;
			mshader->use();
			SetBoneSSAOId(mshader->createShaderStorageBufferObject(2, sizeof(glm::mat4)*model->matrixNum, NULL, GL_DYNAMIC_COPY));
			if (bone_ssao)
			{
				matrixDate = new glm::mat4[model->matrixNum];
				model->ComputeBoneMatrix(&model->mNode[0], glm::mat4(1.0f), matrixDate);
				mshader->setShaderStorageBufferObjectData(*this->bone_ssao, model->matrixNum * sizeof(glm::mat4), matrixDate);
			}
			for (unsigned int i = 0; i < model->nodeNum; i++)
			{
				PQE_NODE *node = &model->mNode[i];
				unsigned int shapeNodeVertexCount = 0;
				unsigned int shpaeVertexCount = 0;
				unsigned int weightCount = 0;
				bool isShape = false;
				if (node->meshNum > 0)
				{
					if (mshader&& node->shapeNum)
					{
						weightCount = node->shapeNum;
						float *weight = new float[weightCount];
						PQE_SHAPE *shpae = &model->mShape[node->mShapeIndex[0]];
						shpaeVertexCount = shpae->vertexNum;
						shapeNodeVertexCount = node->shapeNum*shpaeVertexCount;
						glm::vec4 *shapeVertex = new glm::vec4[sizeof(glm::vec4)*shapeNodeVertexCount];
						SetShapeSSAOId(mshader->createShaderStorageBufferObject(3, sizeof(glm::vec4) *shapeNodeVertexCount, NULL, GL_DYNAMIC_COPY));
						SetShapeWeightSSAOId(mshader->createShaderStorageBufferObject(1, sizeof(float) * weightCount, NULL, GL_DYNAMIC_COPY));
						for (unsigned int i = 0; i < node->shapeNum; i++)
						{
							shpae = &model->mShape[node->mShapeIndex[i]];
							weight[i] = shpae->weight;
							if (i == 2)weight[i] = 1.0f;
							memcpy(shapeVertex + shpaeVertexCount * i, &shpae->mPosition[0], shpaeVertexCount * sizeof(glm::vec4));
						}
						mshader->setShaderStorageBufferObjectData(*this->shape_ssao, sizeof(glm::vec4)*shapeNodeVertexCount, shapeVertex);
						mshader->setShaderStorageBufferObjectData(*this->shape_weight_ssao, weightCount * sizeof(float), weight);
						isShape = true;
						delete[] weight;
						delete[] shapeVertex;
					}
					mshader->setInt("shape_data.shapeNodeVertexCount", shapeNodeVertexCount);
					mshader->setInt("shape_data.vertexCount", shpaeVertexCount);
					mshader->setInt("shape_data.weightCount", weightCount);
					for (unsigned int i = 0; i < node->meshNum; i++)
					{
						unsigned int meshIndex = node->mMeshIndex[i];
						PQE_MESH* mesh = &model->mMesh[node->mMeshIndex[i]];
						PQE_MATERIAL *mat = &model->mMaterial[model->mMesh[meshIndex].mMaterilIndex];
						glActiveTexture(GL_TEXTURE0);
						for (unsigned int j = 0; j < mat->textureNum; j++)
						{
							glBindTexture(GL_TEXTURE_2D, model->mTexture[mat->mTextureIndex[j]].mId);
						}
						glBindVertexArray(mesh->vao);
						glDrawElements(GL_TRIANGLES, model->mMesh[meshIndex].faceNum, GL_UNSIGNED_INT, 0);
					}
					if (isShape)
					{
						glDeleteBuffers(1, this->shape_ssao);
						glDeleteBuffers(1, this->shape_weight_ssao);
					}

				}
			}
			glDeleteBuffers(1, this->bone_ssao);
			delete[] matrixDate;
		}
	}

	void PQEModelRender::SetBoneSSAOId(unsigned int id)
	{
		if (!this->bone_ssao)
			this->bone_ssao = new unsigned int(id);
		else
			*this->bone_ssao = id;
	}

	void PQEModelRender::SetShapeSSAOId(unsigned int id)
	{
		if (!this->shape_ssao)
			this->shape_ssao = new unsigned int(id);
		else
			*this->shape_ssao = id;
	}

	void PQEModelRender::SetShapeWeightSSAOId(unsigned int id)
	{
		if (!this->shape_weight_ssao)
			this->shape_weight_ssao = new unsigned int(id);
		else
			*this->shape_weight_ssao = id;
	}

	PQEModelRender::~PQEModelRender()
	{
	
	}

}