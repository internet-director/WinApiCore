#include <windows.h>
#include <iostream>
#include <functional>

DWORD WINAPI ThreadProc(LPVOID lpParameter) {
    auto lambdaFunction = reinterpret_cast<std::function<void()>*>(lpParameter);
    (*lambdaFunction)();
    return 0;
}

template< class Function, class... Args >
void test(Function&& f, Args&&... args) {
    auto lambda = [f = std::forward<Function>(f), ...captured_args = std::forward<Args>(args)]() {
        f(captured_args...);
        };

    std::function<void()> lambdaFunction = std::function<void()>(lambda);

    HANDLE hThread = CreateThread(NULL, 0, ThreadProc, &lambdaFunction, 0, NULL);
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }
}

int main() {
    // Ваша лямбда-функция
    int a = time(0);

    test([&a](int k, std::string str) {
        for (int i = 0; i < 10; i++) {
            std::cout << k << " " << a << " " << str << std::endl;
        }
        }, 123, "data_kal");
    return 0;
}
