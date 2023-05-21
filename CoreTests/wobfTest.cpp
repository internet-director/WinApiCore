#include "pch.h"

TEST(WobfTest, Kernel32) {
	ASSERT_TRUE(goodHash("kernel32.dll"));
}

TEST(WobfTest, Ntdll) {
	ASSERT_TRUE(goodHash("ntdll.dll"));
}

TEST(WobfTest, User32) {
	ASSERT_TRUE(goodHash("user32.dll"));
}

TEST(WobfTest, Advapi32) {
	ASSERT_TRUE(goodHash("advapi32.dll"));
}

TEST(WobfTest, Shlwapi) {
	ASSERT_TRUE(goodHash("shlwapi.dll"));
}

TEST(WobfTest, Gdi32) {
	ASSERT_TRUE(goodHash("gdi32.dll"));
}