#ifndef __PQEEXPORT_H__
#define __PQEEXPORT_H__

#include "PQEModel.h"

namespace PQE

{
	class PQEExport
	{
	public:
		PQEExport();
		PQEExport(PQEModel *model,std::string path)
		{
			this->mModel=model;
			 handle = open(path,  O_WRONLY|O_CREAT);  
			if(*handle==-1)
			{
				std::cout<<"save failure"<<endl;
				return;
			}
			std::cout<<"start save"<<endl;
			
		}
		~PQEExport();
		void ExportNode()
		{
			write(handel,this->mModel->nodeNum,sizeof(unsigned int));
			write(handel,this->mModel->mNode,sizeof());
		}
	private:
	PQEModel *mModel;
	int *handle;
	};
}

#endif

