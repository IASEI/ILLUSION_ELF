#ifndef __QDEMODEL_H__
#define __QDEMODEL_H__

#include "PQE\PurpleDreamElf.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <algorithm>
#include <iostream>
#include "fbxsdk.h"
#include "QdeRender.h"
#include "shader.h"

class QdeModel :public QdeRender
{
public:

	enum Status
	{
		UNLOADED,               // Unload file or load failure;
		MUST_BE_LOADED,         // Ready for loading file;
		MUST_BE_REFRESHED,      // Something changed and redraw needed;
		REFRESHED               // No redraw needed.
	};

	struct pqeMaterial
	{
		unsigned int type;
		unsigned int id;
	};

	struct pqeMesh
	{
		unsigned int vao, ebo;
		std::vector<pqeMaterial> material;
		std::vector<unsigned int> vertexIndex;
	};

	struct pqeMatrix
	{
		unsigned int parent,id;
		unsigned int finish;
		unsigned int self;
		unsigned int offset;
		std::vector<int> child;
		//pqeMatrix(glm::mat4 *mOffset){parent = 0;offset = mOffset;}
		pqeMatrix(){parent = id=0;}
	};

	struct pqeBlendShapeChannel
	{
		char name[255];
		float weight;
		std::vector<unsigned int> shapeIndex;
	};

	struct pqeBlendShape
	{
		std::vector<pqeBlendShapeChannel> shapeChannel;
	};

	struct pqeShapeDeformer
	{
		std::string nodeName;
		std::map<std::string,pqeBlendShapeChannel*> shapeChannel;
		std::vector<pqeBlendShape> blendShapes;
		std::vector<glm::vec4> vertexFbxOrigin;
		std::vector<std::vector<glm::vec4>>vertexFbxShape;
	};

	struct pqeNode
	{
		std::string name;									//节点名称
		glm::mat4 transformation;							//父的相对变换
		bool isShape;										//是否含有变形器
		pqeNode *parent;									//父节点
		std::vector<pqeNode*>children;						//孩子节点
		glm::int4 mesh;										//第一个int代表偏移量，第二个int代表面数量，第三个代表顶点数量
		pqeNode() { isShape = false; }
	};

	struct pqeModelData
	{
		std::vector<glm::vec4>							position;				//模型顶点
		std::vector<glm::vec3>							normal;					//模型法线
		std::vector<glm::vec2>							coord;					//模型纹理顶点

		std::vector<float>								weightShape;			//变形器权重
		std::vector<std::vector<glm::vec4>>				positionShape;			//变形器顶点

		std::vector<glm::mat4>							selfMatrix;				//初始矩阵数据
		std::vector<glm::mat4>							finshMatrix;			//变形矩阵数据
		std::vector<glm::mat4>							offestMatrix;			//偏移矩阵数据

		std::vector<std::string>						texture;				//纹理

		std::vector<glm::int4>							indexBone;				//骨骼
		std::vector<glm::vec4>							weightBone;				//骨骼

	};

	struct pqeModel
	{
		pqeNode											*rootNode;				//根节点
		map<string, GLuint>								nameToBone;				//骨骼矩阵索引
		std::vector<pqeMatrix>							matrix;					//所有矩阵结构
		std::vector<pqeMesh>							mesh;					//所有的面（一纹理划分）
		std::vector<pqeShapeDeformer>					shapeDeformer;			//变形器
		std::vector<unsigned int>						textureId;				//纹理id

		unsigned int root_vertex_vbo[3];
		unsigned int bone_vbo[2];

	};

public:
	QdeModel()
	{
		rootType = MODEL;
		name = "model";
		model = new pqeModel;
		modelData = new pqeModelData;
	}

	~QdeModel()
	{
		delete model;
	}

