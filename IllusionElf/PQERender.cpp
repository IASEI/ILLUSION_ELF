#include "PQERender.h"
namespace PQE
{
	void PQERender::GenSpesicalID()
	{
		//GenSpesicalIdChild(mModel->mRootNode);
		for (unsigned int i = 0; i < this->mModel->nodeNum; i++)
		{
			PQE_NODE *node = &this->mModel->mNode[i];
			if (node->vertexNum > 0)
			{
				glGenBuffers(5, node->spesicalId);
				for (unsigned int i = 0; i < node->meshNum; i++)
				{
					PQE_MESH *pqe_mesh = &mModel->mMesh[node->mMeshIndex[i]];

					glGenVertexArrays(1, &pqe_mesh->vao);
					glBindVertexArray(pqe_mesh->vao);

					glGenBuffers(1, &pqe_mesh->ebo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pqe_mesh->ebo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, pqe_mesh->faceNum * sizeof(unsigned int), &pqe_mesh->mFace[0], GL_STATIC_DRAW);

					glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[0]);
					glBufferData(GL_ARRAY_BUFFER, node->vertexNum * sizeof(glm::vec4), &node->mPosition[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);

					glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[1]);
					glBufferData(GL_ARRAY_BUFFER, node->vertexNum * sizeof(glm::vec3), &node->mNormal[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

					glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[2]);
					glBufferData(GL_ARRAY_BUFFER, node->vertexNum * sizeof(glm::vec2), &node->mCoord[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(2);
					glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

					glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[3]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::int4) *node->vertexNum, &node->mBoneIndex[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(3);
					glVertexAttribIPointer(3, 4, GL_INT, sizeof(glm::int4), (GLvoid*)0);

					glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[4]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) *node->vertexNum, &node->mBoneWeight[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(4);
					glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);

					//glBindBuffer(GL_ARRAY_BUFFER, shape_vbo[node->mMeshIndex[i]]);
					//glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * pqe_mesh->vertexNum, mModel->mMesh[i]->mShapeVertexIndex, GL_STATIC_DRAW);
					//glEnableVertexAttribArray(5);
					//glVertexAttribIPointer(5, 1, GL_INT, sizeof(int), (GLvoid*)0);

					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				}
			}
		}
	}


	void PQERender::GenTextureID()
	{
		for (int i = 0; i < mModel->textureNum; i++)
		{
			mModel->mTexture[i].mId = PDE::TextureFromFile(mModel->mTexture[i].mPath);
		}
	}

	void PQERender::Render(shader *mshader)
	{
		glm::mat4 *matrixDate = NULL;
		unsigned int cout = this->mModel->meshNum;
		if (!mshader)
			return;
		mshader->use();
		SetBoneSSAOId(mshader->createShaderStorageBufferObject(1, sizeof(glm::mat4)*this->mModel->matrixNum, NULL, GL_DYNAMIC_COPY));
		if (bone_ssao)
		{
			matrixDate = new glm::mat4[this->mModel->matrixNum];
			ComputeBoneMatrix(&this->mModel->mNode[0], glm::mat4(1.0f), matrixDate);
			std::vector<glm::mat4> mtt;
			for (int i = 0; i < mModel->matrixNum; i++)
			{
				mtt.push_back(glm::mat4(1.0f));
			}
			mshader->setShaderStorageBufferObjectData(*this->bone_ssao, this->mModel->matrixNum * sizeof(glm::mat4), matrixDate);
		}
		for (unsigned int i = 0; i < this->mModel->nodeNum; i++)
		{
			PQE_NODE *node = &this->mModel->mNode[i];
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
					PQE_SHAPE *shpae = &mModel->mShape[node->mShapeIndex[0]];
					shpaeVertexCount = shpae->vertexNum;
					shapeNodeVertexCount = node->shapeNum*shpaeVertexCount;
					glm::vec4 *shapeVertex = new glm::vec4[sizeof(glm::vec4)*shapeNodeVertexCount];
					SetShapeSSAOId(mshader->createShaderStorageBufferObject(2, sizeof(glm::vec4) *shapeNodeVertexCount, NULL, GL_DYNAMIC_COPY));
					SetShapeWeightSSAOId(mshader->createShaderStorageBufferObject(0, sizeof(float) * weightCount, NULL, GL_DYNAMIC_COPY));
					for (unsigned int i = 0; i < node->shapeNum; i++)
					{
						shpae = &mModel->mShape[node->mShapeIndex[i]];
						weight[i] = shpae->weight;
						weight[i] = 0.0f;
						if (i == 10)
						{
							weight[i] = 0.5f;
						}

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
					PQE_MESH* mesh = &mModel->mMesh[node->mMeshIndex[i]];
					PQE_MATERIAL *mat = &mModel->mMaterial[mModel->mMesh[meshIndex].mMaterilIndex];
					glActiveTexture(GL_TEXTURE0);
					for (unsigned int j = 0; j < mat->textureNum; j++)
					{
						glBindTexture(GL_TEXTURE_2D, mModel->mTexture[mat->mTextureIndex[j]].mId);
					}
					glBindVertexArray(mesh->vao);
					glDrawElements(GL_TRIANGLES, mModel->mMesh[meshIndex].faceNum, GL_UNSIGNED_INT, 0);
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

	void PQERender::SetMatrix(PQE_MATRIX *matrix, glm::mat4 *mat)
	{
		matrix->mFinsh = *mat;
	}

	void PQERender::SetMatrix(PQE_NODE *node, glm::mat4 *mat)
	{
		mModel->mMatrix[node->mMatrixIndex].mFinsh = *mat;
	}

	void PQERender::SetMatrix(std::string name, glm::mat4 *mat)
	{
		unsigned int boneindex = 0;
		std::vector<PQE_NODE>::iterator it;
		it = std::find_if(this->mModel->mNode.begin(), this->mModel->mNode.end(),
			[&](PQE_NODE &obj)
		{
			if (std::string(obj.mName) == name)
				return true;
		});
		if (it == this->mModel->mNode.end())
			return;
		else
			this->mModel->mMatrix[it->mMatrixIndex].mSelf = *mat;
	}

	void PQERender::SetBoneSSAOId(unsigned int id)
	{
		if (!this->bone_ssao)
			this->bone_ssao = new unsigned int(id);
		else
			*this->bone_ssao = id;
	}

	void PQERender::SetShapeSSAOId(unsigned int id)
	{
		if (!this->shape_ssao)
			this->shape_ssao = new unsigned int(id);
		else
			*this->shape_ssao = id;
	}

	void PQERender::SetShapeWeightSSAOId(unsigned int id)
	{
		if (!this->shape_weight_ssao)
			this->shape_weight_ssao = new unsigned int(id);
		else
			*this->shape_weight_ssao = id;
	}

	void PQERender::ComputeBoneMatrix(PQE_NODE *node, glm::mat4 parent, glm::mat4 *data)
	{
		glm::mat4 GlobalTransformation = mModel->mMatrix[node->mMatrixIndex].mFinsh * parent;
		data[node->mMatrixIndex] = mModel->mMatrix[node->mMatrixIndex].mOffset * GlobalTransformation*mModel->mMatrix[this->mModel->mNode[0].mMatrixIndex].mFinsh;
		for (unsigned int i = 0; i < node->childNum; i++)
		{
			ComputeBoneMatrix(&this->mModel->mNode[node->mChildId[i]], GlobalTransformation, data);
		}
	}

	PQE_NODE *PQERender::FindNode(std::string name)
	{
		for (unsigned int i = 0; i < this->mModel->nodeNum; i++)
		{
			if (this->mModel->mNode[i].mName == name)
			{
				return &this->mModel->mNode[i];
			}
		}
		return NULL;
	}

	PQE_MATRIX *PQERender::FindMatrix(std::string name)
	{
		std::vector<PQE_NODE>::iterator it;
		unsigned int boneindex = 0;
		it = std::find_if(this->mModel->mNode.begin(), this->mModel->mNode.end(),
			[&](PQE_NODE &obj)
		{
			if (std::string(obj.mName) == name)
				return true;
		});

		if (it == this->mModel->mNode.end())
			return NULL;
		else
			return &this->mModel->mMatrix[boneindex = it - this->mModel->mNode.begin()];
	}

}