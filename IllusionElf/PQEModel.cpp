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

