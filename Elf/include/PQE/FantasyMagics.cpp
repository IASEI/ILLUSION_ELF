#include "FantasyMagics.h"
#include "PurpleDreamElf.h"
#include <learnopengl/shader.h>
namespace FM
{

	//ModelInput_PDE======================================================================

	ModelInput_PDE::ModelInput_PDE()
	{
		textureOrNormalEBO = true;
		isGenCoordFreeImageOrBit = true;
		BOOLS = true;
		j = 0;

	}

	ModelInput_PDE::~ModelInput_PDE()
	{

	}

	PDE::root_m_pde *ModelInput_PDE::getModelRoot()
	{
		return &modelRoot;
	}

	//导入信息分辨1
	void ModelInput_PDE::importChoose(const char *channge, PDE::model_m_pde *models, FILE *fp)
	{

		if (!strcmp(channge, ("PDE::root_m_pde")))
		{
		}
		else if (!strcmp(channge, ("vertex_m_pde")))
		{
			importVertex(models, fp);
		}
		else if (!strcmp(channge, ("model_m_pde")))
		{
		}
		else if (!strcmp(channge, ("modifier_m_pde")))
		{
			importModifier(models, fp);
		}
		else if (!strcmp(channge, ("mapChannel_m_pde")))
		{
			importMapChannel(models, fp);
		}
		else if (!strcmp(channge, ("face_m_pde")))
		{
			importMesh(models, fp);
		}
		else if (!strcmp(channge, ("model_m_imformation")))
		{
			importModelName(models, fp);
		}
		else
		{
			MessageBox(NULL, L"error", L"", MB_OK);
			exit(0);
		}
	}

	//导入信息分辨2
	void ModelInput_PDE::importChoose(const char *channge, FILE *fp)
	{

		if (!strcmp(channge, ("Begin_ModelRoot")))
		{
			importModelRoot(fp);
		}

		else if (!strcmp(channge, ("Begin_Model")))
		{
			importModel(&modelRoot, fp);
		}

		else if (!strcmp(channge, ("Begin_Material")))
		{
			importMaterial(fp);
		}
	}

	//导入模型信息
	bool ModelInput_PDE::importFile(const char *filename)
	{
		fp = fopen(filename, ("r"));
		if (fp != NULL)
		{
			fscanf(fp, ("%s\n"), change);
			if (strcmp(change,"BEGIN_PDE")==0)
			{

				while (1)
				{
					fscanf(fp, ("%s\n"), change);
					if (strcmp(change, "END_PDE") == 0)
					{
						genTexxtureID();
						generationAllMeshNormal();
						return true;

					}
					else if ((strcmp(change, "Begin_ModelRoot") == 0) || (strcmp(change, "Begin_Model") == 0) || (strcmp(change, "Begin_Material") == 0))
					{
						importChoose(change, fp);
					}
					else
					{
						MessageBox(NULL, L"PDE文件损坏", L"error", MB_OK);
						return false;
					}
				}	
			}

			MessageBox(NULL, L"PDE文件损坏或不是PDE文件", L"error", MB_OK);
			fclose(fp);
			return false;	
		}

		
		fclose(fp);
		MessageBox(NULL, L"PDE文件损坏或路径出错\n", L"error", MB_OK);
		return false;
	}

	//导入总的数据
	void ModelInput_PDE::importModelRoot(FILE *fp)
	{
		int modelNum, materialNum;
		modelRoot;
		fscanf(fp, ("\t%s\n"), buffer);
		fscanf(fp, ("\t\tmodelNum:%d,materialNum:%d\n"), &modelNum, &materialNum);
		fscanf(fp, ("%s\n"), change);
	}

	//导入单个模型信息（中转站）
	void ModelInput_PDE::importModel(PDE::root_m_pde *modelRoot, FILE *fp)
	{

		
		PDE::model_m_pde model;
		importModelInformation(&model, fp);
		modelRoot->modelList.push_back(model);

	}

