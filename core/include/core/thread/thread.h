#pragma once
#include <core/config.h>
#include <core/mem.h>
#include <core/function.h>

namespace core {
	class THREAD_EXPORT thread {
		core::unique_ptr<core::function<void()>> func;
		HANDLE hThread{ nullptr };
		bool joined{ false };
		DWORD id{ 0 };

	public:
		thread() = default;
		template< class Function, class... Args >
		explicit thread(Function&& f, Args&&... args) {
			func = core::make_unique<core::function<void()>>([f = core::forward<Function>(f), ...captured_args = core::forward<Args>(args)]() {
				f(captured_args...);
			});
			if (func != nullptr) {
				call();
			}
		}
		~thread();

		thread(const thread&) = delete;
		thread(thread&& other) = default;
		thread& operator=(const thread& other) = delete;
		thread& operator=(thread&& other) noexcept {
			if (this != &other) {
				func = std::move(other.func);
				hThread = std::move(other.hThread);
				joined = std::move(other.joined);
				id = std::move(other.id);

				other.func.~unique_ptr();
				other.hThread = nullptr;
				other.id = 0;
				other.joined = false;
			}
			return *this;
		}

		void join();
		void swap(thread& other) noexcept;

	private:
		void call();
	};
}
