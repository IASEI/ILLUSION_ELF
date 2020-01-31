#ifndef __PLUGINCONTAINER_H__
#define __PLUGINCONTAINER_H__
/*
*插件容器
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
		bool Init(std::string path="../plug/");//加载插件
		void Destory();//释放插件
		bool JudgePlugName(PlugIn &obj, std::string name);
		~PlugInContainer();

	private:
		std::vector<PlugIn*> mPlug;//插件
		std::vector<HINSTANCE> modules;//保存插件dll句柄

	};
}
#endif // !__PLUGINCONTAINER_H__


