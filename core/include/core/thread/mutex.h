#pragma once
#include <core/config.h>
#include <core/wobf/wobf.h>

namespace core {
	class THREAD_EXPORT mutex
	{
		HANDLE _mutex = nullptr;
		bool locked = false;
	public:
		mutex();
		~mutex();

		bool inited() const noexcept { return _mutex; }
		void lock();
		bool try_lock() noexcept;
		void unlock();
	};
}
