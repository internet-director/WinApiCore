#include "pch.h"
#include "core.h"

bool core::init()
{
	bool res = true;
	res &= _wobf.init();
	res &= _directSyscall.init();
	return res;
}

int core::close()
{
	_directSyscall.close();
	_wobf.close();
	return 0;
}
