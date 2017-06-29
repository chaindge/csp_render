#include "ConsoleFactory.h"
#include "../PlatformMacro/define_os.h"

#ifdef OS_WIN32
#include "ConsoleWinImpl.h"
#endif

ConsoleFactory * ConsoleFactory::getInstance()
{
	static ConsoleFactory instance;
	return &instance;
}

IConsole * ConsoleFactory::Create(const std::string & title)
{
#ifdef OS_WIN32
	IConsole * pConsole = new ConsoleWinImpl;
	if (pConsole && pConsole->init(title)) {
		return pConsole;
	}
	delete pConsole;
#endif
	return nullptr;
}
