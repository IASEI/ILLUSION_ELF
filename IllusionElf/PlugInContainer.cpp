#include "PlugInContainer.h"

namespace ASE
{
	PlugInContainer::PlugInContainer()
	{
	}

	bool PlugInContainer::Init(std::string path)
	{
		// 在plugins目录中查找dll文件并将文件信息保存在fileData中
		WIN32_FIND_DATA fileData;
		HANDLE fileHandle = FindFirstFile(path+"*.dll", &fileData);
		if (fileHandle == (void*)ERROR_INVALID_HANDLE ||
			fileHandle == (void*)ERROR_FILE_NOT_FOUND) {
			// 没有找到任何dll文件
			return false;
		}

		// 循环加载plugins目录中的所有dll文件
		do {
			typedef PlugIn* (__cdecl *ObjProc)(void);
			typedef const char* (__cdecl *NameProc)(void);

			// 将dll加载到当前进程的地址空间中
			HINSTANCE mod = LoadLibrary((std::wstring(path) + std::wstring(fileData.cFileName)).c_str());

			if (!mod) {
				// 加载dll失败，则释放所有已加载dll
				for (HINSTANCE hInst : modules)
					FreeLibrary(hInst);
				throw MyException(L"Library " + std::wstring(fileData.cFileName) + L" wasn't loaded successfully!");
			}
			// 从dll句柄中获取getObj和getName的函数地址
			ObjProc objFunc = (ObjProc)GetProcAddress(mod, "getObj");
			NameProc nameFunc = (NameProc)GetProcAddress(mod, "getName");

			if (!objFunc || !nameFunc)
				throw std::runtime_error("Invalid Plugin DLL: both 'getObj' and 'getName' must be defined.");

			mPlug.push_back(objFunc());  // 保存objFunc（即getObj）生成的对象指针
			modules.push_back(mod);		 // 保存dll句柄

			std::clog << nameFunc() << " loaded!\n";
		} while (FindNextFile(fileHandle, &fileData));

		std::clog << std::endl;

		// 关闭文件句柄
		FindClose(fileHandle);
		return true;
	}

	bool PlugInContainer::Run()
	{
		if (!this->isRun)return false;
		for (auto &p : this->mPlug)
		{
			bool status=p->Run();
			switch (p->GetType())
			{
			case ASE::PlugIn::ASE_PLUGIN_GLOBAL:
				break;
			case ASE::PlugIn::ASE_PLUGIN_COMMAND:
				if (!status)
				{
					std::vector<PlugIn*>::iterator it=std::find_if(mPlug.begin(), mPlug.end(), std::bind(this->JudgePlugName, std::placeholders::_1, p->GetName()));
					if (it != mPlug.end())
					{
						delete mPlug[it - mPlug.begin()];
						mPlug[it - mPlug.begin()] = NULL;
						mPlug.erase(it);
					}
				}
				break;
			default:
				break;
			}
		}
		return true;
	}

	void PlugInContainer::Destory()
	{
		for (auto &p : this->mPlug)
		{
			delete p;p = NULL;
		}

		for (HINSTANCE hInst : this->modules)
			FreeLibrary(hInst);
	}

	bool PlugInContainer::JudgePlugName(PlugIn &obj,std::string name)
	{
		if (obj.GetName() == name)
			return true;
		else
			return false;
	}
	PlugInContainer::~PlugInContainer()
	{
		this->Destory();
	}
}
