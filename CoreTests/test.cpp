#include "pch.h"

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

TEST(ProcessTest, MoveConstructor) {
	core::Process process1(-1);
	core::Process process2(std::move(process1));

	EXPECT_EQ(process1.getPid(), -1);
	EXPECT_EQ(process1.getName(), nullptr);
	EXPECT_FALSE(process1.isOpen());
	EXPECT_FALSE(process1.isExist());

	EXPECT_FALSE(process1.open());
	EXPECT_FALSE(process1.kill());
	EXPECT_FALSE(process1.suspend());
	EXPECT_FALSE(process1.resume());
	EXPECT_NO_THROW(process1.close());
}