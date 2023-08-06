#pragma once
#include <core/config.h>
#include <core/types.h>
#include <core/mem.h>

namespace core {
	namespace hash_details {
		template<typename T = uint32_t>
		class hash {
			T _hash = 0;

		public:
			constexpr explicit hash() noexcept = default;
			constexpr explicit hash(T seed) noexcept : _hash{ seed } { }
			template<typename C>
			constexpr explicit hash(const C* dst) noexcept(core::is_same_v<C, char> || core::is_same_v<C, wchar_t>) {
				_hash = calculate(dst);
			}

			constexpr T getHash() const noexcept { return _hash; }
			template<typename C>
			constexpr T getHash(const C* dst)
				noexcept(core::is_same_v<C, char> || core::is_same_v<C, wchar_t>)
			{
				return getHash(dst, core::strlen(dst));
			}
			template<typename C>
			constexpr T getHash(const C* dst, size_t sz)
				noexcept(core::is_same_v<C, char> || core::is_same_v<C, wchar_t>)
			{
				return _hash = calculate(dst, sz);
			}
			constexpr T getHash(const void* dst, size_t sz) noexcept {
				return _hash = calculate<uint8_t>(dst, sz);
			}
			constexpr void clearLocalVariable() noexcept { _hash = 0; }

			template<typename C>
			constexpr static T calculate(const C* dst)
				noexcept(core::is_same_v<C, char> || core::is_same_v<C, wchar_t>) {
				return calculate<C>(dst, core::strlen(dst));
			}
			template<typename C>
			constexpr static T calculate(const C* dst, size_t sz) noexcept;

			constexpr explicit operator T() const noexcept
			{
				return _hash;
			}
		};
		template<typename T>
		template<typename C>
		inline constexpr static T hash<T>::calculate(const C* dst, size_t sz) noexcept
		{
			T h = 0;
			if (dst == nullptr || sz == 0) {
				return h;
			}
			for (size_t i = 0; i < sz; i++) {
				C letter = dst[i];
				if (letter >= C('A') && letter <= C('Z')) letter -= C('A') - C('a');
				h = (h << 6) + (h << 16) - h + letter;
			}
			return h;
		}
	}

	class hash32 : public hash_details::hash<uint32_t>
	{
	};

	class hash64 : public hash_details::hash<uint64_t>
	{
	};

	class hash : public hash_details::hash<size_t> 
	{
	};
}