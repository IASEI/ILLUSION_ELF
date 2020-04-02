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
		
		delete[] aabb;

		mShapeVertexIndex=NULL;
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
			LoadShape(mModelPath, mModel->mShape);	std::cout << "load shape finish" << std::endl;
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
		mModel->mMesh.resize(mModel->meshNum);
		for (int i = 0; i < mModel->meshNum; i++)
		{
			aiMesh		*mMesh = scene->mMeshes[i];
			PQE_MESH	*pMesh = &mModel->mMesh[i];
			unsigned int mNumVertices = mMesh->mNumVertices;
			pMesh->vertexNum = mNumVertices;
			pMesh->faceNum = mMesh->mNumFaces*3;
			pMesh->boneNum = mMesh->mNumBones;
			pMesh->mMaterilIndex = mMesh->mMaterialIndex;
			strcpy(pMesh->mName ,mMesh->mName.C_Str());
			pMesh->mFace.resize(pMesh->faceNum);
			pMesh->mShapeVertexIndex = new int[mNumVertices]();
		}
		
	}

	void PQEModel::LoadNode(const aiScene *scen)
	{
		unsigned int index = 0;
		LoadNodeChild(scen, mModel->mMatrix, mModel->mNode,scen->mRootNode, NULL, index);
		mModel->matrixNum = mModel->mMatrix.size();
		mModel->nodeNum= mModel->mNode.size();
		LoadBoneChild(scen);
	}

	void PQEModel::LoadBoneChild(const aiScene *scen)
	{
		for (unsigned int i = 0; i < this->mModel->nodeNum; i++)
		{
			PQE_NODE *pNode = &this->mModel->mNode[i];
			if (pNode->meshNum != 0)
			{
				unsigned int offestIndex = 0;
				for (unsigned int i = 0; i < pNode->meshNum; i++)
				{
					aiMesh *aMesh = scen->mMeshes[pNode->mMeshIndex[i]];
					PQE_MESH *pqe_mesh = &mModel->mMesh[pNode->mMeshIndex[i]];
					//顶点与骨骼矩阵联系
					for (unsigned int j = 0; j < pqe_mesh->boneNum; j++)
					{
						unsigned int boneindex = 0;
						std::vector<PQE_NODE>::iterator it;
						it=std::find_if(this->mModel->mNode.begin(), this->mModel->mNode.end(), 
						[&](PQE_NODE &obj) 
						{
							if (std::string(obj.mName) == std::string(aMesh->mBones[j]->mName.C_Str()))
								return true;
						});
						boneindex = it- this->mModel->mNode.begin();
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
								}
							}
						}
					}

					offestIndex += aMesh->mNumVertices;
				}

			}
		}
	}

	void PQEModel::LoadNodeChild(const aiScene *scen,std::vector<PQE_MATRIX> &vec,std::vector<PQE_NODE>& pqe_node, aiNode *node, PQE_NODE *pNodeParent, unsigned int &index)
	{
		PQE_MATRIX mMatrix;
		//----------------------------
		PQE_NODE nodeChild;
		strcpy(nodeChild.mName, node->mName.C_Str());
		nodeChild.childNum = node->mNumChildren;
		nodeChild.meshNum = node->mNumMeshes;
		nodeChild.mMatrixIndex = index;
		nodeChild.mSelfId=pqe_node.size();
		//----------------------------
		if(nodeChild.childNum<=0)nodeChild.mChildId.push_back(-1);
		nodeChild.mMeshIndex.resize(nodeChild.meshNum);
		memcpy(&mMatrix.mSelf, &node->mTransformation, sizeof(glm::mat4));
		mMatrix.mFinsh = mMatrix.mSelf;
		if(pNodeParent!=NULL)nodeChild.mParentId = pNodeParent->mSelfId;//获取父id
		else nodeChild.mParentId =-1;
		vec.push_back(mMatrix);
		//mMatrixName[nodeChild.mName] = nodeChild.mSelfId;
		if (nodeChild.meshNum != 0)
		{
			nodeChild.mType = PQE_NODE::PQE_NODE_MESH;
			if (pNodeParent->meshNum == 0)
				pNodeParent->mType = PQE_NODE::PQE_NODE_MESH_ROOT;
			//获取顶点数据
			unsigned int offestIndex = 0;
			memcpy(&nodeChild.mMeshIndex[0], node->mMeshes, nodeChild.meshNum * sizeof(unsigned int));
			for (unsigned int i = 0; i < nodeChild.meshNum; i++)
			{
				aiMesh *aMesh = scen->mMeshes[nodeChild.mMeshIndex[i]];
				PQE_MESH *pqe_mesh = &mModel->mMesh[nodeChild.mMeshIndex[i]];
				for (unsigned int j = 0; j < aMesh->mNumVertices; j++)
				{
					const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
					nodeChild.mPosition.push_back(glm::vec4(aMesh->mVertices[j].x, aMesh->mVertices[j].y, aMesh->mVertices[j].z,1.0f));//顶点
					nodeChild.mNormal.push_back(glm::vec3(aMesh->mNormals[j].x, aMesh->mNormals[j].y, aMesh->mNormals[j].z));//法线
					const aiVector3D* pTexCoord = aMesh->HasTextureCoords(0) ? &(aMesh->mTextureCoords[0][j]) : &Zero3D;
					nodeChild.mCoord.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));//纹理坐标
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
			nodeChild.vertexNum = nodeChild.mPosition.size();
			nodeChild.mBoneIndex.resize(nodeChild.vertexNum);
			nodeChild.mBoneWeight.resize(nodeChild.vertexNum);
		}
		nodeChild.mChildId.resize(nodeChild.childNum);
		for (unsigned int i = 0; i < nodeChild.childNum; i++)
		{
			LoadNodeChild(scen,vec, pqe_node,node->mChildren[i],  &nodeChild, index+=1);
			nodeChild.mChildId[i]=pqe_node[pqe_node.size()-1].mSelfId;
		}
		pqe_node.push_back(nodeChild);
	}

	void PQEModel::LoadMaterial(const aiScene *scene)
	{
		mModel->materialNum = scene->mNumMaterials;
		mModel->mMaterial.resize(mModel->materialNum);
		for (unsigned int i = 0; i < mModel->materialNum; i++)
		{
			aiMaterial *aiMat = scene->mMaterials[i];
			PQE_MATERIAL *pqeMat = &mModel->mMaterial[i];
			LoadMaterialTexture(aiMat, pqeMat, mModel->mTexture,i);
		}
		mModel->textureNum = mModel->mTexture.size();
	}

	void PQEModel::LoadMaterialTexture(aiMaterial *material, PQE_MATERIAL *pqematerial, std::vector<PQE_TEXTURE> &texture, unsigned int materialIndex)
	{
		switch (0)
		{
		case aiTextureType_NONE:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_NONE, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_DIFFUSE:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_DIFFUSE, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_SPECULAR:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_SPECULAR, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_AMBIENT:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_AMBIENT, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_EMISSIVE:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_EMISSIVE, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_HEIGHT:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_HEIGHT, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_NORMALS:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_NORMALS, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_SHININESS:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_SHININESS, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_OPACITY:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_OPACITY, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_DISPLACEMENT:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_DISPLACEMENT, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_LIGHTMAP:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_LIGHTMAP, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_REFLECTION:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_REFLECTION, texture, pqematerial->mTextureIndex, materialIndex);
		case aiTextureType_UNKNOWN:
			LoadMaterialTextureChild(material, pqematerial, aiTextureType_UNKNOWN, texture, pqematerial->mTextureIndex, materialIndex);
		case _aiTextureType_Force32Bit:
			LoadMaterialTextureChild(material, pqematerial, _aiTextureType_Force32Bit, texture, pqematerial->mTextureIndex, materialIndex);
			break;
		default:
			break;
		}
		pqematerial->textureNum = pqematerial->mTextureIndex.size();
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


	void PQEModel::GenTextureID()
	{
		for (int i = 0; i < mModel->textureNum; i++)
		{
			mModel->mTexture[i].mId = PDE::TextureFromFile(mModel->mTexture[i].mPath);
		}
	}

	void PQEModel::Render(shader *mshader)
	{
		glm::mat4 *matrixDate = NULL;
		unsigned int cout = this->mModel->meshNum;
		if (!mshader)
			return;
		mshader->use();
		SetBoneSSAOId(mshader->createShaderStorageBufferObject(1, sizeof(glm::mat4)*GetModel()->matrixNum, NULL, GL_DYNAMIC_COPY));
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
		data[node->mMatrixIndex]= mModel->mMatrix[node->mMatrixIndex].mOffset * GlobalTransformation*mModel->mMatrix[this->mModel->mNode[0].mMatrixIndex].mFinsh;
		for (unsigned int i = 0; i < node->childNum; i++)
		{
			ComputeBoneMatrix(&this->mModel->mNode[node->mChildId[i]], GlobalTransformation, data);
		}
	}

	PQE_NODE *PQEModel::FindNode(std::string name)
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

	PQE_MATRIX *PQEModel::FindMatrix(std::string name)
	{
		std::vector<PQE_NODE>::iterator it;
		unsigned int boneindex = 0;
		it = std::find_if(this->mModel->mNode.begin(), this->mModel->mNode.end(),
		[&](PQE_NODE &obj)
		{
			if (std::string(obj.mName) == name)
				return true;
		});
		
		if (it== this->mModel->mNode.end())
			return NULL;
		else
			return &this->mModel->mMatrix[boneindex = it - this->mModel->mNode.begin()];
	}

	void PQEModel::LoadShape(std::string path, std::vector<PQE_SHAPE> &shape)
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

	void PQEModel::LoadFbxNode(FbxNode* pNode, std::vector<PQE_SHAPE> &shape)
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

	void PQEModel::LoadFbxMesh(FbxNode* pNode, std::vector<PQE_SHAPE> &shape)
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
		}
	}

	void PQEModel::LoadFbxShape(FbxMesh* pMesh, PQE_NODE *pqe_node, std::vector<PQE_SHAPE> &shape)
	{
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
						PQE_SHAPE pqe_shape;
						pqe_shape.mPosition.resize(num);
						for (int i = 0; i < num; i++)
						{
							pqe_shape.mPosition[i] = glm::vec4(fbxShape->GetControlPoints()[i][0], fbxShape->GetControlPoints()[i][1], fbxShape->GetControlPoints()[i][2], 1.0f);
						}
						strcpy(pqe_shape.mName , lChannel->GetName());
						pqe_shape.vertexNum = num;
						shape.push_back(pqe_shape);
						pqe_node->mShapeIndex.push_back(shapeTotle);
						shapeTotle++;
					}
				}

			}
		}
		pqe_node->shapeNum = pqe_node->mShapeIndex.size();
	}

	void PQEModel::LoadFbxShapeVertexIndex(PQE_NODE *pqe_node, std::vector<PQE_SHAPE> &shape, std::vector<glm::vec4> &vertex)
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

