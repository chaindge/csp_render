#include "ConsoleSingleton.h"
#include "ConsoleFactory.h"

ConsoleSingleton* ConsoleSingleton::getInstance()
{
	static ConsoleSingleton instance;
	return &instance;
}

ConsoleSingleton::ConsoleSingleton()
{
	_impl = _ConsoleFactory->Create("Debug");
}

ConsoleSingleton::~ConsoleSingleton()
{
	delete _impl;
}

