#include "PlugIn.h"


namespace ASE
{
	PlugIn::PlugIn()
	{
	}

	bool PlugIn::Run()
	{
		return true;
	}

	bool PlugIn::Render()
	{
		return true;
	}

	bool PlugIn::Compute()
	{
		return true;
	}

	std::string PlugIn::GetName()
	{
		return this->mName;
	}

	PlugIn::ASE_PLUGIN_TYPE PlugIn::GetType()
	{
		return this->mType;
	}

	bool PlugIn::GetStatus()
	{
		return this->isRun;
	}

	PlugIn::~PlugIn()
	{
	}

}