	void loadNode(const aiScene *Root,aiNode *node, pqeNode *mPqeNode, pqeNode *mPqeNodeParent,int *index,int *vertexOffest)
	{
		int MeshVertex = 0;
		for (int i = 0; i < node->mNumMeshes; i++){MeshVertex += Root->mMeshes[node->mMeshes[i]]->mNumVertices;}
		mPqeNode->name = node->mName.C_Str();
		mPqeNode->parent = mPqeNodeParent;
		mPqeNode->transformation = PDE::aiMatrix4x4ToMat4x4(node->mTransformation);
		*vertexOffest += MeshVertex;
		if(mPqeNodeParent!=NULL)
			mPqeNode->mesh = glm::int4(*index,node->mNumMeshes, *vertexOffest- MeshVertex,MeshVertex);
		else
			mPqeNode->mesh = glm::int4(*index, node->mNumMeshes,0, MeshVertex);
		*index += node->mNumMeshes;
		for (int i = 0; i < node->mNumChildren; i++)
		{
			pqeNode *mPqeNodeChildren=new pqeNode;
			loadNode(Root,node->mChildren[i], mPqeNodeChildren, mPqeNode, index, vertexOffest);
			mPqeNode->children.push_back(mPqeNodeChildren);
		}
	}

	void load(const std::string path)
	{
		Assimp::Importer m_Importer;
		const aiScene* m_pScene = m_Importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
		if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
		{
			cout << "ERROR::ASSIMP:: " << m_Importer.GetErrorString() << endl;
			return;
		}
		if (m_pScene)
		{
			unsigned long num = 0;
			int index = 0,index1=0,index2=0;
			model->mesh.resize(m_pScene->mNumMeshes);
			model->rootNode = new pqeNode;
			loadNode(m_pScene, m_pScene->mRootNode, model->rootNode, NULL, &index1,&index2);
			for (int i = 0;i < m_pScene->mNumMeshes;i++){num += m_pScene->mMeshes[i]->mNumVertices;}
			modelData->indexBone.resize(num);
			modelData->weightBone.resize(num);
			loadBone(m_pScene->mRootNode, index);
			boneToMatrix();
			for (GLuint i = 0; i <m_pScene->mNumMeshes; i++)
			{
				LoadMesh(i, m_pScene->mMeshes[i], m_pScene);
				vertexMatchSkeleton(i, m_pScene->mMeshes[i]);
			}
		}
		else{printf("Error parsing '%s': '%s'\n", path.c_str(), m_Importer.GetErrorString());}
		Load(path.c_str());
		changeShape();
		setVertexId();
		GenSpesicalID();
		GenTextureID();
		GenModelUBO();
	}