	//导入单个模型头信息
	void ModelInput_PDE::importModelName(PDE::model_m_pde *models, FILE *fp)
	{

		fscanf(fp, ("\t\tmodelName:%s\n"), models->modelName);
		fscanf(fp, ("\t\tmodelType:%s\n"), models->modelType);
		fscanf(fp, ("\t\tmaterialIndex:%d\n"), &models->materialIndex);
		fscanf(fp, ("\t\tparentId:%d,nodeId:%d\n"),&models->parentId, &models->nodeId);
		fscanf(fp, ("\t\t%d,%d,%d\n"), &models->R, &models->G, &models->B);
		fscanf(fp, ("\t\t%f,%f,%f\n"), &models->translation[0], &models->translation[1], &models->translation[2]);
		fscanf(fp, ("\t\t%f,%f,%f,%f\n"), &models->rotation[0], &models->rotation[1], &models->rotation[2], &models->rotation[3]);
		fscanf(fp, ("\t\t%f,%f,%f,%f,%f,%f,%f\n"), &models->scale[0], &models->scale[1], &models->scale[2], &models->scale[3], &models->scale[4], &models->scale[5], &models->scale[6]);
		fscanf(fp, ("\t\tgroup:%d\n"), &models->isGroup);
	}

	//导入单个模型全部信息
	void ModelInput_PDE::importModelInformation(PDE::model_m_pde *models, FILE *fp)
	{


		while (1)
		{
			fscanf(fp, ("%s\n"), change);
			if (strcmp(change, "End_Model") == 0)
				break;
			if (strcmp(change, "Begin") != 0)
				return;

			fscanf(fp, ("\t%s\n"), buffer);
			importChoose(buffer, models, fp);

			fscanf(fp, ("%s\n"), change);

			if (strcmp(change, "End") != 0)
				return;

		}


	}

	//导入通道
	void ModelInput_PDE::importMapChannel(PDE::model_m_pde *models, FILE *fp)
	{
		TCHAR type[20];
		int mapChannelNum, b;
		/*
		if (!strcmp(buffer, ("mapChannel_pde")))
		{
			
			fscanf(fp, ("\t\tmapChannelNum:%d\n"), &mapChannelNum);
			for (int mapChannel_i = 0; mapChannel_i < mapChannelNum; mapChannel_i++)
			{
				PDE::mapChannel_m_pde mapChannel;
				fscanf(fp, ("\t\ttype:%s\n"), type);
				fscanf(fp, ("\t\tid:%d,mapVertNum:%d,mapfaceNum:%d\n"), &mapChannel.id, &mapChannel.mapVertNum, &mapChannel.mapfaceNum);
				//mapChannel.type = (TCHAR *)malloc(lstrlen(type)*sizeof(TCHAR));
				wcscpy(mapChannel.type, type);

				fscanf(fp, ("\t\t%s\n"), buffer);
				for (int mapVert_i = 0; mapVert_i < mapChannel.mapVertNum; mapVert_i++)
				{
					mapVert_pde mapVert;
					fscanf(fp, ("\t\tx,%f,y:%f,z:%f\n"), &mapVert.x, &mapVert.y, &mapVert.z);
					mapChannel.mapVert.push_back(mapVert);
				}
				fscanf(fp, ("\t\t%s\n"), buffer);
				for (int mapFace_i = 0; mapFace_i < mapChannel.mapfaceNum; mapFace_i++)
				{
					mapFace_pde mapFace ;
					fscanf(fp, ("\t\tx:%f,y:%f,z:%f\n"), &mapFace.x, &mapFace.y, &mapFace.z);
					mapChannel.mapFace.push_back(mapFace);
				}
				models->mapChannel.push_back(mapChannel);
			}


		}*/
	}


	//导入顶点
	void ModelInput_PDE::importVertex(PDE::model_m_pde *models, FILE *fp)
	{
		int vertexNum = 0;
		fscanf(fp, ("\t\tvertexNum:%d\n"), &vertexNum);
		models->vertex.reserve(vertexNum);
		for (int vertex_i = 0; vertex_i < vertexNum; vertex_i++)
		{
			PDE::vertex_m_pde vertex;
			fscanf(fp, ("\t\tx:%f,y:%f,z:%f,x:%f,y:%f,z:%f,coord_x:%f,coord_y:%f\n"), &vertex.position.x, &vertex.position.y, &vertex.position.z, &vertex.normal.x, &vertex.normal.y, &vertex.normal.z, &vertex.coord.x, &vertex.coord.y );
			models->vertex.push_back(vertex);
		}

	}


