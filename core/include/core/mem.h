#pragma once
#include <core/config.h>
#include <core/types.h>
#include <core/NtApi.h>

namespace core
{
	MEM_EXPORT void memInit();
	MEM_EXPORT constexpr volatile void* memcpy(volatile void* dst, const void* src, size_t sz);
	MEM_EXPORT constexpr volatile void* memset(volatile void* dst, int byte, size_t sz);
	MEM_EXPORT constexpr int memcmp(const void* dst, const void* src, size_t sz);
	MEM_EXPORT constexpr volatile void* zeromem(volatile void* dst, size_t sz);

	template<typename T>
	MEM_EXPORT constexpr void fill(T* dst, const T& obj, size_t sz) {
		for (size_t i = 0; i < sz; i++) {
			dst[i] = obj;
		}
	}

	template<typename T>
	MEM_EXPORT constexpr void zero(T& obj) {
		zeromem(&obj, sizeof T);
	}

	MEM_EXPORT void* alloc(size_t sz);
	template<typename T>
	MEM_EXPORT T* alloc(size_t sz) {
		return static_cast<T*>(core::alloc(sz * sizeof(T))); 
	}
	MEM_EXPORT int free(void* heap);

	template<typename T>
	MEM_EXPORT T* talloc(size_t sz) {
		return static_cast<T*>(alloc(sz * sizeof(T)));
	}

	template<typename T>
	MEM_EXPORT constexpr size_t strlen(const T* str)
	{
		size_t sz = -1;
		while (str[++sz]);
		return sz;
	}

	void Wide2Char(const WCHAR* data, char* out, UINT len);

	template<typename T>
	size_t find(const T* array, const T& obj, size_t sz) {
		for (size_t i = 0; i < sz; i++) {
			if (array[i] == obj) return i;
		}
		return core::npos;
	}
}

void* operator new(size_t size);
void operator delete(void* p) noexcept;
void operator delete(void* ptr, size_t) noexcept;
void* WINAPI operator new(size_t _Size, void* _Where) noexcept;

namespace core {
	template<typename T>
	class unique_ptr {
		T* ptr = nullptr;

	public:
		unique_ptr() = default;
		unique_ptr(unique_ptr&& other) noexcept {
			if (this != &other) {
				ptr = other.ptr;
				other.ptr = nullptr;
			}
		}
		unique_ptr(const unique_ptr&) = delete;
		unique_ptr(T* ptr) noexcept: ptr{ptr} {}
		~unique_ptr() {
			reset(nullptr);
		}

		T* get() const noexcept {
			return ptr;
		}

		unique_ptr& operator=(unique_ptr&& other) noexcept {
			if (this != &other) {
				unique_ptr(core::move(other)).swap(*this);
			}
			return *this;
		}
		unique_ptr& operator=(const unique_ptr& other) = delete;

		void reset(T* rhs) noexcept {
			if (ptr != nullptr) {
				ptr->~T();
				core::free(ptr);
			}
			ptr = rhs;
		}

		void swap(unique_ptr& other) noexcept {
			core::swap(ptr, other.ptr);
		}

		bool operator!=(T* rhs) const noexcept {
			return ptr != rhs;
		}
		bool operator==(T* rhs) const noexcept {
			return ptr == rhs;
		}
	};

	template<typename T, typename ...Args>
	constexpr unique_ptr<T> make_unique(Args... args) {
		auto* ptr = core::alloc<T>(1);
		return unique_ptr<T>(new(ptr) T(core::forward<Args>(args)...));
	}
}