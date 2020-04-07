#include "PQEExport.h"

namespace PQE
{
	PQEExport::PQEExport(PQE_MODEL *model, std::string path)
	{
		this->mModel = model;
		file = new std::fstream;
		file->open(path, ios::out | ios::binary);
		if (file->fail())
		{
			std::cout << "save failure" << endl;
			return;
		}
		std::cout << "start save" << endl;
		file->write((char*)&this->mModel->nodeNum, sizeof(unsigned int)); 		std::cout << "start save nodeNum successful" << endl;
		file->write((char*)&this->mModel->meshNum, sizeof(unsigned int)); 		std::cout << "start save meshNum successful" << endl;
		file->write((char*)&this->mModel->shapeNum, sizeof(unsigned int)); 		std::cout << "start save shapeNum successful" << endl;
		file->write((char*)&this->mModel->matrixNum, sizeof(unsigned int)); 	std::cout << "start save matrixNum successful" << endl;
		file->write((char*)&this->mModel->textureNum, sizeof(unsigned int)); 	std::cout << "start save textureNum successful" << endl;
		file->write((char*)&this->mModel->materialNum, sizeof(unsigned int)); 	std::cout << "start save materialNum successful" << endl;
		ExportNode();		std::cout << "start save Node successful" << endl;
		ExportMesh();		std::cout << "start save Mesh successful" << endl;
		ExportShape();		std::cout << "start save Shape successful" << endl;
		ExportMatrix();		std::cout << "start save Matrix successful" << endl;
		ExportTexture();	std::cout << "start save Texture successful" << endl;
		ExportMaterial();	std::cout << "start save Material successful" << endl;
		file->close();
	}

	void PQEExport::ExportNode()
	{
		unsigned int num = this->mModel->nodeNum;
		for (unsigned int i = 0; i < num; i++)
		{
			PQE_NODE *node = &this->mModel->mNode[i];
			file->write((char*)node->mName, 255 * sizeof(char));
			file->write((char*)&node->childNum, 1 * sizeof(unsigned int));
			file->write((char*)&node->meshNum, 1 * sizeof(unsigned int));
			file->write((char*)&node->shapeNum, 1 * sizeof(unsigned int));
			file->write((char*)&node->vertexNum, 1 * sizeof(unsigned int));
			file->write((char*)&node->shapeVertexNum, 1 * sizeof(unsigned int));
			file->write((char*)&node->spesicalId, 5 * sizeof(unsigned int));
			file->write((char*)&node->mMatrixIndex, 1 * sizeof(unsigned int));
			file->write((char*)&node->mParentId, 1 * sizeof(unsigned int));
			file->write((char*)&node->mSelfId, 1 * sizeof(unsigned int));
			if (node->childNum)file->write((char*)&node->mChildId[0], node->childNum * sizeof(unsigned int));
			if (node->meshNum)file->write((char*)&node->mMeshIndex[0], node->meshNum * sizeof(unsigned int));
			if (node->shapeNum)file->write((char*)&node->mShapeIndex[0], node->shapeNum * sizeof(unsigned int));
			if (node->vertexNum)file->write((char*)&node->mBoneIndex[0], node->vertexNum * sizeof(glm::int4));
			if (node->vertexNum)file->write((char*)&node->mBoneWeight[0], node->vertexNum * sizeof(glm::vec4));
			if (node->vertexNum)file->write((char*)&node->mPosition[0], node->vertexNum * sizeof(glm::vec4));
			if (node->vertexNum)file->write((char*)&node->mNormal[0], node->vertexNum * sizeof(glm::vec3));
			if (node->vertexNum)file->write((char*)&node->mCoord[0], node->vertexNum * sizeof(glm::vec2));

		}
	}

	void PQEExport::ExportMatrix()
	{
		file->write((char*)&this->mModel->mMatrix[0], this->mModel->matrixNum * sizeof(PQE_MATRIX));
	}

	void PQEExport::ExportMesh()
	{
		unsigned int num = this->mModel->meshNum;
		for (unsigned int i = 0; i < num; i++)
		{
			PQE_MESH *mesh = &this->mModel->mMesh[i];
			file->write((char*)mesh->mName, 255 * sizeof(char));
			file->write((char*)&mesh->vertexNum, 1 * sizeof(unsigned int));
			file->write((char*)&mesh->faceNum, 1 * sizeof(unsigned int));
			file->write((char*)&mesh->boneNum, 1 * sizeof(unsigned int));
			file->write((char*)&mesh->mMaterilIndex, 1 * sizeof(unsigned int));
			file->write((char*)&mesh->mShapeWeight, 1 * sizeof(unsigned int));
			file->write((char*)&mesh->vao, 1 * sizeof(unsigned int));
			file->write((char*)&mesh->ebo, 1 * sizeof(unsigned int));
			file->write((char*)&mesh->mFace[0], mesh->faceNum * sizeof(unsigned int));
		}
	}

	void PQEExport::ExportShape()
	{
		unsigned int num = this->mModel->shapeNum;
		for (unsigned int i = 0; i < num; i++)
		{
			PQE_SHAPE *shape = &this->mModel->mShape[i];
			file->write((char*)&shape->mName, 255 * sizeof(char));
			file->write((char*)&shape->vertexNum, 1 * sizeof(unsigned int));
			file->write((char*)&shape->weight, 1 * sizeof(float));
			file->write((char*)&shape->mPosition[0], shape->vertexNum * sizeof(glm::vec4));
		}
	}

	void PQEExport::ExportMaterial()
	{
		unsigned int num = this->mModel->materialNum;
		for (unsigned int i = 0; i < num; i++)
		{
			PQE_MATERIAL *material = &this->mModel->mMaterial[i];
			file->write((char*)&material->materialNum, 1 * sizeof(unsigned int));
			file->write((char*)&material->textureNum, 1 * sizeof(unsigned int));
			file->write((char*)&material->mTextureIndex[0], material->textureNum * sizeof(unsigned int));
		}
	}

	void PQEExport::ExportTexture()
	{
		file->write((char*)&this->mModel->mTexture[0], this->mModel->textureNum * sizeof(PQE_TEXTURE));
	}

	PQEExport::~PQEExport()
	{
		mModel = NULL;
		delete file;
	}

}
