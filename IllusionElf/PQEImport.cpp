#include "PQEImport.h"

namespace PQE
{
	PQEImport::PQEImport(std::string path)
	{
		this->mModelPath = path;
		this->mModel = new PQE_MODEL;
		std::string type = path.substr(path.find_last_of(".") + 1);
		this->mIsFbx = false;
		if (type == "ase")
		{
			LoadPQE(path);
		}
		else
		{
			if (type == "fbx") this->mIsFbx = true;
			LoadModel(path);
		}
	}

	void PQEImport::LoadModel(std::string path)
	{
		Assimp::Importer m_Importer;
		const aiScene* m_pScene = m_Importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
		if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode) { std::cout << "ERROR::ASSIMP:: " << m_Importer.GetErrorString() << std::endl; return; }
		std::cout << "start load model" << std::endl;
		if (m_pScene)
		{
			LoadMesh(m_pScene);										std::cout << "load mesh finish" << std::endl;
			LoadNode(m_pScene);										std::cout << "load node finish" << std::endl;
			LoadMaterial(m_pScene);									std::cout << "load material finish" << std::endl;
			if(this->mIsFbx)LoadShape(mModelPath, mModel->mShape);	std::cout << "load shape finish" << std::endl;
		}
		else
		{
			std::cout << "Error parsing " << path.c_str() << ":" << m_Importer.GetErrorString() << std::endl;
		}
		//delete m_pScene;
	}

	void PQEImport::LoadMesh(const aiScene *scene)
	{

		mModel->meshNum = scene->mNumMeshes;
		mModel->mMesh.resize(mModel->meshNum);
		for (int i = 0; i < mModel->meshNum; i++)
		{
			aiMesh		*mMesh = scene->mMeshes[i];
			PQE_MESH	*pMesh = &mModel->mMesh[i];
			unsigned int mNumVertices = mMesh->mNumVertices;
			pMesh->vertexNum = mNumVertices;
			pMesh->faceNum = mMesh->mNumFaces * 3;
			pMesh->boneNum = mMesh->mNumBones;
			pMesh->mMaterilIndex = mMesh->mMaterialIndex;
			strcpy(pMesh->mName, mMesh->mName.C_Str());
			pMesh->mFace.resize(pMesh->faceNum);
		}

	}

	void PQEImport::LoadNode(const aiScene *scen)
	{
		unsigned int index = 0;
		LoadNodeChild(scen, mModel->mMatrix, mModel->mNode, scen->mRootNode, NULL, index);
		mModel->matrixNum = mModel->mMatrix.size();
		mModel->nodeNum = mModel->mNode.size();
		LoadBoneChild(scen);
	}

	void PQEImport::LoadBoneChild(const aiScene *scen)
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
						it = std::find_if(this->mModel->mNode.begin(), this->mModel->mNode.end(),
							[&](PQE_NODE &obj)
						{
							if (std::string(obj.mName) == std::string(aMesh->mBones[j]->mName.C_Str()))
								return true;
						});
						boneindex = it - this->mModel->mNode.begin();
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

	void PQEImport::LoadNodeChild(const aiScene *scen, std::vector<PQE_MATRIX> &vec, std::vector<PQE_NODE>& pqe_node, aiNode *node, PQE_NODE *pNodeParent, unsigned int &index)
	{
		PQE_MATRIX mMatrix;
		//----------------------------
		PQE_NODE nodeChild;
		strcpy(nodeChild.mName, node->mName.C_Str());
		nodeChild.childNum = node->mNumChildren;
		nodeChild.meshNum = node->mNumMeshes;
		nodeChild.mMatrixIndex = index;
		nodeChild.mSelfId = pqe_node.size();
		//----------------------------
		if (nodeChild.childNum <= 0)nodeChild.mChildId.push_back(-1);
		nodeChild.mMeshIndex.resize(nodeChild.meshNum);
		memcpy(&mMatrix.mSelf, &node->mTransformation, sizeof(glm::mat4));
		mMatrix.mFinsh = mMatrix.mSelf;
		if (pNodeParent != NULL)nodeChild.mParentId = pNodeParent->mSelfId;//获取父id
		else nodeChild.mParentId = -1;
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
					nodeChild.mPosition.push_back(glm::vec4(aMesh->mVertices[j].x, aMesh->mVertices[j].y, aMesh->mVertices[j].z, 1.0f));//顶点
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
		pqe_node.push_back(nodeChild);
		for (unsigned int i = 0; i < nodeChild.childNum; i++)
		{
			LoadNodeChild(scen, vec, pqe_node, node->mChildren[i], &nodeChild, index += 1);
			pqe_node[nodeChild.mSelfId].mChildId[i] = pqe_node[pqe_node.size() - 1].mSelfId;
		}

	}

	void PQEImport::LoadMaterial(const aiScene *scene)
	{
		mModel->materialNum = scene->mNumMaterials;
		mModel->mMaterial.resize(mModel->materialNum);
		for (unsigned int i = 0; i < mModel->materialNum; i++)
		{
			aiMaterial *aiMat = scene->mMaterials[i];
			PQE_MATERIAL *pqeMat = &mModel->mMaterial[i];
			LoadMaterialTexture(aiMat, pqeMat, mModel->mTexture, i);
		}
		mModel->textureNum = mModel->mTexture.size();
	}

	void PQEImport::LoadMaterialTexture(aiMaterial *material, PQE_MATERIAL *pqematerial, std::vector<PQE_TEXTURE> &texture, unsigned int materialIndex)
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

	void PQEImport::LoadMaterialTextureChild(aiMaterial *material, PQE_MATERIAL *pqematerial, aiTextureType type, std::vector<PQE_TEXTURE> &texture, std::vector<unsigned int> &textureIndex, unsigned int materialIndex)
	{
		unsigned int cout = material->GetTextureCount(type);
		for (unsigned int s = 0; s < cout; s++)
		{
			aiString path;
			material->GetTexture(type, s, &path);
			std::vector<PQE_TEXTURE>::iterator it = std::find_if(texture.begin(), texture.end(), std::bind(&PQEImport::checkTextureRepeat, this, std::placeholders::_1, path.C_Str()));
			if (it == texture.end())
			{
				textureIndex.push_back(texture.size());
				texture.push_back(PQE_TEXTURE(PQE_TEXTURE::PQE_TEXTURE_TYPE(type), path.C_Str()));
			}
			else
			{
				textureIndex.push_back(it - texture.begin());
			}
		}
	}

	bool PQEImport::checkTextureRepeat(PQE_TEXTURE &tex, std::string path)
	{
		return tex.mPath == path;
	}

	void PQEImport::LoadShape(std::string path, std::vector<PQE_SHAPE> &shape)
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

	void PQEImport::InitFbxSdk(FbxManager *&pManager, FbxScene *&pScene)
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

	void PQEImport::LoadFbxNode(FbxNode* pNode, std::vector<PQE_SHAPE> &shape)
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

	void PQEImport::LoadFbxMesh(FbxNode* pNode, std::vector<PQE_SHAPE> &shape)
	{
		FbxMesh* lMesh = pNode->GetMesh();
		std::vector<glm::vec4> sd;

		const int lVertexCount = lMesh->GetControlPointsCount();
		const bool lHasShape = lMesh->GetShapeCount() > 0;
		if (lHasShape)
		{
			PQE_NODE *pqe_node = this->mModel->FindNode(pNode->GetName());
			if (!pqe_node)return;
			for (int i = 0; i < lVertexCount; i++)
			{
				sd.push_back(glm::vec4(lMesh->GetControlPoints()[i][0], lMesh->GetControlPoints()[i][1], lMesh->GetControlPoints()[i][2], 1.0f));
			}
			LoadFbxShapeVertexIndex(pqe_node, shape, sd);
			LoadFbxShape(lMesh, pqe_node, shape);
			pqe_node->mType = PQE_NODE::PQE_NODE_MESH_SHAPE;
		}
	}

	void PQEImport::LoadFbxShape(FbxMesh* pMesh, PQE_NODE *pqe_node, std::vector<PQE_SHAPE> &shape)
	{
		unsigned int shapeTotle = shape.size();
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
						strcpy(pqe_shape.mName, lChannel->GetName());
						pqe_shape.vertexNum = num;
						shape.push_back(pqe_shape);
						pqe_node->mShapeIndex.push_back(shapeTotle);
						shapeTotle++;
					}
				}

			}
		}
		pqe_node->shapeNum = pqe_node->mShapeIndex.size();
		this->mModel->shapeNum = shapeTotle;
	}

	void PQEImport::LoadFbxShapeVertexIndex(PQE_NODE *pqe_node, std::vector<PQE_SHAPE> &shape, std::vector<glm::vec4> &vertex)
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

	//----------------------------------------------------------------------------------------
	void PQEImport::LoadPQE(std::string path)
	{
		file = new std::fstream;
		file->open(path, ios::in | ios::binary);
		if (file->fail())
		{
			std::cout << "load failure" << endl;
			return;
		}

		this->mModel = new PQE_MODEL;
		std::cout << "start load" << endl;
		file->read((char*)&this->mModel->nodeNum, sizeof(unsigned int)); 		std::cout << "start load nodeNum finish" << endl;
		file->read((char*)&this->mModel->meshNum, sizeof(unsigned int)); 		std::cout << "start load meshNum finish" << endl;
		file->read((char*)&this->mModel->shapeNum, sizeof(unsigned int)); 		std::cout << "start load shapeNum finish" << endl;
		file->read((char*)&this->mModel->matrixNum, sizeof(unsigned int)); 		std::cout << "start load matrixNum finish" << endl;
		file->read((char*)&this->mModel->textureNum, sizeof(unsigned int)); 	std::cout << "start load textureNum finish" << endl;
		file->read((char*)&this->mModel->materialNum, sizeof(unsigned int)); 	std::cout << "start load materialNum finish" << endl;
		this->mModel->mNode.resize(this->mModel->nodeNum);
		this->mModel->mMesh.resize(this->mModel->meshNum);
		this->mModel->mShape.resize(this->mModel->shapeNum);
		this->mModel->mMatrix.resize(this->mModel->matrixNum);
		this->mModel->mTexture.resize(this->mModel->textureNum);
		this->mModel->mMaterial.resize(this->mModel->materialNum);
		LoadPQENode();		std::cout << "start load Node finish" << endl;
		LoadPQEMesh();		std::cout << "start load Mesh finish" << endl;
		LoadPQEShape();		std::cout << "start load Shape finish" << endl;
		LoadPQEMatrix();	std::cout << "start load Matrix finish" << endl;
		LoadPQETexture();	std::cout << "start load Texture finish" << endl;
		LoadPQEMaterial();	std::cout << "start load Material finish" << endl;
		file->close();
	}

	void PQEImport::LoadPQENode()
	{
		unsigned int num = this->mModel->nodeNum;
		for (unsigned int i = 0; i < num; i++)
		{
			PQE_NODE *node = &this->mModel->mNode[i];
			file->read(node->mName, 255 * sizeof(char));
			file->read((char*)&node->childNum, 1 * sizeof(unsigned int));
			file->read((char*)&node->meshNum, 1 * sizeof(unsigned int));
			file->read((char*)&node->shapeNum, 1 * sizeof(unsigned int));
			file->read((char*)&node->vertexNum, 1 * sizeof(unsigned int));
			file->read((char*)&node->shapeVertexNum, 1 * sizeof(unsigned int));
			file->read((char*)&node->spesicalId, 5 * sizeof(unsigned int));
			file->read((char*)&node->mMatrixIndex, 1 * sizeof(unsigned int));
			file->read((char*)&node->mParentId, 1 * sizeof(unsigned int));
			file->read((char*)&node->mSelfId, 1 * sizeof(unsigned int));
			if (node->childNum)node->mChildId.resize(node->childNum);
			if (node->meshNum)node->mMeshIndex.resize(node->meshNum);
			if (node->shapeNum)node->mShapeIndex.resize(node->shapeNum);
			if (node->vertexNum)node->mBoneIndex.resize(node->vertexNum);
			if (node->vertexNum)node->mBoneWeight.resize(node->vertexNum);
			if (node->vertexNum)node->mPosition.resize(node->vertexNum);
			if (node->vertexNum)node->mNormal.resize(node->vertexNum);
			if (node->vertexNum)node->mCoord.resize(node->vertexNum);

			if (node->childNum)file->read((char*)&node->mChildId[0], node->childNum * sizeof(unsigned int));
			if (node->meshNum)file->read((char*)&node->mMeshIndex[0], node->meshNum * sizeof(unsigned int));
			if (node->shapeNum)file->read((char*)&node->mShapeIndex[0], node->shapeNum * sizeof(unsigned int));
			if (node->vertexNum)file->read((char*)&node->mBoneIndex[0], node->vertexNum * sizeof(glm::int4));
			if (node->vertexNum)file->read((char*)&node->mBoneWeight[0], node->vertexNum * sizeof(glm::vec4));
			if (node->vertexNum)file->read((char*)&node->mPosition[0], node->vertexNum * sizeof(glm::vec4));
			if (node->vertexNum)file->read((char*)&node->mNormal[0], node->vertexNum * sizeof(glm::vec3));
			if (node->vertexNum)file->read((char*)&node->mCoord[0], node->vertexNum * sizeof(glm::vec2));

		}
	}

	void PQEImport::LoadPQEMatrix()
	{
		file->read((char*)&this->mModel->mMatrix[0], this->mModel->matrixNum * sizeof(PQE_MATRIX));
	}

	void PQEImport::LoadPQEMesh()
	{
		unsigned int num = this->mModel->meshNum;
		for (unsigned int i = 0; i < num; i++)
		{
			PQE_MESH *mesh = &this->mModel->mMesh[i];
			file->read(mesh->mName, 255 * sizeof(char));
			file->read((char*)&mesh->vertexNum, 1 * sizeof(unsigned int));
			file->read((char*)&mesh->faceNum, 1 * sizeof(unsigned int));
			file->read((char*)&mesh->boneNum, 1 * sizeof(unsigned int));
			file->read((char*)&mesh->mMaterilIndex, 1 * sizeof(unsigned int));
			file->read((char*)&mesh->mShapeWeight, 1 * sizeof(unsigned int));
			file->read((char*)&mesh->vao, 1 * sizeof(unsigned int));
			file->read((char*)&mesh->ebo, 1 * sizeof(unsigned int));
			mesh->mFace.resize(mesh->faceNum);
			file->read((char*)&mesh->mFace[0], mesh->faceNum * sizeof(unsigned int));
		}
	}

	void PQEImport::LoadPQEShape()
	{
		unsigned int num = this->mModel->shapeNum;
		for (unsigned int i = 0; i < num; i++)
		{
			PQE_SHAPE *shape = &this->mModel->mShape[i];
			file->read((char*)&shape->mName, 255 * sizeof(char));
			file->read((char*)&shape->vertexNum, 1 * sizeof(unsigned int));
			file->read((char*)&shape->weight, 1 * sizeof(float));
			shape->mPosition.resize(shape->vertexNum);
			file->read((char*)&shape->mPosition[0], shape->vertexNum * sizeof(glm::vec4));
		}
	}

	void PQEImport::LoadPQEMaterial()
	{
		unsigned int num = this->mModel->materialNum;
		for (unsigned int i = 0; i < num; i++)
		{
			PQE_MATERIAL *material = &this->mModel->mMaterial[i];
			file->read((char*)&material->materialNum, 1 * sizeof(unsigned int));
			file->read((char*)&material->textureNum, 1 * sizeof(unsigned int));
			material->mTextureIndex.resize(material->textureNum);
			file->read((char*)&material->mTextureIndex[0], material->textureNum * sizeof(unsigned int));
		}
	}

	void PQEImport::LoadPQETexture()
	{
		file->read((char*)&this->mModel->mTexture[0], this->mModel->textureNum * sizeof(PQE_TEXTURE));
	}

	PQE_MODEL *PQEImport::GetModel()
	{
		return this->mModel;
	}

	PQEImport::~PQEImport()
	{

	}

	//----------------------------------------------------------------------------------------
	
}