	//导入面的信息
	void ModelInput_PDE::importMesh(PDE::model_m_pde *models, FILE *fp)
	{
		int faceNum = 0, index0 = -1;
		float index1 = -1, index2 = -1, index3 = -1;
		PDE::mesh_m_pde mesh;
		bool have = false;
		PDE::mesh_m_pde *meshIndex= &mesh;

		if (strcmp(buffer, "face_m_pde")==0)
		{
			fscanf(fp, "\t\tfaceNum:%d\n", &faceNum);
			for (int mesh_i = 0; mesh_i < faceNum; mesh_i++)
			{
				fwscanf(fp, _T("\t\tmatID:%d\n"), &index0);
				fwscanf(fp, _T("\t\tv[0]:%f,v[1]:%f,v[2]:%f\n"),&index1, &index2, &index3);
				for (int j = 0;j < models->mesh.size();j++)
				{
					if (models->mesh[j].faceMatrialIndex == index0)
					{
						have = true;
						meshIndex = &models->mesh[j];
						break;
					}
				}

				if ((meshIndex->faceMatrialIndex == index0)&&(have))
				{
					have = false;
					meshIndex->indices.push_back(index1);
					meshIndex->indices.push_back(index2);
					meshIndex->indices.push_back(index3);
				}
				else
				{
					vector<GLuint>().swap(mesh.indices);
					mesh.faceMatrialIndex = index0;
					mesh.indices.push_back(index1);
					mesh.indices.push_back(index2);
					mesh.indices.push_back(index3);
					models->mesh.push_back(mesh);
					
				}
			}
		}
	}

	//导入灯光
	void ModelInput_PDE::importLight(PDE::model_m_pde *models, FILE *fp)
	{
		/*
		char lightType[20];
		//fscanf(fp, ("\t\t%s\n"), buffer);
		if (!strcmp(buffer, ("light_m_pde")))
		{
			
			models->light = mod.model.light;
			fscanf(fp, ("\t\tlightType:%s\n"), lightType);
			fscanf(fp, ("\t\ttargetId:%d,R:%f,G:%f,B:%f,multiplier:%f,aspectRatio:%f,end:%f,start:%f,fallOff:%f\n"),
				&models->light[0].targetId, &models->light[0].R, &models->light[0].G, &models->light[0].B,
				&models->light[0].multiplier, &models->light[0].aspectRatio, &models->light[0].end, &models->light[0].start,
				&models->light[0].fallOff);
			//models->light[0].type = (TCHAR*)malloc(lstrlen(lightType)*sizeof(TCHAR));
			wcscpy(models->light[0].type, lightType);
		}
		*/
	}

	//导入修改器
	void ModelInput_PDE::importModifier(PDE::model_m_pde *models, FILE *fp)
	{

		int modifierNum = 0, skinVertnum=0,boneNum=0;
		int isSkin = 0;
		char name[255];
		fscanf(fp, ("\t\tmodifierNum:%d\n"), &modifierNum);
		for (int modeifier_i = 0; modeifier_i < modifierNum; modeifier_i++)
		{
				
			PDE::modifier_m_pde modifier;

			fscanf(fp, ("\t\tisSkin:%d\n"), &isSkin);
			strcpy(modifier.modName,"none");
			
			if (isSkin)
			{
				modifier.isSkin = true;
				fscanf(fp, ("\t\tskinType:%s\n"), modifier.skinType);
				fscanf(fp, ("\t\tskinVertnum:%d\n"), &skinVertnum);

				for (int skinVert_i = 0; skinVert_i < skinVertnum; skinVert_i++)
				{

					PDE::skinVert_m_pde skinVert;
					fscanf(fp, ("\t\tskinVertType:%s\n"), skinVert.skinVertType);
					fscanf(fp, ("\t\tvertexId:%d,Num:%d\n"), &skinVert.vertexId, &boneNum);

						
					for (int bonei = 0; bonei < boneNum; bonei++)
					{
						PDE::boneAndweight_m_pde boneAndweight;
						fscanf(fp, ("\t\tboneId:%d,weight:%f\n"), &boneAndweight.boneId, &boneAndweight.weight);
						skinVert.bone.push_back(boneAndweight);
							
					}
					modifier.skinVert.push_back(skinVert);
				}
			}
			printf("modeifier_i%d\n", modeifier_i);
			models->modifier.push_back(modifier);
		}
		
	}

