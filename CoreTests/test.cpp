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

TEST(ProcessTest, EmptyInitially) {
	core::Process process;
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_EQ(process.getName(), nullptr);
	EXPECT_FALSE(process.isOpen());
	EXPECT_FALSE(process.isExist());

	EXPECT_FALSE(process.open());
	EXPECT_FALSE(process.kill());
	EXPECT_FALSE(process.suspend());
	EXPECT_FALSE(process.resume());
	EXPECT_NO_THROW(process.close());
}

TEST(ProcessTest, WrongInitiallyByPid) {
	core::Process process(-1);
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_EQ(process.getName(), nullptr);
	EXPECT_FALSE(process.isOpen());
	EXPECT_FALSE(process.isExist());

	EXPECT_FALSE(process.open());
	EXPECT_FALSE(process.kill());
	EXPECT_FALSE(process.suspend());
	EXPECT_FALSE(process.resume());
}

TEST(ProcessTest, WrongInitiallyByName) {
	core::Process process(L"");
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_EQ(process.getName(), nullptr);
	EXPECT_FALSE(process.isOpen());
	EXPECT_FALSE(process.isExist());

	EXPECT_FALSE(process.open());
	EXPECT_FALSE(process.kill());
	EXPECT_FALSE(process.suspend());
	EXPECT_FALSE(process.resume());
	EXPECT_NO_THROW(process.close());
}

TEST(ProcessTest, FindWaiterByPid) {
	STARTUPINFO si{};
	PROCESS_INFORMATION pi{};
	EXPECT_TRUE(runWaiter(si, pi)) << "Unable to start the waiter.exe, maybe you should check the path to it";

	core::Process process(pi.dwProcessId);

	EXPECT_EQ(process.getPid(), pi.dwProcessId);
	EXPECT_FALSE(process.isOpen());
	EXPECT_TRUE(process.isExist());

	EXPECT_TRUE(process.open());
	EXPECT_TRUE(process.isOpen());
	EXPECT_TRUE(process.resume());
	bool suspendResult = process.suspend();
	EXPECT_EQ(process.resume(), suspendResult);
	EXPECT_TRUE(process.kill());

	EXPECT_EQ(process.getPid(), -1);
	EXPECT_EQ(process.getName(), nullptr);
	EXPECT_FALSE(process.isOpen());
	EXPECT_FALSE(process.isExist());

	EXPECT_FALSE(process.open());
	EXPECT_FALSE(process.kill());
	EXPECT_FALSE(process.suspend());
	EXPECT_FALSE(process.resume());

	killWaiter(si, pi);
}