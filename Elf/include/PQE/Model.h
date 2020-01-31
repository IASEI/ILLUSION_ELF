#ifndef __MODEL_H__
#define __MODEL_H__

#include "pde/PurpleDreamElf.h"
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <map>
#include <stb_image.h>
#include "shader.h"

class Model
{
public:
	Model()
	{
		scale.x = scale.y = scale.z = 1.0f;
		m_NumBones = 0;
	}
	~Model()
	{}

	Model(string const &path, shader *shader)
	{
		modelLoad(path);
		m_shader = shader;
	}

	Model(string const &path)
	{
		modelLoad(path);
	}

	void modelLoad(string const &path, shader *shader)
	{
		modelLoad(path);
		m_shader = shader;
	}

	void in()
	{
		for (int i = 0;i < model.mesh.size();i++)
		{
			if (model.mesh[i].vertex.size() == 0)
				continue;
			PDE::mesh_m_pde *mesh = &model.mesh[i];

			glGenVertexArrays(1, &mesh->VAO);
			glGenBuffers(1, &mesh->VBO);
			glGenBuffers(1, &mesh->EBO);
			glGenBuffers(1, &mesh->BVBO);

			glBindVertexArray(mesh->VAO);

			glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
			glBufferData(GL_ARRAY_BUFFER, mesh->vertex.size() * sizeof(PDE::vertex_m_pde), &mesh->vertex[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(GLuint), &mesh->indices[0], GL_STATIC_DRAW);


			// Vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PDE::vertex_m_pde), (GLvoid*)0);
			// Vertex Normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PDE::vertex_m_pde), (GLvoid*)offsetof(PDE::vertex_m_pde, normal));
			// Vertex color
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(PDE::vertex_m_pde), (GLvoid*)offsetof(PDE::vertex_m_pde, color));
			// Vertex Texture Coords
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(PDE::vertex_m_pde), (GLvoid*)offsetof(PDE::vertex_m_pde, coord));


			glBindBuffer(GL_ARRAY_BUFFER, mesh->BVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(PDE::vertexBoneData_m_pde) *mesh->bone.size(), &mesh->bone[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(4);
			glVertexAttribIPointer(4, 4, GL_INT, sizeof(PDE::vertexBoneData_m_pde), (GLvoid*)0);

			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(PDE::vertexBoneData_m_pde), (GLvoid*)16);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	//加载相关
	void modelLoad(string const &path)
	{
		//Assimp::Importer importer;
		m_pScene = m_Importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
		if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
		{
			cout << "ERROR::ASSIMP:: " << m_Importer.GetErrorString() << endl;
			return;
		}

		if (m_pScene)
		{
			model.globalInverse = glm::inverse(PDE::aiMatrix4x4ToMat4x4(m_pScene->mRootNode->mTransformation));
			model.global = PDE::aiMatrix4x4ToMat4x4(m_pScene->mRootNode->mTransformation);

			model.mesh.resize(m_pScene->mNumMeshes);

			for (GLuint i = 0; i <m_pScene->mNumMeshes; i++)
			{
				initMesh(i, m_pScene->mMeshes[i], model.mesh[i]);
			}
			
		}
		else
		{
			printf("Error parsing '%s': '%s'\n", path.c_str(), m_Importer.GetErrorString());
		}
	}

	void initMesh(GLuint MeshIndex, const aiMesh* paiMesh, PDE::mesh_m_pde& mesh)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const aiColor4D Zero4D(0.0f, 0.0f, 0.0f, 0.0f);
		loadBones(MeshIndex, paiMesh, mesh.bone);
		for (GLuint i = 0; i < paiMesh->mNumVertices; i++)
		{

			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
			const aiColor4D* pTexColor = paiMesh->HasVertexColors(0) ? &(paiMesh->mColors[0][i]) : &Zero4D;
			mesh.vertex.push_back(PDE::vertex_m_pde(paiMesh->mVertices[i].x, paiMesh->mVertices[i].y, paiMesh->mVertices[i].z,
				pTexColor->r, pTexColor->g, pTexColor->b, pTexColor->a,
				paiMesh->mNormals[i].x, paiMesh->mNormals[i].y, paiMesh->mNormals[i].z,
				pTexCoord->x, pTexCoord->y));
		}
		for (GLuint i = 0; i < paiMesh->mNumFaces; i++)
		{
			const aiFace& Face = paiMesh->mFaces[i];
			for (unsigned int j = 0; j < Face.mNumIndices; j++)
				mesh.indices.push_back(Face.mIndices[j]);
		}
		{
			aiMaterial* material = m_pScene->mMaterials[paiMesh->mMaterialIndex];
			// 1. diffuse maps
			vector<PDE::texture_m_pde> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			mesh.textures.insert(mesh.textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			vector<PDE::texture_m_pde> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			mesh.textures.insert(mesh.textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<PDE::texture_m_pde> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			mesh.textures.insert(mesh.textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<PDE::texture_m_pde> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			mesh.textures.insert(mesh.textures.end(), heightMaps.begin(), heightMaps.end());
		}
	}

	void loadBones(GLuint MeshIndex, const aiMesh* pMesh, vector<PDE::vertexBoneData_m_pde>& Bones)
	{
		Bones.resize(pMesh->mNumVertices);
		for (GLuint i = 0; i < pMesh->mNumBones; i++)
		{
			GLuint BoneIndex = 0;
			string BoneName(pMesh->mBones[i]->mName.data);
			if (model.boneMapping.find(BoneName) == model.boneMapping.end())
			{
				BoneIndex = m_NumBones;
				m_NumBones++;
				PDE::boneInfo_m_pde bi;
				model.boneInfo.push_back(bi);
				model.boneInfo[BoneIndex].BoneOffset = PDE::aiMatrix4x4ToMat4x4(pMesh->mBones[i]->mOffsetMatrix);
				model.boneMapping[BoneName] = BoneIndex;
			}
			else
			{
				BoneIndex = model.boneMapping[BoneName];
			}
			for (GLuint j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
			{
				GLuint VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
				Bones[VertexID].AddBoneData(BoneIndex, Weight);
			}
		}
	}

	bool InitMaterials(const aiScene* pScene, const string& Filename)
	{
		// Extract the directory part from the file name
		string::size_type SlashIndex = Filename.find_last_of("/");
		string Dir;

		if (SlashIndex == string::npos)
		{
			Dir = ".";
		}
		else if (SlashIndex == 0)
		{
			Dir = "/";
		}
		else
		{
			Dir = Filename.substr(0, SlashIndex);
		}

		bool Ret = true;

		// Initialize the materials
		for (GLuint i = 0; i < pScene->mNumMaterials; i++)
		{
			const aiMaterial* pMaterial = pScene->mMaterials[i];

			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString Path;

				if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					string p(Path.data);

					if (p.substr(0, 2) == ".\\")
					{
						p = p.substr(2, p.size() - 2);
					}

					string FullPath = Dir + "/" + p;

					//texture[i] = PDE::texture_m_pde(GL_TEXTURE_2D, Path.data);
					//texture[i].id = PDE::createTextures(Path.data);

				}
			}
		}

		return Ret;
	}

	void processNode(aiNode *node, const aiScene *scene)
	{
		for (unsigned int i = 0;i < node->mNumMeshes;i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			PDE::mesh_m_pde mesh1;
			initMesh(i, scene->mMeshes[node->mMeshes[i]], mesh1);
			model.mesh.push_back(mesh1);
			//=============
		}

		for (unsigned int i = 0;i < node->mNumChildren;i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void InitUBO()
	{

		specialId.uboIndex = glGetUniformBlockIndex(m_shader->getInitId(), "boneMat4");
		glUniformBlockBinding(m_shader->getInitId(), specialId.uboIndex, 0);
		glGenBuffers(1, &specialId.ubo);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, specialId.ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, specialId.ubo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 500, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		boneTransformStatic(transforms);
	}

	void draw()
	{

		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (int i = 0;i < model.mesh.size();i++)
		{

			for (int j = 0;j < model.mesh[i].textures.size();j++)
			{
				/*
				string number;
				string name = model.mesh[i].textures[j].type;
				if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
				number = std::to_string(specularNr++);
				else if (name == "texture_normal")
				number = std::to_string(normalNr++);
				else if (name == "texture_height")
				number = std::to_string(heightNr++);
				*/

				//glUniform1i(glGetUniformLocation(m_shader->getInitId(), (name + number).c_str()), j);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, model.mesh[i].textures[j].id);
			}
			if (model.mesh[i].VAO>0)
			{
				glBindVertexArray(model.mesh[i].VAO);
				glDrawElements(GL_TRIANGLES, model.mesh[i].indices.size(), GL_UNSIGNED_INT, 0);
			}
			
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		//glActiveTexture(GL_TEXTURE0);


	}

	void drawStatic()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, specialId.ubo);
		for (int i = 0; i < transforms.size();i++)
		{

			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*i, sizeof(glm::mat4), glm::value_ptr(transforms[i]));
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		draw();
	}

	vector<PDE::texture_m_pde> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<PDE::texture_m_pde> textures;
		int a = mat->GetTextureCount(type);
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				PDE::texture_m_pde texture;
				texture.id = PDE::TextureFromFile(str.C_Str());
				//PDE::TextureFromFile(str.C_Str());
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
		return textures;
	}

	//姿势相关
	void boneTransformStatic(vector<glm::mat4>& Transforms)
	{
		glm::mat4 Identity(1.0f);

		readNodeTransform(m_pScene->mRootNode, Identity);

		Transforms.resize(m_NumBones);

		for (GLuint i = 0; i < m_NumBones; i++)
		{
			Transforms[i] = model.boneInfo[i].FinalTransformation;
		}
	}

	void readNodeTransform(const aiNode* pNode, const glm::mat4& ParentTransform)
	{
		string NodeName(pNode->mName.data);

		glm::mat4 NodeTransformation(PDE::aiMatrix4x4ToMat4x4(pNode->mTransformation));

		glm::mat4 GlobalTransformation = NodeTransformation*ParentTransform;

		if (model.boneMapping.find(NodeName) != model.boneMapping.end())
		{
			GLuint BoneIndex = model.boneMapping[NodeName];
			model.boneInfo[BoneIndex].FinalTransformation = model.boneInfo[BoneIndex].BoneOffset* GlobalTransformation*model.globalInverse;
		}

		for (GLuint i = 0; i < pNode->mNumChildren; i++)
		{
			readNodeTransform(pNode->mChildren[i], GlobalTransformation);
		}
	}

	//动画相关
	GLuint findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (GLuint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
				return i;
			}
		}
		return 0;
	}

	GLuint findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (GLuint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}

	GLuint findScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumScalingKeys > 0);

