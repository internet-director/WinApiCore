#pragma once
#include <core/config.h>

namespace core {
	class THREAD_EXPORT thread {
		HANDLE hThread = nullptr;
		DWORD id;

	public:
		thread() noexcept = default;
		thread(thread&& other) noexcept = default;
		template< class Function, class... Args >
		explicit thread(Function&& f, Args&&... args) {
			struct Data {
				Function f;
				Args... arg;
			};
			Data data;
			data.f = f;
			
			auto work = [](LPVOID data) -> DWORD { 
				
				return 0; 
			};
			hThread = CreateThread(nullptr, 0, work, &data, 0, nullptr);
		}
		thread(const thread&) = delete;

		void join() noexcept {
			API(KERNEL32, WaitForSingleObject)(hThread, INFINITE);
		}
	};
}