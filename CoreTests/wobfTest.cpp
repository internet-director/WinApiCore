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

TEST(WobfTest, Iphlpapi) {
	ASSERT_TRUE(goodHash("iphlpapi.dll"));
}

TEST(WobfTest, Urlmon) {
	ASSERT_TRUE(goodHash("urlmon.dll"));
}

TEST(WobfTest, Ws2_32) {
	ASSERT_TRUE(goodHash("ws2_32.dll"));
}

TEST(WobfTest, Crypt32) {
	ASSERT_TRUE(goodHash("crypt32.dll"));
}

TEST(WobfTest, Shell32) {
	ASSERT_TRUE(goodHash("shell32.dll"));
}

TEST(WobfTest, Ole32) {
	ASSERT_TRUE(goodHash("ole32.dll"));
}

TEST(WobfTest, Psapi) {
	ASSERT_TRUE(goodHash("psapi.dll"));
}

TEST(WobfTest, Cabinet) {
	ASSERT_TRUE(goodHash("cabinet.dll"));
}

TEST(WobfTest, Imagehlp) {
	ASSERT_TRUE(goodHash("imagehlp.dll"));
}

TEST(WobfTest, Netapi32) {
	ASSERT_TRUE(goodHash("netapi32.dll"));
}

TEST(WobfTest, Wtsapi32) {
	ASSERT_TRUE(goodHash("Wtsapi32.dll"));
}

TEST(WobfTest, Mpr) {
	ASSERT_TRUE(goodHash("Mpr.dll"));
}

TEST(WobfTest, WinHTTP) {
	ASSERT_TRUE(goodHash("WinHTTP.dll"));
}