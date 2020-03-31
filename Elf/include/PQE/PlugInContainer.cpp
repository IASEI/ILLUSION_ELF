#include "PlugInContainer.h"

namespace ASE
{
	PlugInContainer::PlugInContainer()
	{
		this->isRun = true;
	}

	bool PlugInContainer::Init(std::string path)
	{
		if (_access(path.c_str(), 0) == -1)_mkdir(path.c_str());
		_bstr_t t = path.c_str();
		wchar_t* pwchar = (wchar_t*)t;
		std::wstring wpath = pwchar;
		// 在plugins目录中查找dll文件并将文件信息保存在fileData中
		WIN32_FIND_DATA fileData;
		HANDLE fileHandle = FindFirstFile((wpath +L"*.dll").c_str(), &fileData);
		if (fileHandle == (void*)ERROR_INVALID_HANDLE ||fileHandle == (void*)ERROR_FILE_NOT_FOUND||fileHandle== INVALID_HANDLE_VALUE)
		{
			// 没有找到任何dll文件
			std::cout << "not dll" << std::endl;
			return false;
		}

		//循环加载plugins目录中的所有dll文件
		do {
			typedef PlugIn* (__cdecl *ObjProc)(void);
			typedef const char* (__cdecl *NameProc)(void);

			// 将dll加载到当前进程的地址空间中
			HINSTANCE mod = LoadLibrary((wpath + std::wstring(fileData.cFileName)).c_str());

			if (!mod) 
			{
				// 加载dll失败，则释放所有已加载dll
				for (HINSTANCE hInst : modules)
					FreeLibrary(hInst);
				printf("not load successfully\n");
				return false;
			}
			// 从dll句柄中获取getObj和getName的函数地址
			ObjProc objFunc = (ObjProc)GetProcAddress(mod, "GetObj");
			NameProc nameFunc = (NameProc)GetProcAddress(mod, "GetName");

			if (!objFunc || !nameFunc)
				printf("Invalid Plugin DLL: both 'getObj' and 'getName' must be defined.\n");
			printf("%s\n", path.c_str());

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
		std::vector<std::string> deleteName;
		if (!this->isRun)return false;
		for (auto &p : this->mPlug)
		{
			p->Run();
			bool status=p->GetStatus();
			switch (p->GetType())
			{
			case ASE::PlugIn::ASE_PLUGIN_GLOBAL:
				break;
			case ASE::PlugIn::ASE_PLUGIN_COMMAND:
				if (!status)
					deleteName.push_back(p->GetName());
				break;
			default:
				break;
			}
		}
		for (auto &d : deleteName)
		{
			std::vector<PlugIn*>::iterator it = std::find_if(this->mPlug.begin(), this->mPlug.end(), [&](PlugIn *obj) {return obj->GetName() == d;});
			if (it != mPlug.end())
			{
				delete mPlug[it - mPlug.begin()];
				mPlug[it - mPlug.begin()] = NULL;
				mPlug.erase(it);
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

	PlugInContainer::~PlugInContainer()
	{
		this->Destory();
	}
}
