#pragma once
#include <core/types.h>
#include <core/mem.h>

namespace core {
	template<typename T>
	class Vector {
		T* arr{ nullptr };
		size_t _size{ 0 };
		size_t _capacity{ 0 };

		template<typename T>
		struct base_iterator {
			T& operator*() noexcept {
				return *ptr;
			}
			T* operator->() noexcept {
				return ptr;
			}

			base_iterator& operator++() noexcept {
				ptr++;
				return *this;
			}
			base_iterator& operator++(int) noexcept {
				auto old_it = *this;
				++(*this);
				return old_it;
			}

			base_iterator& operator--() noexcept {
				ptr--;
				return *this;
			}
			base_iterator& operator--(int) noexcept {
				auto old_it = *this;
				--(*this);
				return old_it;
			}

			friend bool operator==(const base_iterator& a,
				const base_iterator& b) {
				return a.ptr == b.ptr;
			}
			friend bool operator!=(const base_iterator& a,
				const base_iterator& b) {
				return a.ptr != b.ptr;
			}

		private:
			T* ptr{ nullptr };
			explicit base_iterator(T* ptr) : ptr{ ptr } {}
		};

	public:
		using iterator = base_iterator<T>;
		using const_iterator = const base_iterator<const T>;

		Vector() = default;
		Vector(Vector&& rhs) {
			if (this != &rhs) {
				this->swap(rhs);
			}
		}
		Vector(const Vector& rhs) {
			if (this != &rhs) {
				reserve(rhs.capacity());
				for (size_t i = 0; i < rhs.size(); i++) {
					this->push_back(rhs[i]);
				}
			}
		}
		~Vector() {
			clear();
			core::free(arr);
			arr = nullptr;
		}

		Vector& operator=(Vector&& rhs) {
			if (this != &rhs) {
				this->swap(rhs);
			}
			return *this;
		}

		Vector& operator=(const Vector& rhs) {
			if (this != &rhs) {
				Vector r(rhs);
				this->swap(r);
			}
			return *this;
		}

		T& operator[](size_t index) noexcept {
			return arr[index];
		}

		const T& operator[](size_t index) const noexcept {
			return arr[index];
		}

		T& back() noexcept {
			return arr[_size - 1];
		}

		const T& back() const noexcept {
			return arr[_size - 1];
		}

		T& front() noexcept {
			return arr[0];
		}

		const T& front() const noexcept {
			return arr[0];
		}

		iterator& begin() noexcept {
			return iterator<T>(arr);
		}

		const_iterator& begin() const noexcept {
			return iterator<T>(arr);
		}

		iterator& end() noexcept {
			return iterator<T>(&arr[_size - 1]);
		}

		const_iterator& end() const noexcept {
			return iterator<T>(&arr[_size - 1]);
		}

		void pop_back() noexcept {
			arr[--_size].~T();
		}

		bool insert(iterator& it, const T& value) {
			if (_size == _capacity) {
				T* tmp = alloc_tmp();
				if (tmp == nullptr) return false;
				
				move_arr(tmp, get_sub(it, begin()));
				deleteArr();
				arr = tmp;
			}
			return true;
		}

		bool push_back(const T& value) {
			if (_size == _capacity) {
				T* tmp = alloc_tmp();

				if (tmp == nullptr) return false;

				new(&tmp[_size]) T(value);
				move_arr(tmp);
				deleteArr();
				arr = tmp;
			}
			else new(&arr[_size]) T(value);
			_size++;
			return true;
		}

		bool shrink_to_fit() {
			return resize_arr(_size);
		}

		bool reserve(size_t new_capacity) {
			if (new_capacity > _capacity) {
				return resize_arr(new_capacity);
			}
			return true;
		}

		void clear() noexcept {
			for (size_t i = 0; i < _size; i++) {
				(arr + i)->~T();
			}
			_size = 0;
		}

		void swap(Vector& rhs) {
			core::swap(this->arr, rhs.arr);
			core::swap(this->_size, rhs._size);
			core::swap(this->_capacity, rhs._capacity);
		}

		size_t size() const noexcept { return _size; }
		size_t capacity() const noexcept { return _capacity; }
		bool empty() const noexcept { return size() == 0; }
		T* data() noexcept { return arr; }
		const T* data() const noexcept { return arr; }

	private:
		size_t get_sub(iterator& it1, iterator& it2) {
			return (static_cast<size_t>(it1.operator->) - static_cast<size_t>(it2.operator->)) / sizeof size_t;
		}

		void deleteArr() {
			if (arr == nullptr)
				return;

			for (size_t i = 0; i < _size; i++) {
				(arr + i)->~T();
			}
			core::free(arr);
			arr = nullptr;
		}

		T* alloc_tmp(size_t new_capacity = -1) {
			if(new_capacity == -1) new_capacity = max(1, 2 * _capacity);
			T* tmp = core::alloc<T>(new_capacity);

			if (tmp != nullptr) {
				_capacity = new_capacity;
			}

			return tmp;
		}

		bool resize_arr(size_t new_capacity) {
			if (new_capacity > _capacity) {
				T* tmp = alloc_tmp(new_capacity);

				if (tmp == nullptr) {
					return false;
				}

				move_arr(tmp);
				deleteArr();
				arr = tmp;
			}
			else {
				_capacity = new_capacity;
			}
			return true;
		}

		void move_arr(T* tmp) {
			for (size_t i = 0; i < _size; i++) {
				new(&tmp[i]) T(core::move(arr[i]));
			}
		}
	};
}
