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

		}
		thread(const thread&) = delete;

		void join() noexcept {
			API(KERNEL32, WaitForSingleObject)(hThread, INFINITE);
		}
	};
}