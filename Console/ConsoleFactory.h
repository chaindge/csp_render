#ifndef ConsoleFactory_h
#define ConsoleFactory_h
#include "IConsole.h"

#define _ConsoleFactory ConsoleFactory::getInstance()

class ConsoleFactory
{
public:
	static ConsoleFactory* getInstance();

	IConsole* Create(const std::string& title);
protected:
	ConsoleFactory(){}
	~ConsoleFactory() {}
};
#endif // !ConsoleFactory_h
