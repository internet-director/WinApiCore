#include "pch.h"
#include "core.h"

bool core::init()
{
	return _wobf.init();
}

void core::close()
{
	_wobf.close();
}