	void GenSpesicalID()
	{
		if (!model->mesh.size())return;
		glGenBuffers(3, model->root_vertex_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, model->root_vertex_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, modelData->position.size() * sizeof(glm::vec4), &modelData->position[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, model->root_vertex_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, modelData->position.size() * sizeof(glm::vec3), &modelData->normal[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, model->root_vertex_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, modelData->position.size() * sizeof(glm::vec2), &modelData->coord[0], GL_STATIC_DRAW);

		glGenBuffers(2, model->bone_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, model->bone_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::int4) *modelData->indexBone.size(), &modelData->indexBone[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, model->bone_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) *modelData->indexBone.size(), &modelData->weightBone[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		for (int i = 0;i < model->mesh.size();i++)
		{
			if (model->mesh[i].vertexIndex.size() == 0)continue;
			glGenVertexArrays(1, &model->mesh[i].vao);
			glGenBuffers(1, &model->mesh[i].ebo);

			glBindVertexArray(model->mesh[i].vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->mesh[i].ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->mesh[i].vertexIndex.size() * sizeof(GLuint), &model->mesh[i].vertexIndex[0], GL_STATIC_DRAW);

			// Vertex Positions
			glBindBuffer(GL_ARRAY_BUFFER, model->root_vertex_vbo[0]);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);

			// Vertex Normals
			glBindBuffer(GL_ARRAY_BUFFER, model->root_vertex_vbo[1]);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

			// Vertex Texture Coords
			glBindBuffer(GL_ARRAY_BUFFER, model->root_vertex_vbo[2]);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

			//pqeBone
			glBindBuffer(GL_ARRAY_BUFFER, model->bone_vbo[0]);
			glEnableVertexAttribArray(3);
			glVertexAttribIPointer(3, 4, GL_INT, sizeof(glm::int4), (GLvoid*)0);

			glBindBuffer(GL_ARRAY_BUFFER, model->bone_vbo[1]);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void GenTextureID()
	{
		for (int i = 0;i < modelData->texture.size();i++)
		{
			model->textureId.push_back( PDE::TextureFromFile(modelData->texture[i].c_str()));
		}
	}

	void render()
	{
		int meshCout = model->mesh.size();
		for (int i = 0;i < meshCout;i++)
		{
			glActiveTexture(GL_TEXTURE0);
			for (int j = 0; j < model->mesh[i].material.size(); j++){glBindTexture(GL_TEXTURE_2D, model->textureId[model->mesh[i].material[j].id]);}
			glBindVertexArray(model->mesh[i].vao);
			glDrawElements(GL_TRIANGLES, model->mesh[i].vertexIndex.size(), GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	virtual void updateMatrixGPU(std::string attri="boneMat4")
	{
		mshader->use();
		//mshader->setUBOs(attri, &matrix1[0], sizeof(glm::mat4)*model->matrix.size(), 0, ubo);
	}

	//设置骨骼矩阵变化
	void opationBoneMatrix(pqeMatrix *matrix, glm::mat4 ParentTransform)
	{
		std::vector<glm::mat4> &self = modelData->selfMatrix;
		std::vector<glm::mat4> &finish = modelData->finshMatrix;
		std::vector<glm::mat4> &offest = modelData->offestMatrix;
		glm::mat4 GlobalTransformation = self[matrix->self]*ParentTransform;
		finish[matrix->finish] = offest[matrix->offset] * GlobalTransformation*glm::mat4(1.0f);
		for (GLuint i = 0; i < matrix->child.size(); i++)
		{
			opationBoneMatrix(&model->matrix[matrix->child[i]], GlobalTransformation);
		}
	}

	//设置变形对象权重
	pqeBlendShapeChannel *setShapeWeight(std::string name,float weight)
	{
		if (weight > 1.0f)weight = 1.0f;
		if (weight < 0.0f)weight = 0.0f;
		for (int i = 0; i < model->shapeDeformer.size(); i++)
		{
			for (int j = 0; j < model->shapeDeformer[i].blendShapes.size(); j++)
			{
				for (int z = 0; z < model->shapeDeformer[i].blendShapes[j].shapeChannel.size();z++) 
				{
					if (name == model->shapeDeformer[i].blendShapes[j].shapeChannel[z].name)
					{
						model->shapeDeformer[i].blendShapes[j].shapeChannel[z].weight = weight;
						return &model->shapeDeformer[i].blendShapes[j].shapeChannel[z];
					}
						
				}
			}
		}
	}

	pqeBlendShapeChannel *setShapeWeight(pqeBlendShapeChannel * channel, float weight)
	{
		channel->weight += weight;
		if (channel->weight > 1.0f)channel->weight = 1.0f;
		if (channel->weight < 0.0f)channel->weight = 0.0f;
		return channel;
	}

	pqeBlendShapeChannel *findShapeChannelFor(std::string name)
	{
		for (int i = 0; i < model->shapeDeformer.size(); i++)
		{
			for (int j = 0; j < model->shapeDeformer[i].blendShapes.size(); j++)
			{
				for (int z = 0; z < model->shapeDeformer[i].blendShapes[j].shapeChannel.size(); z++)
				{
					if (name == model->shapeDeformer[i].blendShapes[j].shapeChannel[z].name)
					{
						return &model->shapeDeformer[i].blendShapes[j].shapeChannel[z];
					}

				}
			}
		}
	}

	pqeBlendShapeChannel *findShapeChannelMap(std::string name)
	{
		for (int i = 0; i < model->shapeDeformer.size(); i++)
		{
			return model->shapeDeformer[i].shapeChannel[name];
		}
	}

	void setMaterix(int index,glm::mat4 mat)
	{
		std::vector<glm::mat4> &self = modelData->selfMatrix;
		self[model->matrix[index].self] = mat;
	}

	//从mroot中获取蒙皮矩阵
	void getModelBoneMatrix()
	{
		//copyMatrixl = model->matrix;
		//matrix1.resize(model->matrix.size());
		//opationBoneMatrix(&copyMatrixl[0],glm::mat4(1.0f));
		//for (auto it = model->nameToBone.begin(); it != model->nameToBone.end(); it++)
		//{
		//	matrix1[it->second] = *(copyMatrixl[it->second].finish);
		//}
	}

	//变换骨骼矩阵
	void setModelBoneMatrixRotate(pqeMatrix *matrix, unsigned int ubo = 0,float x=0,float y=0,float z=0, std::string attri = "boneMat4")
	{
		std::vector<glm::mat4> &self = modelData->selfMatrix;
		std::vector<glm::mat4> &finish = modelData->finshMatrix;
		std::vector<glm::mat4> &offest = modelData->offestMatrix;
		glm::mat4 m = self[matrix->self];
		glm::mat4 m1 = glm::rotate(glm::rotate(glm::rotate(self[model->matrix[matrix->id].self], x, glm::vec3(1, 0, 0)), y, glm::vec3(0, 1, 0)), z, glm::vec3(0, 0, 1));
		for (int i = 0; i < 4; i++) m1[i].w = m[i].w;
		finish[matrix->finish] = m1;
	}
	
	pqeMatrix *getMatrixHead()
	{
		return &model->matrix[0];
	}

	std::vector<pqeMatrix> GetBoneMatrix()
	{
		return model->matrix;
	}

	map<string, GLuint> GetMapBone()
	{
		return model->nameToBone;
	}

	//处理形变
	void changeShape()
	{
		for (int t = 0; t < model->shapeDeformer.size(); t++)
		{
			pqeNode *node = FindNode(model->shapeDeformer[t].nodeName);
			if (node == NULL)return;
			pqeShapeDeformer * shapeDef = &model->shapeDeformer[t];
			int vertexCout = modelData->position.size();
			for (int i = node->mesh.z; i < vertexCout; i++)
			{
				vector<glm::vec4>::iterator location_index = std::find(model->shapeDeformer[t].vertexFbxOrigin.begin(), model->shapeDeformer[t].vertexFbxOrigin.end(), modelData->position[i]);
				if (model->shapeDeformer[t].vertexFbxOrigin.end() == location_index) continue;
				int index = (location_index - model->shapeDeformer[t].vertexFbxOrigin.begin());
				for (int z = 0; z < shapeDef->vertexFbxShape.size(); z++)
				{
					modelData->positionShape[z][i] = shapeDef->vertexFbxShape[z][index];
				}
			}
		}
		
	}

	void Load(std::string path)
	{
		InitializeSdkObjects(mSdkManagerFbx, mSceneFbx);

		if (mSdkManagerFbx)
		{
			// Create the importer.
			int lFileFormat = -1;
			mImporterFbx = FbxImporter::Create(mSdkManagerFbx, "");
			if (!mSdkManagerFbx->GetIOPluginRegistry()->DetectReaderFileFormat(path.c_str(), lFileFormat))
			{
				// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
				lFileFormat = mSdkManagerFbx->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
			}

			// Initialize the importer by providing a filename.
			if (mImporterFbx->Initialize(path.c_str(), lFileFormat) == true)
			{
				mStatus = MUST_BE_LOADED;
				bool lResult = mImporterFbx->Import(mSceneFbx);
				if (lResult)
					ProcessNode(mSceneFbx->GetRootNode());
			}
			else
			{
			}

		}
		else
		{
		}

		printf("读取fbx完成\n");

	}

	void InitializeSdkObjects(FbxManager *&pManager, FbxScene*&pScene)
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

		//Load plugins from the executable directory (optional)
		FbxString lPath = FbxGetApplicationDirectory();
		pManager->LoadPluginsDirectory(lPath.Buffer());

		//Create an FBX scene. This object holds most objects imported/exported from/to files.
		pScene = FbxScene::Create(pManager, "My Scene");
		if (!pScene)
		{
			FBXSDK_printf("Error: Unable to create FBX scene!\n");
			exit(1);
		}
	}

	void ProcessNode(FbxNode* pNode)
	{
		if (pNode->GetNodeAttribute())
		{
			switch (pNode->GetNodeAttribute()->GetAttributeType())
			{
			case FbxNodeAttribute::eMesh:
				ProcessMesh(pNode);
				break;
			}
		}
		for (int i = 0; i < pNode->GetChildCount(); ++i)
		{
			ProcessNode(pNode->GetChild(i));
		}
	}

	bool ProcessMesh(FbxNode* pNode)
	{
		pqeShapeDeformer sd;
		sd.nodeName = pNode->GetName();
		FbxMesh* lMesh = pNode->GetMesh();
		const int lVertexCount = lMesh->GetControlPointsCount();
		const bool lHasShape = lMesh->GetShapeCount() > 0;
		if (lHasShape)
		{
			std::string name=pNode->GetName();
			pqeNode *node=FindNode(name);
			node->isShape = true;
			for (int i = 0; i < lVertexCount; i++)
			{
				sd.vertexFbxOrigin.push_back(glm::vec4(lMesh->GetControlPoints()[i][0], lMesh->GetControlPoints()[i][1], lMesh->GetControlPoints()[i][2],1.0f));
			}

			ComputeShapeDeformation(lMesh,&sd);
			model->shapeDeformer.push_back(sd);
		}
		return true;
	}

	void ComputeShapeDeformation(FbxMesh* pMesh, pqeShapeDeformer *sd)
	{
		int lBlendShapeDeformerCount = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
		for (int lBlendShapeIndex = 0; lBlendShapeIndex < lBlendShapeDeformerCount; ++lBlendShapeIndex)
		{
			pqeBlendShape blend;
			FbxBlendShape* lBlendShape = (FbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
			int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
			blend.shapeChannel.resize(lBlendShapeChannelCount);
			for (int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; ++lChannelIndex)
			{
				FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
				std::string name = lChannel->GetName();
				pqeBlendShapeChannel ch;
				if (lChannel)
				{
					strcpy(ch.name , name.c_str());
					ch.shapeIndex.push_back(lChannelIndex);
					int lShapeCount = lChannel->GetTargetShapeCount();
					double* lFullWeights = lChannel->GetTargetShapeFullWeights();
					for (int i = 0; i < lShapeCount; i++)
					{
						FbxShape *shape= lChannel->GetTargetShape(i);
						int num = shape->GetControlPointsCount();
						std::vector<glm::vec4> vv;
						for (int i = 0; i < num; i++)
						{
							vv.push_back(glm::vec4(shape->GetControlPoints()[i][0], shape->GetControlPoints()[i][1], shape->GetControlPoints()[i][2],1.0f));
						}
						sd->vertexFbxShape.push_back(vv);
						modelData->positionShape.push_back(modelData->position);
						modelData->weightShape.push_back(0);
					}
					blend.shapeChannel[lChannelIndex] = (ch);
					sd->shapeChannel[name] = &blend.shapeChannel[lChannelIndex];
				}
				
			}
			sd->blendShapes.push_back(blend);
		}

	}

	void assimpLoad(aiNode* mRootNode)
	{
		if (mRootNode->mNumMeshes > 0)
		{
			for (int i = 0; i < mRootNode->mNumMeshes; i++)
			{
				int a = mRootNode->mMeshes[i];
				printf("%d", a);
			}
			printf("=================");
		}
		for (int i = 0; i < mRootNode->mNumChildren; i++)
		{
			assimpLoad(mRootNode->mChildren[i]);
		}
	}

	void ComputeShapeDeformation1()
	{
		std::vector<glm::vec4> vers = modelData->position;
		int vertexCount = vers.size();
		int shapeDeformerCout = model->shapeDeformer.size();
		for (int shapeDeformerIndex = 0; shapeDeformerIndex < shapeDeformerCout; shapeDeformerIndex++)
		{
			for (auto it = model->shapeDeformer[shapeDeformerIndex].shapeChannel.begin(); it != model->shapeDeformer[shapeDeformerIndex].shapeChannel.end(); it++)
			{
				if (it->second->weight <= 0)continue;
				//std::vector<glm::vec3> &vertexBuffer = modelData->positionShape[it->second->shapeIndex[0]];
				for (int j = 0; j < vertexCount; j++)
				{
					//glm::vec3 lInfluence = (vertexBuffer[j] - vers[j]) * it->second->weight;
					//vers[j] += lInfluence;
				}
			}
		}
		updateGpuVertexs(vers);
	}

	void updateGpuVertexs(std::vector<glm::vec4> &mVertex)
	{
		if (!model->mesh.size())return;
		glBindBuffer(GL_ARRAY_BUFFER, rootId.vbo);
		glBufferData(GL_ARRAY_BUFFER, mVertex.size() * sizeof(glm::vec3), &mVertex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		for (int i = 0; i < model->mesh.size(); i++)
		{
			if (model->mesh[i].vertexIndex.size() == 0)
				continue;
			glBindVertexArray(model->mesh[i].vao);
			glBindBuffer(GL_ARRAY_BUFFER, rootId.vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->mesh[i].ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->mesh[i].vertexIndex.size() * sizeof(GLuint), &model->mesh[i].vertexIndex[0], GL_STATIC_DRAW);
			// Vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	pqeNode *FindNode(std::string name)
	{
		return FindNodeChildren(name, model->rootNode);
	}

	pqeNode * FindNodeChildren(std::string name, pqeNode *children)
	{
		if (name == children->name) { return children; }
		for (int i = 0; i < children->children.size(); i++)
		{
			pqeNode *node1 = FindNodeChildren(name, children->children[i]);
			if (node1->name == name)
			{
				return node1;
			};
		}
	}
	
	void setVertexId()
	{
		for (int i = 0; i < modelData->position.size(); i++)
		{
			modelData->position[i].w = i;
		}
	}
private:
	inline int GenModelUBO()
	{
		mshader->use();
		ubo=mshader->GenUBOBuf(sizeof(glm::mat4) *model->matrix.size());
		return ubo;
	}

	inline void LoadMesh(unsigned long MeshIndex, const aiMesh* paiMesh, const aiScene *m_pScene)
	{
	
		pqeMesh *mMesh = &model->mesh[MeshIndex];
		unsigned long vertexTotal = modelData->position.size();
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const aiColor4D Zero4D(0.0f, 0.0f, 0.0f, 0.0f);
		for (GLuint i = 0; i < paiMesh->mNumVertices; i++)
		{
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
			modelData->position.push_back(glm::vec4(paiMesh->mVertices[i].x,paiMesh->mVertices[i].y,paiMesh->mVertices[i].z,1.0f));
			modelData->normal.push_back(glm::vec3(paiMesh->mNormals[i].x, paiMesh->mNormals[i].y, paiMesh->mNormals[i].z));
			modelData->coord.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
		}

		for (GLuint i = 0; i < paiMesh->mNumFaces; i++)
		{
			const aiFace& Face = paiMesh->mFaces[i];
			for (unsigned int j = 0; j < Face.mNumIndices; j++)
				mMesh->vertexIndex.push_back(vertexTotal + Face.mIndices[j]);//特殊的面
		}

		aiMaterial* pqeMaterial = m_pScene->mMaterials[paiMesh->mMaterialIndex];
		// 1. diffuse maps
		loadMaterialTextures(pqeMaterial, aiTextureType_DIFFUSE, mMesh);
		// 2. specular maps
		loadMaterialTextures(pqeMaterial, aiTextureType_SPECULAR, mMesh);
		// 3. normal maps
		loadMaterialTextures(pqeMaterial, aiTextureType_HEIGHT, mMesh);
		// 4. height maps
		loadMaterialTextures(pqeMaterial, aiTextureType_AMBIENT, mMesh);
	}

	inline int loadBone(const aiNode *pNode,int &index,int parentId=-1)
	{
		model->nameToBone[pNode->mName.C_Str()] = index;
		pqeMatrix mt;
		glm::mat4 m = PDE::aiMatrix4x4ToMat4x4(pNode->mTransformation);
		modelData->selfMatrix.push_back(m);
		modelData->offestMatrix.push_back(glm::mat4(1.0f));
		modelData->finshMatrix.push_back(m);
		mt.parent = parentId;
		mt.id = index;
		parentId = index;
		model->matrix.push_back(mt);
		for (int i = 0;i < pNode->mNumChildren;i++)
		{
			index++;
			model->matrix[parentId].child.push_back(index);
			loadBone(pNode->mChildren[i], index,parentId);
		}
		return index;
	}

	inline void boneToMatrix()
	{
		
		for (int i = 0; i < model->matrix.size(); i++)
		{
			model->matrix[i].self = i;
			model->matrix[i].offset = i;
			model->matrix[i].finish = i;

		}
	}

	inline void vertexMatchSkeleton(GLuint MeshIndex, const aiMesh* pMesh)
	{
	
		long m_NumVertex = modelData->position.size() - pMesh->mNumVertices;
		long m_NumBones = model->matrix.size();
		for (GLuint i = 0; i < pMesh->mNumBones; i++)
		{
			GLuint BoneIndex = 0;
			BoneIndex = model->nameToBone[pMesh->mBones[i]->mName.data];
			modelData->offestMatrix[BoneIndex] = PDE::aiMatrix4x4ToMat4x4(pMesh->mBones[i]->mOffsetMatrix);
			model->matrix[BoneIndex].offset = BoneIndex;
			
			for (GLuint j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
			{
				GLuint VertexID = m_NumVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
				loadBoneWeight(VertexID, BoneIndex, Weight);
			}
		}
	}

	inline void loadBoneWeight(unsigned int index,unsigned int id, float weight)
	{
		for (unsigned int i = 0; i < 4; i++) {
			if (modelData->weightBone[index][i] == 0.0) {
				modelData->indexBone[index][i] = id;
				modelData->weightBone[index][i] = weight;
				return;
			}
		}
	}

	inline void loadMaterialTextures(aiMaterial *mat, aiTextureType type,pqeMesh *mMesh)
	{
		if (mat->GetTextureCount(type) == 0){return ;}
		else
		{
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				for (int j = 0;j < modelData->texture.size();j++)if (!strcmp(modelData->texture[j].c_str(), str.C_Str())){return;}
				modelData->texture.push_back(str.C_Str());
				pqeMaterial material;
				material.id = modelData->texture.size() - 1;
				material.type = 0;
				mMesh->material.push_back(material);
			}
		}
	}

	inline void loadMatrix()
	{
		glm::mat4 identity(1.0f);
		loadMatrixChild(&model->matrix[0], identity);
	}

	inline void loadMatrixChild(pqeMatrix *pNode, const glm::mat4 ParentTransform)
	{

		std::vector<glm::mat4> &self = modelData->selfMatrix;
		std::vector<glm::mat4> &finish = modelData->finshMatrix;
		std::vector<glm::mat4> &offest = modelData->offestMatrix;
		glm::mat4 GlobalTransformation = ParentTransform* self[pNode->self];
		finish[pNode->finish] = model->rootNode->transformation* GlobalTransformation*offest[pNode->offset];
		for (GLuint i = 0; i < pNode->child.size(); i++)
		{
			loadMatrixChild(&model->matrix[pNode->child[i]], GlobalTransformation);
		}

	}

	inline void loadVertexId() 
	{
		for (int i = 0; i < modelData->position.size(); i++)
		{
			modelData->position[i].w = i;
		}
	}
public:

	mutable Status mStatus;
	FbxManager * mSdkManagerFbx;
	FbxScene * mSceneFbx;
	FbxImporter * mImporterFbx;
	PDE::SpecialID rootId, boneId,other;
	std::vector<PDE::SpecialID> meshId;
	shader *mshader;
	unsigned int ubo;
	pqeModel *model;
	pqeModelData *modelData;
};

#endif

