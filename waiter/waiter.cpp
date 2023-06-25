#include <Windows.h>
#include <core/core.h>
#include <iostream>

int main()
{
	core::init();
	{
		for (int i = 0; i < 1000; i++) {
			Sleep(100000);
		}
	}
	core::close();
	return 0;
}