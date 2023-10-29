#include "pch.h"
#include <thread/mutex.h>

namespace core {
	mutex::mutex() {
		_mutex = API(KERNEL32, CreateMutexW)(NULL, FALSE, L"mutex");
	}
	mutex::~mutex() {
		unlock();
		core::CloseHandle(_mutex);
	}

	void mutex::lock() {
		API(KERNEL32, WaitForSingleObject)(_mutex, INFINITE);
	}

	bool mutex::try_lock() noexcept {
		locked = API(KERNEL32, WaitForSingleObject)(_mutex, 0) == WAIT_OBJECT_0;
		return locked;
	}

	void mutex::unlock() {
		if (locked) {
			API(KERNEL32, ReleaseMutex)(_mutex);
			locked = false;
		}
	}
}