	//导入材质
	void ModelInput_PDE::importMaterial(FILE *fp)
	{
		int materialNum = 0;
		while(1)
		{
			
			PDE::material_m_pde material;
			int isMultiType = 0, subMaterialNum = 0, textureNum = 0;
			fscanf(fp, "\%s", change);
			if (strcmp(change, "End_Material") == 0)
				return;
			if (strcmp(change, "Begin") != 0)
				return;

			fscanf(fp, ("\t\tisMultiType:%d\n"), &material.IsMultiType);
			if (material.IsMultiType)
			{

				fscanf(fp, ("\t\tmaterialName:%s\n"), material.materialName);
				fscanf(fp, ("\t\tclassName:%s\n"), material.className);
				fscanf(fp, ("\t\tmaterialIndex:%d,subMaterialNum:%d\n"), &material.materialIndex, &subMaterialNum);
				for (int material_child = 0; material_child < subMaterialNum; material_child++)
				{

					PDE::material_m_pde materialChild;
					importMaterialChild(&materialChild, fp);
					material.materialChild.push_back(materialChild);
				}

			}
			else
			{
				fscanf(fp, ("\t\tmaterialName:%s\n"), material.materialName);
				fscanf(fp, ("\t\tclassName:%s\n"), material.className);
				fscanf(fp, ("\t\tmaterialIndex:%d,materialId:%d,textureNum:%d\n"), &material.materialIndex, &material.materialId, &textureNum);
				fscanf(fp, ("\t\tdiffuse[0]:%f,diffuse[1]:%f,diffuse[2]:%f\n"), &material.diffuse[0], &material.diffuse[1], &material.diffuse[2]);
				fscanf(fp, ("\t\tambient[0]:%f,ambient[1]:%f,ambient[2]:%f\n"), &material.ambient[0], &material.ambient[1], &material.ambient[2]);
				fscanf(fp, ("\t\tspecular[0]:%f,specular[1]:%f,specular[2]:%f\n"), &material.specular[0], &material.specular[1], &material.specular[2]);
				fscanf(fp, ("\t\tspecularLever:%f,glossiness:%f,opacity:%f\n"), &material.specularLevel, &material.glossiness, &material.opacity);
				//texture_pde======================================================================
				fscanf(fp, ("\t%s\n"), buffer);
				fscanf(fp, ("\t\ttextureNum:%d\n"), &textureNum);
				for (int texture_i = 0; texture_i < textureNum; texture_i++)
				{

					PDE::texture_m_pde texture;
					fscanf(fp, ("\t\ttextureName:%s\n"), texture.type);
					fscanf(fp, ("\t\ttextureIndex:%d\n"), &texture.id);
					fscanf(fp, ("\t\tfileName:%s\n"), texture.path);
					fscanf(fp, ("\t\tClipU:%f,ClipV:%f,ClipW:%f,ClipH:%f\n"), &texture.ClipU, &texture.ClipV, &texture.ClipW, &texture.ClipH);
					material.texture.push_back(texture);
				}
			}

			fscanf(fp, "%s", change);
			if (strcmp(change, "End") != 0)
				;
			modelRoot.materialList.push_back(material);
			
			
			
		}

		

	}

