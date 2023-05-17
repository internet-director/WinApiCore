#include <iostream>
#include <Windows.h>

int main()
{
	int a = 2, b;
	b = std::move(a);
	std::is_same_v<char, int>;
	for (int i = 0; i < 1000000; i++) {
		std::cout << i << ' ';
		Sleep(1000);
	}
	return 0;
}