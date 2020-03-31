#include "PQEModel.h"


namespace PQE
{
	/*-------------------------------------------------------------------------------------------*/
	PQE_MODEL::PQE_MODEL()
	{
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
		delete[] mShapeVertexIndex;
		delete[] mFace;
		
		delete[] aabb;

		mShapeVertexIndex=NULL;
		mFace = NULL;
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
	PQEModel::PQEModel()
	{
	}

	PQEModel::PQEModel(std::string path)
	{
		unsigned int shape_cout = 0;
		std::vector<PQE_SHAPE*> pqe_shape;
		this->mModelPath = path;
		this->mModel = new PQE_MODEL;
		Assimp::Importer m_Importer;
		const aiScene* m_pScene = m_Importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
		if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode){ std::cout << "ERROR::ASSIMP:: " << m_Importer.GetErrorString() << std::endl;return;}
		std::cout << "start load model" << std::endl;
		if (m_pScene)
		{
			LoadMesh(m_pScene);					std::cout << "load mesh finish" << std::endl;
			LoadNode(m_pScene);					std::cout << "load node finish" << std::endl;
			LoadMaterial(m_pScene);				std::cout << "load material finish" << std::endl;
			LoadShape(mModelPath, pqe_shape);	std::cout << "load shape finish" << std::endl;
			mModel->mShape = new PQE_SHAPE*[pqe_shape.size()];
			shape_cout = pqe_shape.size();
			for (unsigned int i = 0; i < shape_cout; i++)
				mModel->mShape[i] = pqe_shape[i];
			GenSpesicalID();					std::cout << "generat spesical id finish" << std::endl;
			GenTextureID();						std::cout << "generat texture id finish" << std::endl;
		}
		else 
		{
			std::cout << "Error parsing " << path.c_str() <<":"<< m_Importer.GetErrorString() << std::endl;
		}
	}

	void PQEModel::LoadMesh(const aiScene *scene)
	{
		
		mModel->meshNum = scene->mNumMeshes;
		mModel->mMesh = new PQE_MESH*[mModel->meshNum];
		for (int i = 0; i < mModel->meshNum; i++)
		{
			aiMesh		*mMesh = scene->mMeshes[i];
			PQE_MESH	*pMesh = new PQE_MESH;
			mModel->mMesh[i] = pMesh;
			unsigned int mNumVertices = mMesh->mNumVertices;
			pMesh->vertexNum = mNumVertices;
			pMesh->faceNum = mMesh->mNumFaces*3;
			pMesh->boneNum = mMesh->mNumBones;
			pMesh->mMaterilIndex = mMesh->mMaterialIndex;
			strcpy(pMesh->mName ,mMesh->mName.C_Str());
			pMesh->mFace = new unsigned int[pMesh->faceNum];
			pMesh->mShapeVertexIndex = new int[mNumVertices]();
		}
		
	}

	void PQEModel::LoadNode(const aiScene *scen)
	{
		unsigned int index = 0;
		std::vector<PQE_MATRIX> vec;
		mModel->mRootNode = new PQE_NODE;
		LoadNodeChild(scen,vec, scen->mRootNode, mModel->mRootNode, NULL, index);
		mModel->matrixNum = vec.size();
		mModel->mMatrix = new PQE_MATRIX[mModel->matrixNum];
		memcpy(mModel->mMatrix, &vec[0], mModel->matrixNum * sizeof(PQE_MATRIX));
		LoadBoneChild(scen, scen->mRootNode, mModel->mRootNode);

	}

