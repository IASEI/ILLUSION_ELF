#ifndef __PQEEXPORT_H__
#define __PQEEXPORT_H__

#include <fstream>
#include "PQEModel.h"

namespace PQE
{
	class PQEExport
	{
	public:
		PQEExport(PQE_MODEL *model, std::string path);
		~PQEExport();
	private:
		void ExportNode();
		void ExportMatrix();
		void ExportMesh();
		void ExportShape();
		void ExportMaterial();
		void ExportTexture();
	private:
		PQE_MODEL *mModel;
		std::fstream *file;
	};
}

#endif