	//导入子材质
	void ModelInput_PDE::importMaterialChild(PDE::material_m_pde *material, FILE *fp)
	{
		int isMultiType = 0, subMaterialNum=0, textureNum=0;
		fscanf(fp, "\%s", change);
		if (strcmp(change, "End_Material") == 0)
			return;
		if (strcmp(change, "Begin") != 0)
			return;

		fscanf(fp, ("\t\tisMultiType:%d\n"), &material->IsMultiType);
		if (material->IsMultiType)
		{

			fscanf(fp, ("\t\tmaterialName:%s\n"), material->materialName);
			fscanf(fp, ("\t\tclassName:%s\n"), material->className);
			fscanf(fp, ("\t\tmaterialIndex:%d,subMaterialNum:%d\n"), &material->materialIndex, &subMaterialNum);
			for (int material_child = 0; material_child < subMaterialNum; material_child++)
			{
				
				PDE::material_m_pde materialChild;
				importMaterialChild(&materialChild, fp);
				material->materialChild.push_back(materialChild);
			}

		}
		else
		{
			fscanf(fp, ("\t\tmaterialName:%s\n"), material->materialName);
			fscanf(fp, ("\t\tclassName:%s\n"), material->className);
			fscanf(fp, ("\t\tmaterialIndex:%d,materialId:%d,textureNum:%d\n"), &material->materialIndex, &material->materialId, &textureNum);
			fscanf(fp, ("\t\tdiffuse[0]:%f,diffuse[1]:%f,diffuse[2]:%f\n"), &material->diffuse[0], &material->diffuse[1], &material->diffuse[2]);
			fscanf(fp, ("\t\tambient[0]:%f,ambient[1]:%f,ambient[2]:%f\n"), &material->ambient[0], &material->ambient[1], &material->ambient[2]);
			fscanf(fp, ("\t\tspecular[0]:%f,specular[1]:%f,specular[2]:%f\n"), &material->specular[0], &material->specular[1], &material->specular[2]);
			fscanf(fp, ("\t\tspecularLever:%f,glossiness:%f,opacity:%f\n"), &material->specularLevel, &material->glossiness, &material->opacity);
			//texture_pde======================================================================
			fscanf(fp, ("\t%s\n"), buffer);
			fscanf(fp, ("\t\ttextureNum:%d\n"), &textureNum);
			for (int texture_i = 0; texture_i < textureNum; texture_i++)
			{
				
				PDE::texture_m_pde texture;
				fscanf(fp, ("\t\ttextureName:%s\n"), texture.type);
				fscanf(fp, ("\t\ttextureIndex:%d\n"), &texture.id);
				fscanf(fp, ("\t\tfileName:%s\n"), texture.path);
				fscanf(fp, ("\t\tClipU:%f,ClipV:%f,ClipW:%f,ClipH:%f\n"), &texture.ClipU, &texture.ClipV, &texture.ClipW, &texture.ClipH);
				material->texture.push_back(texture);
			}
		}

		fscanf(fp, "%s", change);
		if (strcmp(change, "End") != 0)
			;

	}


	PDE::material_m_pde * ModelInput_PDE::getMaterial(PDE::root_m_pde modelRoots, const char *materialName)
	{
		for (int i = 0; i < modelRoots.materialList.size(); i++)
		{
			if (modelRoots.materialList[i].materialName==materialName)
			{
				return &modelRoots.materialList[i];
			}
		}
	}

	//为纹理分配id
	void ModelInput_PDE::genTexxtureID()
	{
		for (int i = 0; i < modelRoot.materialList.size(); i++)
		{
			PDE::material_m_pde *material = &modelRoot.materialList[i];
			genTextureIDChild(material);
		}
	}

	//为纹理分配id续
	void ModelInput_PDE::genTextureIDChild(PDE::material_m_pde *material)
	{
		if (!material->IsMultiType)
		{
				for (int i = 0; i < material->texture.size(); i++)
				{
					PDE::texture_m_pde *texture = &material->texture[i];
					if (texture->path == "none")
						return;
				/**/	if(!isGenCoordFreeImageOrBit)
						texture->id=PDE::createBitTextures(texture->path);
					else
						texture->id = PDE::createTextures(texture->path);
						
				}
			
		}
		else
		{
			for (int i = 0; i < material->materialChild.size(); i++)
			{
				genTextureIDChild(&material->materialChild[i]);
			}

		}
	}