	void PQEModel::LoadBoneChild(const aiScene *scen, aiNode *node, PQE_NODE *pNode)
	{
		if (pNode->meshNum != 0)
		{
			unsigned int offestIndex = 0;
			for (unsigned int i = 0; i < pNode->meshNum; i++)
			{
				aiMesh *aMesh = scen->mMeshes[pNode->mMeshIndex[i]];
				PQE_MESH *pqe_mesh = mModel->mMesh[pNode->mMeshIndex[i]];
				//顶点与骨骼矩阵联系
				for (unsigned int j = 0; j < pqe_mesh->boneNum; j++)
				{
					unsigned int boneindex = 0;
					boneindex = mMatrixName[aMesh->mBones[j]->mName.C_Str()]->mMatrixIndex;
					memcpy(&mModel->mMatrix[boneindex].mOffset, &aMesh->mBones[j]->mOffsetMatrix, sizeof(glm::mat4));
					for (unsigned int t = 0; t < aMesh->mBones[j]->mNumWeights; t++)
					{
						unsigned int vertexIndex = aMesh->mBones[j]->mWeights[t].mVertexId + offestIndex;
						for (unsigned int s = 0; s < 4; s++)
						{
							if (pNode->mBoneWeight[vertexIndex][s] == 0)
							{
								pNode->mBoneIndex[vertexIndex][s] = boneindex;
								pNode->mBoneWeight[vertexIndex][s] = aMesh->mBones[j]->mWeights[t].mWeight;
								break;
							}
						}
					}
				}

				offestIndex += aMesh->mNumVertices;
			}
			
		}
		for (unsigned int i = 0; i < pNode->childNum; i++)
		{
			LoadBoneChild(scen, node->mChildren[i], &pNode->mChild[i]);
		}
	}

