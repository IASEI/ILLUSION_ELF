#include "PlugInContainer.h"

namespace ASE
{
	PlugInContainer::PlugInContainer()
	{
	}

	bool PlugInContainer::Init(std::string path)
	{
		// ��pluginsĿ¼�в���dll�ļ������ļ���Ϣ������fileData��
		WIN32_FIND_DATA fileData;
		HANDLE fileHandle = FindFirstFile(LPCWCHAR((path+"*.dll").c_str()), &fileData);
		if (fileHandle == (void*)ERROR_INVALID_HANDLE ||
			fileHandle == (void*)ERROR_FILE_NOT_FOUND) {
			// û���ҵ��κ�dll�ļ�
			return false;
		}

		// ѭ������pluginsĿ¼�е�����dll�ļ�
		do {
			typedef PlugIn* (__cdecl *ObjProc)(void);
			typedef const char* (__cdecl *NameProc)(void);

			// ��dll���ص���ǰ���̵ĵ�ַ�ռ���
			HINSTANCE mod = LoadLibrary((LPCWCHAR(path.c_str()) + std::wstring(fileData.cFileName)).c_str());

			if (!mod) {
				// ����dllʧ�ܣ����ͷ������Ѽ���dll
				for (HINSTANCE hInst : modules)
					FreeLibrary(hInst);
				//throw MyException(L"Library " + std::wstring(fileData.cFileName) + L" wasn't loaded successfully!");
			}
			// ��dll����л�ȡgetObj��getName�ĺ�����ַ
			ObjProc objFunc = (ObjProc)GetProcAddress(mod, "getObj");
			NameProc nameFunc = (NameProc)GetProcAddress(mod, "getName");

			if (!objFunc || !nameFunc)
				throw std::runtime_error("Invalid Plugin DLL: both 'getObj' and 'getName' must be defined.");

			mPlug.push_back(objFunc());  // ����objFunc����getObj�����ɵĶ���ָ��
			modules.push_back(mod);		 // ����dll���

			std::clog << nameFunc() << " loaded!\n";
		} while (FindNextFile(fileHandle, &fileData));

		std::clog << std::endl;

		// �ر��ļ����
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
