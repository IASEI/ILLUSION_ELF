#ifndef __PLUGINCONTAINER_H__
#define __PLUGINCONTAINER_H__
/*
*�������
*/

#include<iostream>
#include<vector>
#include<algorithm>
#include<functional>
#include<windows.h>
#include"PlugIn.h"


namespace ASE
{
	class PlugInContainer:public PlugIn
	{
	public:
		PlugInContainer();
		bool Run();
		bool Init(std::string path="../plug/");//���ز��
		void Destory();//�ͷŲ��
		bool JudgePlugName(PlugIn &obj, std::string name);
		~PlugInContainer();

	private:
		std::vector<PlugIn*> mPlug;//���
		std::vector<HINSTANCE> modules;//������dll���

	};
}
#endif // !__PLUGINCONTAINER_H__