	void PQEModel::LoadNodeChild(const aiScene *scen,std::vector<PQE_MATRIX> &vec, aiNode *node, PQE_NODE *pNode, PQE_NODE *pNodeParent, unsigned int &index)
	{
		PQE_MATRIX mMatrix;
		strcpy(pNode->mName, node->mName.C_Str());
		pNode->childNum = node->mNumChildren;
		pNode->meshNum = node->mNumMeshes;
		pNode->mMatrixIndex = index;
		if(pNode->childNum>0)pNode->mChild = new PQE_NODE[pNode->childNum];
		pNode->mMeshIndex = new unsigned int[pNode->meshNum];
		memcpy(&mMatrix.mSelf, &node->mTransformation, sizeof(glm::mat4));
		mMatrix.mFinsh = mMatrix.mSelf;
		pNode->mParent = pNodeParent;
		vec.push_back(mMatrix);
		mMatrixName[pNode->mName] = pNode;
		memcpy(pNode->mMeshIndex, node->mMeshes, pNode->meshNum * sizeof(unsigned int));
		if (pNode->meshNum != 0)
		{
			pNode->mType = PQE_NODE::PQE_NODE_MESH;
			if (pNodeParent->meshNum == 0)
				pNodeParent->mType = PQE_NODE::PQE_NODE_MESH_ROOT;
			//获取顶点数据
			unsigned int offestIndex = 0;
			std::vector<glm::vec4> position;
			std::vector<glm::vec3> normal;
			std::vector<glm::vec2> coord;
			for (unsigned int i = 0; i < pNode->meshNum; i++)
			{
				aiMesh *aMesh = scen->mMeshes[pNode->mMeshIndex[i]];
				PQE_MESH *pqe_mesh = mModel->mMesh[pNode->mMeshIndex[i]];
				for (unsigned int j = 0; j < aMesh->mNumVertices; j++)
				{
					const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
					position.push_back(glm::vec4(aMesh->mVertices[j].x, aMesh->mVertices[j].y, aMesh->mVertices[j].z,1.0f));//顶点
					normal.push_back(glm::vec3(aMesh->mNormals[j].x, aMesh->mNormals[j].y, aMesh->mNormals[j].z));//法线
					const aiVector3D* pTexCoord = aMesh->HasTextureCoords(0) ? &(aMesh->mTextureCoords[0][j]) : &Zero3D;
					coord.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));//纹理坐标
				}
				//片面的顶点索引
				for (unsigned int j = 0; j < aMesh->mNumFaces; j++)
				{
					pqe_mesh->mFace[j * 3 + 0] = aMesh->mFaces[j].mIndices[0] + offestIndex;
					pqe_mesh->mFace[j * 3 + 1] = aMesh->mFaces[j].mIndices[1] + offestIndex;
					pqe_mesh->mFace[j * 3 + 2] = aMesh->mFaces[j].mIndices[2] + offestIndex;
				}
				offestIndex += aMesh->mNumVertices;
			}
			pNode->vertexNum = position.size();
			pNode->mBoneIndex = new glm::int4[pNode->vertexNum];
			pNode->mBoneWeight = new glm::vec4[pNode->vertexNum];
			pNode->mPosition = new glm::vec4[pNode->vertexNum];
			pNode->mNormal = new glm::vec3[pNode->vertexNum];
			pNode->mCoord = new glm::vec2[pNode->vertexNum];
			memcpy(pNode->mPosition,&position[0],sizeof(glm::vec4)*position.size());
			memcpy(pNode->mNormal, &normal[0], sizeof(glm::vec3)*position.size());
			memcpy(pNode->mCoord, &coord[0], sizeof(glm::vec2)*position.size());
		}
		for (unsigned int i = 0; i < pNode->childNum; i++)
		{
			LoadNodeChild(scen,vec, node->mChildren[i], &pNode->mChild[i], pNode, index+=1);
		}
	}

	void PQEModel::LoadMaterial(const aiScene *scene)
	{
		std::vector<PQE_TEXTURE> path;
		mModel->materialNum = scene->mNumMaterials;
		mModel->mMaterial = new PQE_MATERIAL[mModel->materialNum];
		for (unsigned int i = 0; i < mModel->materialNum; i++)
		{
			aiMaterial *aiMat = scene->mMaterials[i];
			PQE_MATERIAL *pqeMat = &mModel->mMaterial[i];
			LoadMaterialTexture(aiMat, pqeMat,path,i);
		}
		mModel->textureNum = path.size();
		mModel->mTexture = new PQE_TEXTURE[mModel->textureNum];
		memcpy(mModel->mTexture, &path[0], mModel->textureNum * sizeof(PQE_TEXTURE));
	}

	void PQEModel::LoadMaterialTexture(aiMaterial *material, PQE_MATERIAL *pqematerial, std::vector<PQE_TEXTURE> &texture, unsigned int materialIndex)
	{
		std::vector<unsigned int> textureIndex;
		switch (0)
		{
		case aiTextureType_NONE:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_NONE, texture, textureIndex, materialIndex);
		case aiTextureType_DIFFUSE:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_DIFFUSE, texture, textureIndex, materialIndex);
		case aiTextureType_SPECULAR:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_SPECULAR, texture, textureIndex, materialIndex);
		case aiTextureType_AMBIENT:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_AMBIENT, texture, textureIndex, materialIndex);
		case aiTextureType_EMISSIVE:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_EMISSIVE, texture, textureIndex, materialIndex);
		case aiTextureType_HEIGHT:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_HEIGHT, texture, textureIndex, materialIndex);
		case aiTextureType_NORMALS:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_NORMALS, texture, textureIndex, materialIndex);
		case aiTextureType_SHININESS:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_SHININESS, texture, textureIndex, materialIndex);
		case aiTextureType_OPACITY:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_OPACITY, texture, textureIndex, materialIndex);
		case aiTextureType_DISPLACEMENT:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_DISPLACEMENT, texture, textureIndex, materialIndex);
		case aiTextureType_LIGHTMAP:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_LIGHTMAP, texture, textureIndex, materialIndex);
		case aiTextureType_REFLECTION:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_REFLECTION, texture, textureIndex, materialIndex);
		case aiTextureType_UNKNOWN:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_UNKNOWN, texture, textureIndex, materialIndex);
		case _aiTextureType_Force32Bit:
			LoadMaterialTextureChild(material, pqematerial, _aiTextureType_Force32Bit, texture, textureIndex, materialIndex);
			break;
		default:
			break;
		}
		pqematerial->textureNum = textureIndex.size();
		if (!pqematerial->textureNum)return;
		pqematerial->mTextureIndex = new unsigned int[pqematerial->textureNum];
		memcpy(pqematerial->mTextureIndex, &textureIndex[0], pqematerial->textureNum * sizeof(unsigned int));
	}

	void PQEModel::LoadMaterialTextureChild(aiMaterial *material, PQE_MATERIAL *pqematerial,aiTextureType type, std::vector<PQE_TEXTURE> &texture, std::vector<unsigned int> &textureIndex,unsigned int materialIndex)
	{
		unsigned int cout = material->GetTextureCount(type);
		for (unsigned int s = 0; s < cout; s++)
		{
			aiString path;
			material->GetTexture(type, s, &path);
			std::vector<PQE_TEXTURE>::iterator it = std::find_if(texture.begin(), texture.end(), std::bind(&PQEModel::checkTextureRepeat,this, std::placeholders::_1, path.C_Str()));
			if (it==texture.end())
			{
				textureIndex.push_back(texture.size());
				texture.push_back(PQE_TEXTURE(PQE_TEXTURE::PQE_TEXTURE_TYPE(type),path.C_Str()));
			}
			else
			{
				textureIndex.push_back(it-texture.begin());
			}
		}
	}

	bool PQEModel::checkTextureRepeat(PQE_TEXTURE &tex, std::string path)
	{
		return tex.mPath == path;
	}

	void PQEModel::GenSpesicalID()
	{
		GenSpesicalIdChild(mModel->mRootNode);
	}

	void PQEModel::GenSpesicalIdChild(PQE_NODE *node)
	{
		if (node->vertexNum > 0)
		{
			glGenBuffers(5, node->spesicalId);
			for (unsigned int i = 0; i < node->meshNum; i++)
			{
				PQE_MESH *pqe_mesh = mModel->mMesh[node->mMeshIndex[i]];

				glGenVertexArrays(1, &pqe_mesh->vao);
				glBindVertexArray(pqe_mesh->vao);

				glGenBuffers(1, &pqe_mesh->ebo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pqe_mesh->ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, pqe_mesh->faceNum * sizeof(unsigned int), pqe_mesh->mFace, GL_STATIC_DRAW);
	
				glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[0]);
				glBufferData(GL_ARRAY_BUFFER, node->vertexNum * sizeof(glm::vec4), node->mPosition, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);

				glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[1]);
				glBufferData(GL_ARRAY_BUFFER, node->vertexNum * sizeof(glm::vec3), node->mNormal, GL_STATIC_DRAW);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

				glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[2]);
				glBufferData(GL_ARRAY_BUFFER, node->vertexNum * sizeof(glm::vec2), node->mCoord, GL_STATIC_DRAW);
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

				glBindBuffer(GL_ARRAY_BUFFER, node->spesicalId[3]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::int4) *node->vertexNum, node->mBoneIndex, GL_STATIC_DRAW);
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
		for (unsigned int i = 0; i < node->childNum; i++)
		{
			GenSpesicalIdChild(&node->mChild[i]);
		}
	}

	void PQEModel::GenTextureID()
	{
		for (int i = 0; i < mModel->textureNum; i++)
		{
			mModel->mTexture[i].mId = PDE::TextureFromFile(mModel->mTexture[i].mPath);
		}
	}

	std::vector<glm::vec3> PQEModel::GenShapeGpuID()
	{
		unsigned int shapeVertexCout = 0;
		std::vector<glm::vec3> pos;
		GenShapeGpuData(pos,mModel->mRootNode, shapeVertexCout);
		return pos;
	}

	void PQEModel::GenShapeGpuData(std::vector<glm::vec3> &pos,PQE_NODE *node, unsigned int &shapeVertexCout)
	{
		/*
		if (node->shapeNum)
		{
			node->shapeVertexNum = shapeVertexCout;
			shapeVertexCout += node->shapeNum*mModel->mMesh[node->mShapeIndex[0]]->vertexNum;
		}
		for (unsigned int i = 0;i < node->shapeNum;i++)
		{
			printf("%d\n", node->mShapeIndex[i]);
			PQE_MESH *mesh = mModel->mMesh[node->mShapeIndex[i]];
			for (unsigned int j = 0;j < mesh->vertexNum;j++)
			{
				pos.push_back(glm::vec3(mesh->mPosition[j].x,mesh->mPosition[j].y, mesh->mPosition[j].z));
			}
		}
		for (unsigned int i = 0;i < node->childNum;i++)
		{
			GenShapeGpuData(pos, &node->mChild[i], shapeVertexCout);
		}
		*/
	}

	void PQEModel::Render(shader *mshader)
	{
		unsigned int cout = this->mModel->meshNum;
		if (!mshader)
			return;
		mshader->use();
		SetBoneSSAOId(mshader->createShaderStorageBufferObject(1, sizeof(glm::mat4)*GetModel()->matrixNum, NULL, GL_DYNAMIC_COPY));
		if (bone_ssao)
		{
			glm::mat4 *matrixDate = new glm::mat4[this->mModel->matrixNum];
			ComputeBoneMatrix(this->mModel->mRootNode, glm::mat4(1.0f), matrixDate);
			std::vector<glm::mat4> mtt;
			for (int i = 0; i < mModel->matrixNum; i++)
			{
				mtt.push_back(glm::mat4(1.0f));
			}
			mshader->setShaderStorageBufferObjectData(*this->bone_ssao,this->mModel->matrixNum*sizeof(glm::mat4), &mtt[0]);
			delete[] matrixDate;
		}
		RenderChild(this->mModel->mRootNode, mshader);
		glDeleteBuffers(1, this->bone_ssao);
		
		
	}

	void PQEModel::Render2(shader *mshader)
	{
		unsigned int cout = this->mModel->meshNum;
		if (mshader)
		{
			mshader->use();
			if (bone_ssao)
			{
				glm::mat4 *matrixDate = new glm::mat4[this->mModel->matrixNum];
				//ComputeBoneMatrix(this->mModel->mRootNode, glm::mat4(1.0f), matrixDate);
				std::vector<glm::mat4> mtt;
				for (int i = 0; i < mModel->matrixNum; i++)
				{
					mtt.push_back(matrixDate[i]);
				}
				//mshader->setShaderStorageBufferObjectData(*this->bone_ssao, this->mModel->matrixNum * sizeof(glm::mat4), matrixDate);
				delete[] matrixDate;
			}
			PQE_MESH ** mesh = this->mModel->mMesh;
			for (unsigned int meshCout = 0; meshCout < cout; meshCout++)
			{
				PQE_MATERIAL *mat = &mModel->mMaterial[mesh[meshCout]->mMaterilIndex];
				glActiveTexture(GL_TEXTURE0);
				for (unsigned int j = 0; j < mat->textureNum; j++)
				{
					glBindTexture(GL_TEXTURE_2D, mModel->mTexture[mat->mTextureIndex[j]].mId);
				}
				glBindVertexArray(mesh_vao[meshCout]);
				glDrawElements(GL_TRIANGLES, mesh[meshCout]->faceNum, GL_UNSIGNED_INT, 0);
			}
		}
		
	}

	void PQEModel::RenderChild(PQE_NODE *node, shader *mshader)
	{
		unsigned int shapeNodeVertexCount = 0;
		unsigned int shpaeVertexCount = 0;
		unsigned int weightCount = 0;
		if (node->mType == PQE_NODE::PQE_NODE_MESH|| node->mType == PQE_NODE::PQE_NODE_MESH_SHAPE)
		{
			if (mshader&&node->mType == PQE_NODE::PQE_NODE_MESH_SHAPE)
			{
				weightCount = node->shapeNum;
				float *weight = new float[weightCount];
				PQE_SHAPE *shpae = mModel->mShape[node->mShapeIndex[0]];
				shpaeVertexCount = shpae->vertexNum;
				shapeNodeVertexCount = node->shapeNum*shpaeVertexCount;
				glm::vec4 *shapeVertex = new glm::vec4[sizeof(glm::vec4)*shapeNodeVertexCount];
				SetShapeSSAOId(mshader->createShaderStorageBufferObject(2, sizeof(glm::vec4) *shapeNodeVertexCount, NULL, GL_DYNAMIC_COPY));
				SetShapeWeightSSAOId(mshader->createShaderStorageBufferObject(0, sizeof(float) * weightCount, NULL, GL_DYNAMIC_COPY));
				for (unsigned int i= 0;i < node->shapeNum;i++)
				{
					shpae = mModel->mShape[node->mShapeIndex[i]];
					weight[i] = shpae->weight;
					weight[i] = 0.0f;
					if (i == 0)
					{
						weight[i] = 0.5f;
					}

					memcpy(shapeVertex + shpaeVertexCount *i, &shpae->mPosition[0], shpaeVertexCount * sizeof(glm::vec4));
				}
				mshader->setShaderStorageBufferObjectData(*this->shape_ssao, sizeof(glm::vec4)*shapeNodeVertexCount, shapeVertex);
				mshader->setShaderStorageBufferObjectData(*this->shape_weight_ssao, weightCount* sizeof(float), weight);
				delete[] weight;
				delete[] shapeVertex;
			}
			mshader->setInt("shape_data.shapeNodeVertexCount", shapeNodeVertexCount);
			mshader->setInt("shape_data.vertexCount", shpaeVertexCount);
			mshader->setInt("shape_data.weightCount", weightCount);
			for (unsigned int i = 0; i < node->meshNum; i++)
			{
				unsigned int meshIndex=node->mMeshIndex[i];
				PQE_MESH* mesh = mModel->mMesh[node->mMeshIndex[i]];
				PQE_MATERIAL *mat = &mModel->mMaterial[mModel->mMesh[meshIndex]->mMaterilIndex];
				glActiveTexture(GL_TEXTURE0);
				for (unsigned int j = 0; j < mat->textureNum; j++)
				{
					glBindTexture(GL_TEXTURE_2D, mModel->mTexture[mat->mTextureIndex[j]].mId);
				}
				glBindVertexArray(mesh->vao);
				glDrawElements(GL_TRIANGLES, mModel->mMesh[meshIndex]->faceNum, GL_UNSIGNED_INT, 0);
			}
			glDeleteBuffers(1, this->shape_ssao);
			glDeleteBuffers(1, this->shape_weight_ssao);
		}
		for (unsigned int i = 0; i < node->childNum; i++)
				RenderChild(&node->mChild[i], mshader);
	}

	void PQEModel::SetMatrix(PQE_MATRIX *matrix, glm::mat4 *mat)
	{
		matrix->mFinsh = *mat;
	}

	void PQEModel::SetMatrix(PQE_NODE *node, glm::mat4 *mat)
	{
		mModel->mMatrix[node->mMatrixIndex].mFinsh = *mat;
	}

	void PQEModel::SetMatrix(std::string name, glm::mat4 *mat)
	{
		if (mMatrixName.size())
		{
			mModel->mMatrix[mMatrixName[name]->mMatrixIndex].mFinsh = *mat;
			return;
		}
		PQE_MATRIX *matrix = FindMatrix(name);
		matrix->mFinsh = *mat;
	}

	void PQEModel::SetBoneSSAOId(unsigned int id)
	{
		if (!this->bone_ssao)
			this->bone_ssao = new unsigned int(id);
		else
			*this->bone_ssao = id;
	}

	void PQEModel::SetShapeSSAOId(unsigned int id)
	{
		if (!this->shape_ssao)
			this->shape_ssao = new unsigned int(id);
		else
			*this->shape_ssao = id;
	}

	void PQEModel::SetShapeWeightSSAOId(unsigned int id)
	{
		if (!this->shape_weight_ssao)
			this->shape_weight_ssao = new unsigned int(id);
		else
			*this->shape_weight_ssao = id;
	}

	void PQEModel::ComputeBoneMatrix(PQE_NODE *node, glm::mat4 parent,glm::mat4 *data)
	{
		glm::mat4 GlobalTransformation = mModel->mMatrix[node->mMatrixIndex].mFinsh * parent;
		data[node->mMatrixIndex]= mModel->mMatrix[node->mMatrixIndex].mOffset * GlobalTransformation*mModel->mMatrix[mModel->mRootNode->mMatrixIndex].mFinsh;
		for (unsigned int i = 0; i < node->childNum; i++)
		{
			ComputeBoneMatrix(&node->mChild[i], GlobalTransformation, data);
		}
	}

	PQE_NODE *PQEModel::FindNode(std::string name)
	{
		if (!mMatrixName.size())
			return FindNodeChild(mModel->mRootNode, name);
		else
			return mMatrixName[name];
	}

	PQE_NODE *PQEModel::FindNodeChild(PQE_NODE *node, std::string name)
	{
		if (node->mName == name)return node;
		for (unsigned int i = 0; i < node->childNum; i++)
		{
			PQE_NODE *nodeCurrent = FindNodeChild(&node->mChild[i], name);
			if (!nodeCurrent)return nodeCurrent;

		}
		return NULL;
	}

	PQE_MATRIX *PQEModel::FindMatrix(std::string name)
	{
		if (!mMatrixName.size())
			return FindMatrixChild(mModel->mRootNode, name);
		else
		{
			PQE_NODE * node = NULL;
			node = mMatrixName[name];
			if (node)
				return NULL;
			else
				return &mModel->mMatrix[node->mMatrixIndex];
		}
	}

	PQE_MATRIX *PQEModel::FindMatrixChild(PQE_NODE *node, std::string name)
	{
		PQE_NODE *nodeCopy = FindNodeChild(node, name);
		if (nodeCopy)
			return NULL;
		else
			return &mModel->mMatrix[nodeCopy->mMatrixIndex];
	}

	void PQEModel::LoadShape(std::string path, std::vector<PQE_SHAPE*> &shape)
	{
		FbxManager * mSdkManagerFbx;
		FbxScene * mSceneFbx;
		FbxImporter * mImporterFbx;
		InitFbxSdk(mSdkManagerFbx, mSceneFbx);
		if (mSdkManagerFbx)
		{
			int lFileFormat = -1;
			mImporterFbx = FbxImporter::Create(mSdkManagerFbx, "");
			if (!mSdkManagerFbx->GetIOPluginRegistry()->DetectReaderFileFormat(path.c_str(), lFileFormat))
			{
				lFileFormat = mSdkManagerFbx->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
			}

			if (mImporterFbx->Initialize(path.c_str(), lFileFormat))
			{
				bool lResult = mImporterFbx->Import(mSceneFbx);
				if (lResult)
				{
					LoadFbxNode(mSceneFbx->GetRootNode(), shape);

				}
				mImporterFbx->Destroy();
				mSceneFbx->Destroy();
				mSdkManagerFbx->Destroy();
				return;
			}
		}
	}

	void PQEModel::InitFbxSdk(FbxManager *&pManager, FbxScene *&pScene)
	{
		pManager = FbxManager::Create();
		if (!pManager)
		{
			FBXSDK_printf("Error: Unable to create FBX Manager!\n");
			exit(1);
		}
		else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());
		FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
		pManager->SetIOSettings(ios);

		FbxString lPath = FbxGetApplicationDirectory();
		pManager->LoadPluginsDirectory(lPath.Buffer());

		pScene = FbxScene::Create(pManager, "My Scene");
		if (!pScene)
		{
			FBXSDK_printf("Error: Unable to create FBX scene!\n");
			exit(1);
		}
	}

	void PQEModel::LoadFbxNode(FbxNode* pNode, std::vector<PQE_SHAPE*> &shape)
	{
		if (pNode->GetNodeAttribute())
		{
			switch (pNode->GetNodeAttribute()->GetAttributeType())
			{
			case FbxNodeAttribute::eMesh:
				LoadFbxMesh(pNode, shape);
				break;
			}
		}
		for (int i = 0; i < pNode->GetChildCount(); ++i)
		{
			LoadFbxNode(pNode->GetChild(i), shape);
		}
	
	}

	void PQEModel::LoadFbxMesh(FbxNode* pNode, std::vector<PQE_SHAPE*> &shape)
	{
		FbxMesh* lMesh = pNode->GetMesh();
		std::vector<glm::vec4> sd;
		
		const int lVertexCount = lMesh->GetControlPointsCount();
		const bool lHasShape = lMesh->GetShapeCount() > 0;
		if (lHasShape)
		{
			PQE_NODE *pqe_node = FindNode(pNode->GetName());
			if (!pqe_node)return;
			for (int i = 0; i < lVertexCount; i++)
			{
				sd.push_back(glm::vec4(lMesh->GetControlPoints()[i][0], lMesh->GetControlPoints()[i][1], lMesh->GetControlPoints()[i][2],1.0f));
			}
			LoadFbxShapeVertexIndex(pqe_node, shape, sd);
			LoadFbxShape(lMesh, pqe_node, shape);
			pqe_node->mType = PQE_NODE::PQE_NODE_MESH_SHAPE;
			sddd = sd;
		}
	}

	void PQEModel::LoadFbxShape(FbxMesh* pMesh, PQE_NODE *pqe_node, std::vector<PQE_SHAPE*> &shape)
	{
		std::vector<unsigned int> shapeNum;
		unsigned int shapeTotle= shape.size();
		int lBlendShapeDeformerCount = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
		for (int lBlendShapeIndex = 0; lBlendShapeIndex < lBlendShapeDeformerCount; ++lBlendShapeIndex)
		{
			FbxBlendShape* lBlendShape = (FbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
			int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
			for (int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; ++lChannelIndex)
			{
				FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
				//新增mesh
				if (lChannel)
				{
					int lShapeCount = lChannel->GetTargetShapeCount();
					double* lFullWeights = lChannel->GetTargetShapeFullWeights();
					for (int i = 0; i < lShapeCount; i++)
					{
						FbxShape *fbxShape = lChannel->GetTargetShape(i);
						int num = fbxShape->GetControlPointsCount();
						PQE_SHAPE *pqe_shape = new PQE_SHAPE;
						pqe_shape->mPosition = new glm::vec4[num];
						for (int i = 0; i < num; i++)
						{
							pqe_shape->mPosition[i] = glm::vec4(fbxShape->GetControlPoints()[i][0], fbxShape->GetControlPoints()[i][1], fbxShape->GetControlPoints()[i][2], 1.0f);
						}
						strcpy(pqe_shape->mName , lChannel->GetName());
						pqe_shape->vertexNum = num;
						shape.push_back(pqe_shape);
						shapeNum.push_back(shapeTotle);
						shapeTotle++;
					}
				}

			}
		}
		pqe_node->shapeNum = shapeNum.size();
		pqe_node->mShapeIndex = new unsigned int[pqe_node->shapeNum];
		memcpy(pqe_node->mShapeIndex, &shapeNum[0], pqe_node->shapeNum*sizeof(unsigned int));
	}

	void PQEModel::LoadFbxShapeVertexIndex(PQE_NODE *pqe_node, std::vector<PQE_SHAPE*> &shape, std::vector<glm::vec4> &vertex)
	{
		int vertexCout = pqe_node->vertexNum;
		//pqe_mesh->mType = PQE_MESH::PQE_MESH_SHAPE_INDEX;
		for (int i = 0; i < vertexCout; i++)
		{
			vector<glm::vec4>::iterator location_index = std::find(vertex.begin(), vertex.end(), pqe_node->mPosition[i]);
			int index = location_index - vertex.begin();
			pqe_node->mPosition[i].w = index;
		}
	}

	PQE_MODEL *PQEModel::GetModel()
	{
		return this->mModel;
	}

	PQEModel::~PQEModel()
	{
	}

}