		for (GLuint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}

	void calcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		GLuint PositionIndex = findPosition(AnimationTime, pNodeAnim);
		GLuint NextPositionIndex = (PositionIndex + 1);
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}

	void calcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1)
		{
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		GLuint RotationIndex = findRotation(AnimationTime, pNodeAnim);
		GLuint NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out = Out.Normalize();
	}

	void calcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumScalingKeys == 1)
		{
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		GLuint ScalingIndex = findScaling(AnimationTime, pNodeAnim);
		GLuint NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}

	const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const string NodeName)
	{
		for (GLuint i = 0; i < pAnimation->mNumChannels; i++)
		{
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (string(pNodeAnim->mNodeName.data) == NodeName)
			{
				return pNodeAnim;
			}
		}

		return NULL;
	}

	void readNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
	{
		string NodeName(pNode->mName.data);

		const aiAnimation* pAnimation = m_pScene->mAnimations[0];

		glm::mat4 NodeTransformation(PDE::aiMatrix4x4ToMat4x4(pNode->mTransformation));

		const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, NodeName);

		if (pNodeAnim) {

			aiVector3D Scaling;
			calcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
			glm::mat4 ScalingM(1.0f);
			ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));



			aiQuaternion RotationQ;
			calcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
			glm::mat4 RotationM = PDE::aiMatrix4x4ToMat4x4(aiMatrix4x4(RotationQ.GetMatrix()));


			aiVector3D Translation;
			calcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
			glm::mat4 TranslationM(1.0f);

			//TranslationM = glm::translate(TranslationM,glm::vec3(Translation.x, Translation.y, Translation.z));
			TranslationM[0][3] = Translation.x;
			TranslationM[1][3] = Translation.y;
			TranslationM[2][3] = Translation.z;
			NodeTransformation = ScalingM* RotationM*TranslationM;
		}

		glm::mat4 GlobalTransformation = NodeTransformation*ParentTransform;

		if (model.boneMapping.find(NodeName) != model.boneMapping.end())
		{
			GLuint BoneIndex = model.boneMapping[NodeName];
			model.boneInfo[BoneIndex].FinalTransformation = model.boneInfo[BoneIndex].BoneOffset* GlobalTransformation*model.globalInverse;
		}

		for (GLuint i = 0; i < pNode->mNumChildren; i++)
		{
			readNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
		}
	}

	void boneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms)
	{
		glm::mat4 Identity(1.0f);

		float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

		readNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

		Transforms.resize(m_NumBones);

		for (GLuint i = 0; i < m_NumBones; i++)
		{
			Transforms[i] = model.boneInfo[i].FinalTransformation;
		}
	}

	//着色器相关
	shader *getShader()
	{
		return m_shader;
	}

	void setShader(shader *shader)
	{
		m_shader = shader;
	}

	//模型位置相关
	glm::mat4 getGlobal()
	{
		return model.global;
	}

	void setGlobal(glm::mat4 mat)
	{
		model.global = mat;
	}
public:
	glm::vec3 rotate;
	glm::vec3 scale;
	glm::vec3 position;
//private:
	PDE::model_m_pde model;
	vector<PDE::vertexBoneData_m_pde> m_Bone;
	vector<PDE::texture_m_pde> textures_loaded;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
	shader* m_shader;
	GLuint m_NumBones;
	PDE::SpecialID specialId;
	vector<glm::mat4>transforms;
};

#endif

