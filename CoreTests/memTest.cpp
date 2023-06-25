#include "pch.h"

TEST(ConfigTest, TypeCheck) {
	{
		bool res1 = std::is_same_v<std::int8_t, core::int8_t>;
		bool res2 = std::is_same_v<std::int16_t, core::int16_t>;
		bool res3 = std::is_same_v<std::int32_t, core::int32_t>;
		bool res4 = std::is_same_v<std::int64_t, core::int64_t>;

		ASSERT_TRUE(res1);
		ASSERT_TRUE(res2);
		ASSERT_TRUE(res3);
		ASSERT_TRUE(res4);
	}

	{
		bool res1 = std::is_same_v<std::uint8_t, core::uint8_t>;
		bool res2 = std::is_same_v<std::uint16_t, core::uint16_t>;
		bool res3 = std::is_same_v<std::uint32_t, core::uint32_t>;
		bool res4 = std::is_same_v<std::uint64_t, core::uint64_t>;

		ASSERT_TRUE(res1);
		ASSERT_TRUE(res2);
		ASSERT_TRUE(res3);
		ASSERT_TRUE(res4);
	}
}

TEST(MemoryTest, MemcmpEmtryArray) {
	const char* str1 = "";
	const char* str2 = "";

	EXPECT_EQ(core::memcmp(str1, str1, 1), std::memcmp(str1, str1, 1));
	EXPECT_EQ(core::memcmp(str2, str2, 1), std::memcmp(str2, str2, 1));

	EXPECT_EQ(core::memcmp(str1, str1, 0), std::memcmp(str1, str1, 0));
	EXPECT_EQ(core::memcmp(str2, str2, 1), std::memcmp(str2, str2, 1));

	EXPECT_EQ(core::memcmp(str1, str2, 0), std::memcmp(str1, str2, 0));
	EXPECT_EQ(core::memcmp(str2, str1, 1), std::memcmp(str2, str1, 1));
}

TEST(MemoryTest, MemcmpEqualArray) {
	const char* str1 = "asdasdasds";
	const char* str2 = "asdasdasds";
	size_t sz = strlen(str1);

	EXPECT_EQ(core::memcmp(str1, str1, sz), std::memcmp(str1, str1, sz));
	EXPECT_EQ(core::memcmp(str2, str2, sz), std::memcmp(str2, str2, sz));

	EXPECT_EQ(core::memcmp(str1, str2, sz), std::memcmp(str1, str2, sz));
	EXPECT_EQ(core::memcmp(str2, str1, sz), std::memcmp(str2, str1, sz));
}

TEST(MemoryTest, MemcmpDifflArray) {
	const char* str1 = "asdasdasds";
	const char* str2 = "asdasdasda";
	size_t sz = strlen(str1);

	EXPECT_EQ(core::memcmp(str1, str1, sz), std::memcmp(str1, str1, sz));
	EXPECT_EQ(core::memcmp(str2, str2, sz), std::memcmp(str2, str2, sz));

	EXPECT_EQ(core::memcmp(str1, str2, sz), std::memcmp(str1, str2, sz));
	EXPECT_EQ(core::memcmp(str2, str1, sz), std::memcmp(str2, str1, sz));
}

TEST(MemoryTest, Alloc) {
	return;
	for (int i = 0; i < 32; i++) {
		size_t sz = (1 << i);
		uint8_t* ptr = (uint8_t*)core::alloc(sz);

		if (ptr != nullptr) {
			core::zeromem(ptr, sz);
			Sleep(10);
			core::free(ptr);
		}
	}
}