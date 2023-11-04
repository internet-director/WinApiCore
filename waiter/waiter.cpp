#include <windows.h>
#include <iostream>
#include <core/core.h>


DWORD WINAPI ThreadProc(LPVOID lpParameter) {
    auto lambdaFunction = reinterpret_cast<core::function<void(LPVOID)>*>(lpParameter);
    (*lambdaFunction)();
    return 0;
}

class thread {
	core::function<void()> func;
	HANDLE hThread = nullptr;
	bool joined = false;
	DWORD id;

public:
	thread() noexcept = default;
	thread(thread&& other) noexcept = default;
	template< class Function, class... Args >
	explicit thread(Function&& f, Args&&... args) {
        auto lambda = [f = std::forward<Function>(f), ...captured_args = std::forward<Args>(args)]() {
            f(captured_args...);
            };

		core::function<void()> fnc = lambda;
		hThread = CreateThread(NULL, 0, ThreadProc, &fnc, 0, &id);
        join();
	}
	thread(const thread&) = delete;
	~thread() {
		if (!joined) join();
		core::CloseHandle(hThread);
	}

	void join() {
		API(KERNEL32, WaitForSingleObject)(hThread, INFINITE);
		joined = true;
	}
};

int main() {
    int a = time(0);

	core::thread aa([&a](int k, std::string str) {
        for (int i = 0; i < 10; i++) {
            std::cout << k << " " << a << " " << str << std::endl;
        }
    }, 123, "data_kal");
	aa.join();
    return 0;
}