	void ModelInput_PDE::generationMeshNormal(PDE::model_m_pde *model)
	{
		
	}


	void ModelInput_PDE::setGenCoordFreeimageOrBit(bool is)
	{
		isGenCoordFreeImageOrBit = is;
	}

	void ModelInput_PDE::generationAllMeshNormal()
	{
		for (int i = 0;i < modelRoot.modelList.size();i++)
		{
			PDE::model_m_pde *model = &modelRoot.modelList[i];
			generationMeshNormal(model);
		}
	}

	PDE::model_m_pde *ModelInput_PDE::updateJojintMatrix(PDE::model_m_pde *model)
	{
		

		return model;
	}

	void ModelInput_PDE::updateVertex(PDE::model_m_pde *model)
	{
		
		model->ver = model->vertex;
		vector<PDE::modifier_m_pde>modifier = model->modifier;

		for (int i1 = 0; i1 < model->modifier.size(); i1++)
		{
			vector<PDE::skinVert_m_pde> skin = modifier[i1].skinVert;
			for (int i2 = 0; i2 < modifier[i1].skinVert.size(); i2++)
			{			

				glm::vec3 ve(0, 0, 0);
				PDE::skinVert_m_pde *skinVert = &skin[i2];
				glm::vec3 ve1(model->vertex[skinVert->vertexId].position.x, model->vertex[skinVert->vertexId].position.y, model->vertex[skinVert->vertexId].position.z);
					
				for (int x = 0; x < skinVert->bone.size(); x++)
				{
					PDE::boneAndweight_m_pde *bone = &skinVert->bone[x];
					glm::mat4x4 ma;
					ma = modelRoot.modelList[bone->boneId].showJoint *modelRoot.modelList[bone->boneId].inverseJoint;
					//ve += ma.vectorMatrixMultiply(ve1)*bone->weight;
						
					break;
					
				}
					
				model->ver[skinVert->vertexId].position.x = ve.x;
				model->ver[skinVert->vertexId].position.y = ve.y;
				model->ver[skinVert->vertexId].position.z = ve.z;
						
			}
		}
		
	}

	void ModelInput_PDE::updateBoneToPostMatrix(PDE::model_m_pde *model,bool isState)
	{
		
		PDE::model_m_pde *models = model;
		model->showJoint = model->initJoint;
		//姿势矩阵
		if (model->parentId == -1)
		{
			model->showJoint = model->initJoint;
		}
		while (models->parentId != -1)
		{
			model->showJoint = modelRoot.modelList[models->parentId].initJoint*model->showJoint;
			models = &modelRoot.modelList[models->parentId];
		}
		if (isState)
		{
			model->inverseJoint = model->showJoint;
		}
	
	}


	void ModelInput_PDE::updateModelJointMatrix(PDE::model_m_pde *model, glm::mat4x4 Rot)
	{

		glm::mat4x4  TM, TMB;

		glm::mat4x4 clear(1.0f), rot(1.0f);
		glm::quat qua(model->rotation[3], model->rotation[0], model->rotation[1], model->rotation[2]);
		model->initJoint = clear;

		rot = glm::toMat4(qua);
		//glm::translate(model->initJoint, glm::vec3(100, model->translation[1], model->translation[2]));
		rot *= Rot;
		model->initJoint[3][0] = model->translation[0];
		model->initJoint[3][1] = model->translation[1];
		model->initJoint[3][2] = model->translation[2];
		model->initJoint *= rot;
		glm::scale(model->initJoint, glm::vec3(model->scale[0], model->scale[1], model->scale[2]));
		model->showJoint = model->initJoint;
		

	}

	void ModelInput_PDE::updateBoneState(PDE::model_m_pde *model, glm::mat4x4 Rot)
	{
		updateModelJointMatrix(model, Rot);
		
		updateModelNewJointMatrix(model,false);
		
	}

