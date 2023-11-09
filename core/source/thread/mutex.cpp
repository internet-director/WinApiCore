#include "pch.h"
#include <thread/mutex.h>
#include <iostream>

namespace core {
	mutex::mutex() {
		_mutex = core::CreateMutexW(NULL, FALSE, NULL);
	}
	mutex::~mutex() {
		if (inited()) {
			unlock();
			core::CloseHandle(_mutex);
			_mutex = nullptr;
		}
	}

	void mutex::lock() {
		if (_mutex != nullptr) {
			locked = true;
			//locked = core::WaitForSingleObject(_mutex, INFINITE) == WAIT_OBJECT_0;
		}
	}

	bool mutex::try_lock() noexcept {
		if (_mutex == nullptr) {
			return false;
		}
		locked = core::WaitForSingleObject(_mutex, 0) == WAIT_OBJECT_0;
		return locked;
	}

	void mutex::unlock() {
		if (locked) {
			core::ReleaseMutex(_mutex);
			locked = false;
		}
	}
}
