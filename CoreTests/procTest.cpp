#include "pch.h"

TEST(ProcessMonitorTest, EmptyInitially) {
	core::ProcessMonitor process;
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_EQ(process.getTid(), -1);
	EXPECT_FALSE(process.isExist());
	EXPECT_EQ(process.getName(), nullptr);
}

TEST(ProcessMonitorTest, WrongInitiallyByPid) {
	core::ProcessMonitor process(-1);
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_EQ(process.getTid(), -1);
	EXPECT_FALSE(process.isExist());
	EXPECT_EQ(process.getName(), nullptr);
}

TEST(ProcessMonitorTest, WrongInitiallyByName) {
	core::ProcessMonitor process(L"");
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_EQ(process.getTid(), -1);
	EXPECT_FALSE(process.isExist());
	EXPECT_EQ(process.getName(), nullptr);
}

TEST(ProcessTest, EmptyInitially) {
	core::Process process;
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_FALSE(process.isOpen());

	EXPECT_FALSE(process.open(-1, -1));
	EXPECT_FALSE(process.kill());
	EXPECT_FALSE(process.suspend());
	EXPECT_FALSE(process.resume());
	EXPECT_NO_THROW(process.close());
}

TEST(ProcessTest, WrongInitiallyByPid) {
	core::Process process(-1);
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_FALSE(process.isOpen());

	EXPECT_FALSE(process.open(-1, -1));
	EXPECT_FALSE(process.kill());
	EXPECT_FALSE(process.suspend());
	EXPECT_FALSE(process.resume());
	EXPECT_NO_THROW(process.close());
}

TEST(ProcessTest, WrongInitiallyByName) {
	core::Process process(L"");
	EXPECT_EQ(process.getPid(), -1);
	EXPECT_FALSE(process.isOpen());

	EXPECT_FALSE(process.open(-1, -1));
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
	EXPECT_EQ(process.getTid(), pi.dwThreadId);
	EXPECT_TRUE(process.isOpen());

	EXPECT_TRUE(process.isOpen());
	EXPECT_TRUE(process.resume());
	bool suspendResult = process.suspend();
	EXPECT_EQ(process.resume(), suspendResult);
	EXPECT_TRUE(process.kill());

	EXPECT_EQ(process.getPid(), -1);
	EXPECT_EQ(process.getTid(), -1);
	EXPECT_FALSE(process.isOpen());

	EXPECT_FALSE(process.kill());
	EXPECT_FALSE(process.suspend());
	EXPECT_FALSE(process.resume());

	killWaiter(si, pi);
}