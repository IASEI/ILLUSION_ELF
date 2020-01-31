#include "PlugIn.h"


namespace ASE
{
	PlugIn::PlugIn()
	{
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
