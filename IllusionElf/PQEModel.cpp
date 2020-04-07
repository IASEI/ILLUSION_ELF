#include "PQEModel.h"


namespace PQE
{
	/*-------------------------------------------------------------------------------------------*/
	PQE_MODEL::PQE_MODEL()
	{
	}

	void PQE_MODEL::SetMatrix(PQE_MATRIX *matrix, glm::mat4 *mat)
	{
		matrix->mFinsh = *mat;
	}

	void PQE_MODEL::SetMatrix(PQE_NODE *node, glm::mat4 *mat)
	{
		mMatrix[node->mMatrixIndex].mFinsh = *mat;
	}

	void PQE_MODEL::SetMatrix(std::string name, glm::mat4 *mat)
	{
		unsigned int boneindex = 0;
		std::vector<PQE_NODE>::iterator it;
		it = std::find_if(mNode.begin(), mNode.end(),
			[&](PQE_NODE &obj)
		{
			if (std::string(obj.mName) == name)
				return true;
		});
		if (it == mNode.end())
			return;
		else
			mMatrix[it->mMatrixIndex].mSelf = *mat;
	}

	void PQE_MODEL::GenSpesicalID()
	{
		for (unsigned int i = 0; i < this->nodeNum; i++)
		{
			PQE_NODE *node = &this->mNode[i];
			if (node->vertexNum > 0)
			{
				glGenBuffers(5, node->spesicalId);
				for (unsigned int i = 0; i < node->meshNum; i++)
				{
					PQE_MESH *pqe_mesh = &this->mMesh[node->mMeshIndex[i]];

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

	void PQE_MODEL::GenTextureID()
	{
		for (int i = 0; i < this->textureNum; i++)
		{
			this->mTexture[i].mId = PDE::TextureFromFile(this->mTexture[i].mPath);
		}
	}

	void PQE_MODEL::ComputeBoneMatrix(PQE_NODE *node, glm::mat4 parent, glm::mat4 *data)
	{
		glm::mat4 GlobalTransformation = mMatrix[node->mMatrixIndex].mFinsh * parent;
		data[node->mMatrixIndex] = mMatrix[node->mMatrixIndex].mOffset * GlobalTransformation*mMatrix[this->mNode[0].mMatrixIndex].mFinsh;
		for (unsigned int i = 0; i < node->childNum; i++)
		{
			ComputeBoneMatrix(&this->mNode[node->mChildId[i]], GlobalTransformation, data);
		}
	}

	PQE_NODE *PQE_MODEL::FindNode(std::string name)
	{
		for (unsigned int i = 0; i < this->nodeNum; i++)
		{
			if (this->mNode[i].mName == name)
			{
				return &this->mNode[i];
			}
		}
		return NULL;
	}

	PQE_MATRIX *PQE_MODEL::FindMatrix(std::string name)
	{
		std::vector<PQE_NODE>::iterator it;
		unsigned int boneindex = 0;
		it = std::find_if(this->mNode.begin(), this->mNode.end(),
			[&](PQE_NODE &obj)
		{
			if (std::string(obj.mName) == name)
				return true;
		});

		if (it == this->mNode.end())
			return NULL;
		else
			return &this->mMatrix[boneindex = it - this->mNode.begin()];
	}

	PQE_MODEL::~PQE_MODEL()
	{
	}
	/*-------------------------------------------------------------------------------------------*/
	PQE_NODE::PQE_NODE():mType(PQE_NODE_MESH), shapeNum(0)
	{
	}

	PQE_NODE::~PQE_NODE()
	{
	}
	/*-------------------------------------------------------------------------------------------*/
	PQE_MATRIX::PQE_MATRIX()
	{
	}

	PQE_MATRIX::~PQE_MATRIX()
	{
	}
	/*-------------------------------------------------------------------------------------------*/
	PQE_MESH::PQE_MESH():mType(PQE_MESH_ORDINARY)
	{
	}

	PQE_MESH::~PQE_MESH()
	{
		
		delete[] aabb;

		aabb = NULL;

	}
	/*-------------------------------------------------------------------------------------------*/
	PQE_SHAPE::PQE_SHAPE()
	{
	}

	PQE_SHAPE::~PQE_SHAPE()
	{
	}
	/*-------------------------------------------------------------------------------------------*/
	PQE_MATERIAL::PQE_MATERIAL()
	{
	}

	PQE_MATERIAL::~PQE_MATERIAL()
	{
	}
	/*-------------------------------------------------------------------------------------------*/
	PQE_TEXTURE::PQE_TEXTURE()
	{
	}

	PQE_TEXTURE::PQE_TEXTURE(PQE_TEXTURE_TYPE type, std::string path):mType(type)
	{
		strcpy(mPath, path.c_str());
	}

	PQE_TEXTURE::~PQE_TEXTURE()
	{
	}
	/*-------------------------------------------------------------------------------------------*/
}