	void ModelInput_PDE::initBoneMatrix()
	{
		glm::mat4x4 Rot;
		for (int i = 0;i < modelRoot.modelList.size();i++)
		{
			PDE::model_m_pde *model = &modelRoot.modelList[i];
			//updateBonePostMatrix(model, Rot);
		}

		for (int i = 0;i < modelRoot.modelList.size();i++)
		{
			PDE::model_m_pde *model = &modelRoot.modelList[i];
			updateBoneToPostMatrix(model,true);
			//model->showMatrix.inverseTranslate();
		}

	}



	bool ModelInput_PDE::IsExtensionSupported(char* szTargetExtension)
	{
		const unsigned char *pszExtensions = NULL;
		const unsigned char *pszStart;
		unsigned char *pszWhere, *pszTerminator;

		// Extension names should not have spaces
		pszWhere = (unsigned char *)strchr(szTargetExtension, ' ');
		if (pszWhere || *szTargetExtension == '\0')
			return false;

		// Get Extensions String
		pszExtensions = glGetString(GL_EXTENSIONS);

		// Search The Extensions String For An Exact Copy
		pszStart = pszExtensions;
		for (;;)
		{
			pszWhere = (unsigned char *)strstr((const char *)pszStart, szTargetExtension);
			if (!pszWhere)
				break;
			pszTerminator = pszWhere + strlen(szTargetExtension);
			if (pszWhere == pszStart || *(pszWhere - 1) == ' ')
				if (*pszTerminator == ' ' || *pszTerminator == '\0')
					return true;
			pszStart = pszTerminator;
		}
		return false;
	}


	void ModelInput_PDE::drawAnimation(glm::mat4x4 mat,PDE::model_m_pde *model)
	{
		/*
		glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
		float *ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
		if (ptr)
		{
			memcpy(ptr, model->ver.data(),sizeof(float)*3* model->ver.size());

			if (glUnmapBuffer(GL_ARRAY_BUFFER))
			{
				
			}
		}
		glPushMatrix();
		
		glMultMatrixf(glm::value_ptr(mat));

		if (model->textureIndex != 0)
		{
			glBindTexture(GL_TEXTURE_2D, model->textureIndex);

			//进行相关绑定
			if (!textureOrNormalEBO)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, model->Nvboid);
				//glNormalPointer(GL_FLOAT, 0, 0);
			}

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, model->Cvboid);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, model->Vvboid);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		//绑定vao并进行绘制
		//*/
		//glBindVertexArray(ebo.Cvaoid);
/*		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->Ceboid);
		glDrawElements(GL_TRIANGLES, model->CvertexNum, GL_UNSIGNED_INT, NULL);


		if (model->textureIndex != 0)
		{
			//解除相关绑定
			if (!textureOrNormalEBO)
			{
				glDisableClientState(GL_NORMAL_ARRAY);
			}
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glPopMatrix();
	*/	
	}

	/** 初始化多重纹理 */
	bool ModelInput_PDE::initMultiTexture()
	{
		/** 检查是否支持扩展 */
		//if (isExtensionSupported("GL_ARB_multitexture"))
		//{

		/** 获得函数指针地址 */
		//glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
		//glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");

		return true;
		//}
		//else
		//	return false;
	}

	void ModelInput_PDE::updateModelToWord(PDE::model_m_pde *model)
	{/*
		vector<PDE::vertex_m_pde> ver = model->vertex;
		for (int i = 0; i < model->vertex.size(); i++)
		{
			glm::vec3 v(0, 0, 0);
			
			v = model->vertex[i].position*model->showJoint;
			ver[i].position.x = v.x;
			ver[i].position.y = v.y;
			ver[i].position.z = v.z;
		}
		model->vertex = ver;
		*/
	}

	void ModelInput_PDE::updateModelToModel(PDE::model_m_pde *model)
	{
		
	}

	void ModelInput_PDE::updateModelNewJointMatrix(PDE::model_m_pde *model, bool isUpateShowMatrix)
	{
		PDE::model_m_pde *models = model;
		model->showJoint = model->initJoint;

		//姿势矩阵
		if (model->parentId == -1)
		{
			model->showJoint = model->initJoint;
		}
		else
		{
			while (models->parentId != -1)
			{
				models = &modelRoot.modelList[models->parentId];
				model->showJoint = models->initJoint*model->showJoint;
				
			}
			
			
		}
		model->inverseJoint = glm::inverse(model->showJoint);
	}

