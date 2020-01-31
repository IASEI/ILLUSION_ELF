#include "IllusionElf.h"


IllusionElf::IllusionElf()
{
}


IllusionElf::~IllusionElf()
{
}

DimmensionWorldConsole *DimmensionWorldConsole::create(std::string classname)
{
	IllusionElf *t = new IllusionElf();
	return reinterpret_cast<IllusionElf *>(t);
}
