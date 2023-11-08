#pragma once
#include <core/config.h>
#include <core/mem.h>
#include <core/function.h>

namespace core {
	class THREAD_EXPORT thread {
		HANDLE hThread{ nullptr };
		DWORD id{ 0 };

	public:
		thread() = default;
		template< class Function, class... Args >
		explicit thread(Function&& f, Args&&... args) {
			auto* func = core::constuct<core::function<void()>>(1, [f = core::forward<Function>(f), ...captured_args = core::forward<Args>(args)]() {
				f(captured_args...);
			});
			if (func != nullptr) {
				call(func);
			}
		}
		~thread();

		thread(const thread&) = delete;
		thread(thread&& other) = default;
		thread& operator=(const thread& other) = delete;
		thread& operator=(thread&& other) noexcept;

		void join();
		bool joinable() const noexcept {
			return hThread != nullptr;
		}
		void kill();
		void detach() noexcept;
		void swap(thread& other) noexcept;
		DWORD get_id() const noexcept { return id; }

	private:
		void call(core::function<void()>* func);
	};
}
