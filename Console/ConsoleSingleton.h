#ifndef __CONSOLESINGLETON_H__
#define __CONSOLESINGLETON_H__

#include "IConsole.h"

class ConsoleSingleton 
{
public:
	static ConsoleSingleton* getInstance();
	~ConsoleSingleton();

	inline IConsole& getConsole() { return *_impl; }
protected:
	ConsoleSingleton();

	IConsole* _impl;
};

#define debug_con ConsoleSingleton::getInstance()->getConsole()

#endif //__CONSOLESINGLETON_H__