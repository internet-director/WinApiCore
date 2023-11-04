#include "pch.h"
#include <thread>

#include <iostream>
#include <deque>
#include <Windows.h>

template<typename T>
struct queue {
    queue() = default;
    queue(size_t size) : queue(), MAX_SIZE(size) {}
    ~queue() = default;

    void push(const T& value) {
		section.enter();
        cv_full.wait(section, [&]() {
            return que.size() < MAX_SIZE;
            });

        que.push_back(value);
        cv_empty.notify_one();
		section.leave();
    }

    T pop() {
		section.enter();
		cv_empty.wait(section, [&]() {
			return !que.empty();
			});

        T value = que.front();
        que.pop_front();
		cv_full.notify_one();
		section.leave();
        return value;
    }

	size_t size() {
		return que.size();
	}

private:
	size_t MAX_SIZE = 10000;
	std::deque<T> que;
    core::critical_section section;
    core::condition_variable cv_empty, cv_full;
};

TEST(MutexTest, Empty) {
	core::mutex mut;
}

TEST(MutexTest, Random) {
	size_t counter = 0;
	core::mutex mut;
	std::vector<std::thread> vec(10);
	for (auto& it : vec) {
		it = std::thread([&counter, &mut]() {
			for (int i = 0; i < 100000; i++) {
				mut.lock();
				counter++;
				mut.unlock();
			}
		});
	}

	for (auto& it : vec) {
		it.join();
	}
	ASSERT_EQ(counter, 1000000);
}

TEST(CriticalSectionTest, Empty) {
	core::critical_section section;
	section.enter();
	section.leave();
}

// shitty test
TEST(ConditionVariableTest, Queue) {
	queue<int> q;
	std::atomic<size_t> counter = 5000;
	for (int i = 0; i < 5000; i++) q.push(i);
	{
		std::vector<core::thread> vec(10);
		for (auto& it : vec) {
			it = core::thread([&q, &counter]() {
				for (int i = 0; i < 10000; i++) {
					//std::cout << "asdadasdasd";
					if (rand() % 2) q.push(1), counter++;
					else q.pop(), counter--;
				}
			});
		}
	}
	ASSERT_EQ(counter, q.size());
}

TEST(Threadtest, Empty) {
	int c = 0;
	core::thread th([&c]() {
		c++;
		});
	th.join();
	ASSERT_EQ(c, 1);
}

TEST(Threadtest, ArgsTest) {
	int c = 0;
	core::thread th([&c](int a, const char* str) {
		c++;
		ASSERT_STREQ(str, "test_string");
		ASSERT_EQ(a, 123);
		ASSERT_NE(c, 0);
		}, 123, "test_string");
	th.join();
	ASSERT_EQ(c, 1);
}
