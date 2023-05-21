#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

int main()
{
	for (int i = 0; i < 1000000; i++) {
		Sleep(1000);
	}
	return 0;
}