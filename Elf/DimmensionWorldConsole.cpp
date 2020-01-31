#include "DimmensionWorldConsole.h"

int main(int argc, char *argv[])
{
	int ret = -1;
	DimmensionWorldConsole *dwd = DimmensionWorldConsole::create("dwd");
	if (dwd != 0)
	{
		ret = dwd->Main();
		delete dwd;
	}
	return 0;
}