	void ModelInput_PDE::initModelJointMatrix(PDE::model_m_pde *model)
	{
	
		glm::mat4x4 clear(1.0f),rot(1.0f),init(1.0f);
		model->initJoint =clear;
		model->initJoint = glm::translate(init, glm::vec3(model->translation[0], model->translation[1], model->translation[2]));
		model->initJoint *= glm::toMat4(glm::quat(model->rotation[3], model->rotation[0], model->rotation[1], model->rotation[2]));
		model->initJoint = glm::scale(model->initJoint, glm::vec3(model->scale[0], model->scale[1], model->scale[2]));
		
		model->showJoint = model->initJoint;
	}

	void ModelInput_PDE::initModelJointMatrixUntilTran(PDE::model_m_pde *model)
	{
	
	}

	void ModelInput_PDE::updateBoneId()
	{
		for (int i = 0; i < modelRoot.modelList.size(); i++)
		{
			for (int j = 0; j < modelRoot.modelList.size(); j++)
			{
				if ((i == modelRoot.modelList[j].parentId) && (j != i))
				{
				//	modelRoot.modelList[i].childId.push_back(modelRoot.modelList[j].nodeId);
				}
			}
		}
	}

	void ModelInput_PDE::genVaoVboEbo(PDE::model_m_pde *model)
	{
		for (int i = 0;i < model->mesh.size();i++)
		{
			PDE::mesh_m_pde *mesh=&model->mesh[i];
			// Create buffers/arrays
			glGenVertexArrays(1, &mesh->VAO);
			glGenBuffers(1, &mesh->VBO);
			glGenBuffers(1, &mesh->EBO);

			glBindVertexArray(mesh->VAO);
			// Load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, model->vertex.size() * sizeof(PDE::vertex_m_pde), &model->vertex[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(GLuint), &mesh->indices[0], GL_STATIC_DRAW);

			// Set the vertex attribute pointers
			// Vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PDE::vertex_m_pde), (GLvoid*)0);
			// Vertex Normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PDE::vertex_m_pde), (GLvoid*)offsetof(PDE::vertex_m_pde, normal));
			// Vertex Texture Coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PDE::vertex_m_pde), (GLvoid*)offsetof(PDE::vertex_m_pde, coord));
			/**/
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		
	}

	void  ModelInput_PDE::drawUseShader(PDE::model_m_pde *model,Shader *shader)
	{
		PDE::material_m_pde *material = NULL;
		shader->setMat4("model", model->showJoint);
		for (int i = 0;i < model->mesh.size();i++)
		{
			if (model->materialIndex != -1)
			{
				if (modelRoot.materialList[model->materialIndex].IsMultiType)
				{

						material = &modelRoot.materialList[model->materialIndex].materialChild[model->mesh[i].faceMatrialIndex];
						glBindTexture(GL_TEXTURE_2D, modelRoot.materialList[model->materialIndex].
							materialChild[model->mesh[i].faceMatrialIndex].
							texture[0].id);
				}
				else
				{
						material = &modelRoot.materialList[model->materialIndex];
						if(modelRoot.materialList[model->materialIndex].texture.size()>0)
						glBindTexture(GL_TEXTURE_2D, modelRoot.materialList[model->materialIndex].texture[0].id);
					
				}
				shader->setVec3("material.diffuse", material->diffuse[0], material->diffuse[1], material->diffuse[2]);
				shader->setVec3("material.specular", material->specular[0], material->specular[1], material->specular[2]);
				shader->setFloat("material.shininess", material->specularLevel);
			}

			
			

			glBindVertexArray(model->mesh[i].VAO);
			glDrawElements(GL_TRIANGLES, model->mesh[i].indices.size(), GL_UNSIGNED_INT, 0);
			
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (j > 360)
		{
			j = 0;
		}
		j += 0.5;
	}
}