#include "pch.h"

#include <iostream>
#include <deque>
#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <utility>
#include <vector>
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

TEST(Threadtest, Join) {
	int c = 0;
	core::thread th([&c]() {
		c++;
		});
	th.join();
	ASSERT_EQ(c, 1);
}


TEST(Threadtest, Destr) {
	int c = 0; 
	{
		core::thread th([&c]() {
			c++;
			});
	}
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
					if (rand() % 2) q.push(1), counter++;
					else q.pop(), counter--;
				}
			});
		}
	}

	ASSERT_EQ(counter, q.size());
}

class A {
	std::thread::id parent_id;
	unsigned int copy_cnt = 0u;
	unsigned int move_cnt = 0u;

public:
	explicit A(int ii) : parent_id(std::this_thread::get_id()), i(ii) {}
	A(const A& a) : parent_id(a.parent_id), copy_cnt(a.copy_cnt + 1u), move_cnt(a.move_cnt), i(a.i) {
		assert(std::this_thread::get_id() == a.parent_id);
	}
	A(A&& a) : parent_id(a.parent_id), copy_cnt(a.copy_cnt), move_cnt(a.move_cnt + 1u), i(a.i) {
		assert(std::this_thread::get_id() == a.parent_id);
	}

	bool is_copy_move_correct() const {
		return copy_cnt + move_cnt <= 1u;
	}

	int i;
};
/*
TEST(Threadtest, Tests) {
	std::vector<core::thread> t;
	std::unique_ptr<int> p = std::make_unique<int>(-1);
	// Check if std::thread ctor accepts move-only arguments.
	t.emplace_back([](std::unique_ptr<int> p) { assert(*p == -1); }, std::move(p));

	// Check if std::async works with function that returns move-only type
	auto f = std::async([]() { return std::make_unique<int>(42); });
	p = f.get();
	assert(*p == 42);

	// Check if std::promise works with move-only types.
	std::promise<std::unique_ptr<int>> prom;
	t.emplace_back([&]() { prom.set_value(std::make_unique<int>(43)); });
	p = prom.get_future().get();
	assert(*p == 43);

	// Check if reference unwrapping occurs (it should not).
	int i = -1;
	t.emplace_back([](std::reference_wrapper<int> ri) { assert(ri == -1); }, std::ref(i));

	// Check if the copy occurs on the parent thread, as required by the Standard.
	A a(-1);
	t.emplace_back([](const A& an_a) { assert(an_a.i == -1); }, a);

	// Check if the move occurs on the parent thread, as required by the Standard.
	t.emplace_back([](const A& an_a) { assert(an_a.i == -1); }, std::move(a));

	// Check if we are doing the correct number of copies.
	A b(-1);
	t.emplace_back([](const A& an_a) { assert(an_a.is_copy_move_correct() && an_a.i == -1); }, b);

	// Check if we are doing the correct number of moves.
	t.emplace_back([](const A& an_a) { assert(an_a.is_copy_move_correct() && an_a.i == -1); }, std::move(b));

	for (auto&& th : t) {
		th.join();
	}
}*/