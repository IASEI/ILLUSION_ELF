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
	PQE_MESH::PQE_MESH():mType(PQE_MESH_ORDINARY),mBoneIndex(0),mBoneWeight(0)
	{
	}

	PQE_MESH::~PQE_MESH()
	{
		delete[] mShapeVertexIndex;
		delete[] mFace;
		delete[] mPosition;
		delete[] mNormal;
		delete[] mCoord;
		delete[] mColor;
		delete[] mBoneIndex;
		delete[] mBoneWeight;
		delete[] aabb;

		mShapeVertexIndex=NULL;
		mFace = NULL;
		mPosition = NULL;
		mNormal = NULL;
		mCoord = NULL;
		mColor = NULL;
		mBoneIndex = NULL;
		mBoneWeight = NULL;
		aabb = NULL;

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
		if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode){cout << "ERROR::ASSIMP:: " << m_Importer.GetErrorString() << endl;return;}
		if (m_pScene)
		{
			LoadNode(m_pScene);
			LoadMesh(m_pScene);
			LoadMaterial(m_pScene);
			GenSpesicalID();
			GenTextureID();
		}
		else 
		{
			printf("Error parsing '%s': '%s'\n", path.c_str(), m_Importer.GetErrorString()); 
		}
	}

	void PQEModel::LoadMesh(const aiScene *scene)
	{
		unsigned int mesh_cout = 0;
		std::vector<PQE_MESH*> pqe_mesh;
		mModel->meshNum = scene->mNumMeshes;
		for (int i = 0; i < mModel->meshNum; i++)
		{
			aiMesh		*mMesh = scene->mMeshes[i];
			PQE_MESH	*pMesh = new PQE_MESH;
			pMesh->vertexNum = mMesh->mNumVertices;
			pMesh->faceNum = mMesh->mNumFaces*3;
			pMesh->boneNum = mMesh->mNumBones;
			pMesh->mMaterilIndex = mMesh->mMaterialIndex;

			strcpy(pMesh->mName ,mMesh->mName.C_Str());
			pMesh->mPosition = new glm::vec3[pMesh->vertexNum];
			pMesh->mNormal = new glm::vec3[pMesh->vertexNum];
			pMesh->mCoord = new glm::vec2[pMesh->vertexNum];
			pMesh->mColor = new glm::vec4[pMesh->vertexNum];
			pMesh->mBoneIndex = new glm::int4[pMesh->vertexNum];
			pMesh->mBoneWeight = new glm::vec4[pMesh->vertexNum];
			pMesh->mFace = new unsigned int[pMesh->faceNum];
			memcpy(pMesh->mPosition, mMesh->mVertices, pMesh->vertexNum*sizeof(glm::vec3));
			memcpy(pMesh->mNormal, mMesh->mNormals, pMesh->vertexNum * sizeof(glm::vec3));
			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

			for (GLuint t = 0; t < pMesh->vertexNum; t++)
			{
				const aiVector3D* pTexCoord = mMesh->HasTextureCoords(0) ? &(mMesh->mTextureCoords[0][t]) : &Zero3D;
				pMesh->mCoord[t] = glm::vec2(pTexCoord->x, pTexCoord->y);
			}

			for (unsigned int t = 0; t < mMesh->mNumFaces; t++)
			{
				pMesh->mFace[t*3] = mMesh->mFaces[t].mIndices[0];
				pMesh->mFace[t*3+1] = mMesh->mFaces[t].mIndices[1];
				pMesh->mFace[t*3+2] = mMesh->mFaces[t].mIndices[2];
			}

			for (unsigned int j = 0; j < pMesh->boneNum; j++)
			{
				unsigned int boneindex = 0;
				boneindex = mMatrixName[mMesh->mBones[j]->mName.C_Str()]->mMatrixIndex;
				memcpy(&mModel->mMatrix[boneindex].mOffset, &mMesh->mBones[j]->mOffsetMatrix, sizeof(glm::mat4));
				for (unsigned int t = 0; t < mMesh->mBones[j]->mNumWeights; t++)
				{
					unsigned int vertexIndex = mMesh->mBones[j]->mWeights[t].mVertexId;
					for (unsigned int s = 0; s < 4; s++)
					{
						if (pMesh->mBoneWeight[vertexIndex][s]==0)
						{	
							pMesh->mBoneIndex[vertexIndex][s] = boneindex;
							pMesh->mBoneWeight[vertexIndex][s] = mMesh->mBones[j]->mWeights[t].mWeight;
							break;
						}
					}
				}
			}

			pqe_mesh.push_back(pMesh);
		}
		LoadShape(mModelPath, pqe_mesh);
		mesh_cout = pqe_mesh.size();
		mModel->shapeNum = mesh_cout - mModel->meshNum;
		mModel->mMesh = new PQE_MESH*[mesh_cout];
		for (unsigned int i = 0; i < mesh_cout; i++)
			mModel->mMesh[i] = pqe_mesh[i];
	}

	void PQEModel::LoadNode(const aiScene *scen)
	{
		unsigned int index = 0;
		std::vector<PQE_MATRIX> vec;
		mModel->mRootNode = new PQE_NODE;
		LoadNodeChild(vec, scen->mRootNode, mModel->mRootNode, NULL, index);
		mModel->matrixNum = vec.size();
		mModel->mMatrix = new PQE_MATRIX[mModel->matrixNum];
		memcpy(mModel->mMatrix, &vec[0], mModel->matrixNum * sizeof(PQE_MATRIX));
	}

	void PQEModel::LoadNodeChild(std::vector<PQE_MATRIX> &vec, aiNode *node, PQE_NODE *pNode, PQE_NODE *pNodeParent, unsigned int &index)
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

		}
		for (unsigned int i = 0; i < pNode->childNum; i++)
		{
			LoadNodeChild(vec, node->mChildren[i], &pNode->mChild[i], pNode, index+=1);
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
		mesh_vao = new unsigned int[mModel->meshNum];
		mesh_ebo = new unsigned int[mModel->meshNum];
		mesh_vbo_pos = new unsigned int[mModel->meshNum];
		mesh_vbo_nor = new unsigned int[mModel->meshNum];
		mesh_vbo_coord = new unsigned int[mModel->meshNum];
		bone_weight_vbo = new unsigned int[mModel->meshNum];
		bone_index_vbo = new unsigned int[mModel->meshNum];
		shape_vbo = new unsigned int[mModel->shapeNum];

		glGenVertexArrays(mModel->meshNum, mesh_vao);
		glGenBuffers(mModel->meshNum, mesh_ebo);
		glGenBuffers(mModel->meshNum, mesh_vbo_pos);
		glGenBuffers(mModel->meshNum, mesh_vbo_nor);
		glGenBuffers(mModel->meshNum, mesh_vbo_coord);
		glGenBuffers(mModel->meshNum, bone_weight_vbo);
		glGenBuffers(mModel->meshNum, bone_index_vbo);
		glGenBuffers(mModel->shapeNum, shape_vbo);
		for (unsigned int i = 0; i < mModel->meshNum; i++)
		{
			glBindVertexArray(mesh_vao[i]);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ebo[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mModel->mMesh[i]->faceNum * sizeof(unsigned int), mModel->mMesh[i]->mFace, GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo_pos[i]);
			glBufferData(GL_ARRAY_BUFFER, mModel->mMesh[i]->vertexNum * sizeof(glm::vec3), &mModel->mMesh[i]->mPosition[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo_nor[i]);
			glBufferData(GL_ARRAY_BUFFER, mModel->mMesh[i]->vertexNum * sizeof(glm::vec3), &mModel->mMesh[i]->mNormal[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo_coord[i]);
			glBufferData(GL_ARRAY_BUFFER, mModel->mMesh[i]->vertexNum * sizeof(glm::vec2), &mModel->mMesh[i]->mCoord[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

			glBindBuffer(GL_ARRAY_BUFFER, bone_index_vbo[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::int4) *mModel->mMesh[i]->vertexNum, mModel->mMesh[i]->mBoneIndex, GL_STATIC_DRAW);
			glEnableVertexAttribArray(3);
			glVertexAttribIPointer(3, 4, GL_INT, sizeof(glm::int4), (GLvoid*)0);

			glBindBuffer(GL_ARRAY_BUFFER, bone_weight_vbo[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) *mModel->mMesh[i]->vertexNum, &mModel->mMesh[i]->mBoneWeight[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);

			if (mModel->mMesh[i]->mType == PQE_MESH::PQE_MESH_SHAPE_INDEX)
			{
				glBindBuffer(GL_ARRAY_BUFFER, shape_vbo[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * mModel->mMesh[i]->vertexNum, mModel->mMesh[i]->mShapeVertexIndex, GL_STATIC_DRAW);
				glEnableVertexAttribArray(5);
				glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(unsigned int), (GLvoid*)0);
			}
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
		if (node->shapeNum)
		{
			node->shapeVertexNum = shapeVertexCout;
			shapeVertexCout += node->shapeNum*mModel->mMesh[node->mShapeIndex[i]]->vertexNum;
		}
		for (unsigned int i = 0;i < node->shapeNum;i++)
		{
			printf("%d\n", node->mShapeIndex[i]);
			PQE_MESH *mesh = mModel->mMesh[node->mShapeIndex[i]];
			for (unsigned int j = 0;j < mesh->vertexNum;j++)
			{
				pos.push_back(mesh->mPosition[j]);
			}
		}
		for (unsigned int i = 0;i < node->childNum;i++)
		{
			GenShapeGpuData(pos, &node->mChild[i], shapeGpuIndex);
		}
	}

	void PQEModel::Render(shader *mshader)
	{
		unsigned int cout = this->mModel->meshNum;
		if (mshader)
		{
			mshader->use();
			if (bone_ssao)
			{
				glm::mat4 *matrixDate = new glm::mat4[this->mModel->matrixNum];
				ComputeBoneMatrix(this->mModel->mRootNode, glm::mat4(1.0f), matrixDate);
				std::vector<glm::mat4> mtt;
				for (int i = 0; i < mModel->matrixNum; i++)
				{
					mtt.push_back(matrixDate[i]);
				}
				mshader->setShaderStorageBufferObjectData(*this->bone_ssao,this->mModel->matrixNum*sizeof(glm::mat4),matrixDate);
				delete[] matrixDate;
			}
		}
		RenderChild(this->mModel->mRootNode, mshader);
	}

	void PQEModel::RenderChild(PQE_NODE *node, shader *mshader)
	{
		unsigned int shapeNodeVertexCount = 0;
		unsigned int shpaeVertexCout = 0;
		unsigned int weightCout = 0;
		if (node->mType == PQE_NODE::PQE_NODE_MESH|| node->mType == PQE_NODE::PQE_NODE_MESH_SHAPE)
		{
			if (mshader&&shape_ssao&&node->mType == PQE_NODE::PQE_NODE_MESH_SHAPE)
			{
				shapeNodeVertexCount = node->shapeVertexNum;
				shpaeVertexCout = mModel->mMesh[node->mShapeIndex[0]]->vertexNum;
				weightCout = node->shapeNum;
				mshader->setShaderStorageBufferObjectData(*this->shape_ssao, node->shapeNum*shape_vertex_cout * sizeof(glm::vec3), shape_vertex);
				delete[] shape_vertex;
			}
			mshader->setFloat("shape_data.shapeNodeVertexCount", shapeNodeVertexCount);
			mshader->setFloat("shape_data.vertexCout", shpaeVertexCout);
			mshader->setFloat("shape_data.weightCout", weightCout);
			for (unsigned int i = 0; i < node->meshNum; i++)
			{
				unsigned int meshIndex=node->mMeshIndex[i];
				PQE_MATERIAL *mat = &mModel->mMaterial[mModel->mMesh[meshIndex]->mMaterilIndex];
				glActiveTexture(GL_TEXTURE0);
				for (unsigned int j = 0; j < mat->textureNum; j++)
				{
					glBindTexture(GL_TEXTURE_2D, mModel->mTexture[mat->mTextureIndex[j]].mId);
				}
				glBindVertexArray(mesh_vao[meshIndex]);
				glDrawElements(GL_TRIANGLES, mModel->mMesh[meshIndex]->faceNum, GL_UNSIGNED_INT, 0);
			}
		}
		for (unsigned int i = 0; i < node->childNum; i++)
			//if (node->mType == PQE_NODE::PQE_NODE_MESH_ROOT)
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

	void PQEModel::LoadShape(std::string path, std::vector<PQE_MESH*> &shape)
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
					LoadFbxNode(mSceneFbx->GetRootNode(), shape);
				mImporterFbx->Destroy();
				mSceneFbx->Destroy();
				mSdkManagerFbx->Destroy();
				printf("读取shape完成\n");
				return;
			}
		}
		printf("读取shape失败\n");
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

	void PQEModel::LoadFbxNode(FbxNode* pNode, std::vector<PQE_MESH*> &shape)
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

	void PQEModel::LoadFbxMesh(FbxNode* pNode, std::vector<PQE_MESH*> &shape)
	{
		FbxMesh* lMesh = pNode->GetMesh();
		std::vector<glm::vec3> sd;
		
		const int lVertexCount = lMesh->GetControlPointsCount();
		const bool lHasShape = lMesh->GetShapeCount() > 0;
		if (lHasShape)
		{
			PQE_NODE *pqe_node = FindNode(pNode->GetName());
			if (!pqe_node)return;
			for (int i = 0; i < lVertexCount; i++)
			{
				sd.push_back(glm::vec3(lMesh->GetControlPoints()[i][0], lMesh->GetControlPoints()[i][1], lMesh->GetControlPoints()[i][2]));
			}
			LoadFbxShapeVertexIndex(pqe_node, shape, sd);
			LoadFbxShape(lMesh, pqe_node, shape);
			pqe_node->mType = PQE_NODE::PQE_NODE_MESH_SHAPE;
		}
	}

	void PQEModel::LoadFbxShape(FbxMesh* pMesh, PQE_NODE *pqe_node, std::vector<PQE_MESH*> &shape)
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
						std::vector<glm::vec3> vv;
						for (int i = 0; i < num; i++)
						{
							vv.push_back(glm::vec3(fbxShape->GetControlPoints()[i][0], fbxShape->GetControlPoints()[i][1], fbxShape->GetControlPoints()[i][2]));
						}
						PQE_MESH *pqe_mesh = new PQE_MESH;
						strcpy(pqe_mesh->mName , lChannel->GetName());
						pqe_mesh->mType = PQE_MESH::PQE_MESH_SHAPE;
						pqe_mesh->vertexNum = vv.size();
						pqe_mesh->mPosition = new glm::vec3[pqe_mesh->vertexNum];
						memcpy(pqe_mesh->mPosition, &vv[0], pqe_mesh->vertexNum*sizeof(glm::vec3));
						shape.push_back(pqe_mesh);
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

	void PQEModel::LoadFbxShapeVertexIndex(PQE_NODE *pqe_node, std::vector<PQE_MESH*> &shape, std::vector<glm::vec3> &vertex)
	{
		unsigned int mesh_cout = pqe_node->meshNum;
		for (int t = 0; t < mesh_cout; t++)
		{
			PQE_MESH *pqe_mesh = shape[pqe_node->mMeshIndex[t]];
			if (pqe_mesh == NULL)return;
			int vertexCout = pqe_mesh->vertexNum;
			pqe_mesh->mShapeVertexIndex = new int[vertexCout];
			pqe_mesh->mType = PQE_MESH::PQE_MESH_SHAPE_INDEX;
			for (int i = 0; i < vertexCout; i++)
			{
				vector<glm::vec3>::iterator location_index = std::find(vertex.begin(), vertex.end(), pqe_mesh->mPosition[i]);
				int index = location_index - vertex.begin();
				if (vertex.end() == location_index) index = -1;
				pqe_mesh->mShapeVertexIndex[i] = index;
			}
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

