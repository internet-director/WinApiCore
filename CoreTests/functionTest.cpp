#include "pch.h"
#include <core/Vector.h>

TEST(function_test, default_ctor)
{
    core::function<void()> x;
    core::function<void(int, int, int)> y;
    core::function<double&& (float&, int const&, int)> z;
}

TEST(function_test, empty_convertion_to_bool)
{
    core::function<void()> x;
    EXPECT_FALSE(static_cast<bool>(x));
    EXPECT_THROW(x(), core::bad_function_call);
}

TEST(function_test, empty_call)
{
    core::function<void()> x;
    EXPECT_THROW(x(), core::bad_function_call);
}

TEST(function_test, empty_copy_move)
{
    core::function<void()> x;

    core::function<void()> y = x;
    EXPECT_FALSE(static_cast<bool>(y));

    core::function<void()> z = std::move(x);
    EXPECT_FALSE(static_cast<bool>(z));

    z = y;
    EXPECT_FALSE(static_cast<bool>(z));

    y = std::move(z);
    EXPECT_FALSE(static_cast<bool>(y));
}

TEST(function_test, ctor_func)
{
    core::function<int()> f = [] { return 42; };
    EXPECT_EQ(42, f());
}

TEST(function_test, ctor_copy)
{
    core::function<int()> f = [] { return 42; };
    core::function<int()> g = f;
    EXPECT_EQ(42, f());
    EXPECT_EQ(42, g());
}

struct small_func
{
    small_func(int value) noexcept
        : value(value)
    {}

    int operator()() const
    {
        return value;
    }

    int get_value() const
    {
        return value;
    }

private:
    int value;
};

TEST(function_test, empty_target)
{
    core::function<int()> f;
    EXPECT_EQ(nullptr, f.target<small_func>());
    EXPECT_EQ(nullptr, std::as_const(f).target<small_func>());
}

TEST(function_test, small_func)
{
    core::function<int()> f = small_func(42);
    EXPECT_EQ(42, f());
}

TEST(function_test, small_func_copy_ctor)
{
    core::function<int()> f = small_func(42);
    core::function<int()> g = f;
    EXPECT_EQ(42, f());
    EXPECT_EQ(42, g());
}

TEST(function_test, small_func_move_ctor)
{
    core::function<int()> f = small_func(42);
    core::function<int()> g = std::move(f);
    EXPECT_EQ(42, g());
}

TEST(function_test, small_func_assignment_operator)
{
    core::function<int()> f = small_func(42);
    core::function<int()> g;
    g = f;
    EXPECT_EQ(42, g());
}

TEST(function_test, small_func_assignment_operator_move)
{
    core::function<int()> f = small_func(42);
    core::function<int()> g;
    g = std::move(f);
    EXPECT_EQ(42, g());
}

TEST(function_test, small_func_assignment_operator_self)
{
    core::function<int()> f = small_func(42);
    f = f;
    EXPECT_EQ(42, f());
}

TEST(function_test, small_func_assignment_operator_move_self)
{
    core::function<int()> f = small_func(42);
    f = std::move(f);
    EXPECT_EQ(42, f());
}

TEST(function_test, small_func_target)
{
    core::function<int()> f = small_func(42);
    EXPECT_EQ(42, f.target<small_func>()->get_value());
    EXPECT_EQ(42, std::as_const(f).target<small_func>()->get_value());
}

struct large_func
{
    large_func(int value) noexcept
        : that(this)
        , value(value)
    {
        ++n_instances;
    }

    large_func(large_func const& other) noexcept
        : that(this)
        , value(other.value)
    {
        ++n_instances;
    }

    large_func& operator=(large_func const& rhs) noexcept
    {
        value = rhs.value;
        return *this;
    }

    ~large_func()
    {
        assert(this == that);
        --n_instances;
    }

    int operator()() const noexcept
    {
        assert(this == that);
        return value;
    }

    static void assert_no_instances()
    {
        assert(n_instances == 0);
    }

    int get_value() const
    {
        return value;
    }

private:
    large_func* that;
    int value;
    int payload[1000];

    static size_t n_instances;
};

size_t large_func::n_instances = 0;

TEST(function_test, large_func)
{
    {
        core::function<int()> f = large_func(42);
        EXPECT_EQ(42, f());
    }
    large_func::assert_no_instances();
}

TEST(function_test, large_func_copy_ctor)
{
    core::function<int()> f = large_func(42);
    core::function<int()> g = f;
    EXPECT_EQ(42, f());
    EXPECT_EQ(42, g());
}

TEST(function_test, large_func_move_ctor)
{
    core::function<int()> f = large_func(42);
    core::function<int()> g = std::move(f);
    EXPECT_EQ(42, g());
}

TEST(function_test, large_func_assignment_operator)
{
    core::function<int()> f = large_func(42);
    core::function<int()> g;
    g = f;
    EXPECT_EQ(42, g());
}

TEST(function_test, large_func_assignment_operator_move)
{
    core::function<int()> f = large_func(42);
    core::function<int()> g;
    g = std::move(f);
    EXPECT_EQ(42, g());
}

TEST(function_test, large_func_assignment_operator_self)
{
    core::function<int()> f = large_func(42);
    f = f;
    EXPECT_EQ(42, f());
}

TEST(function_test, large_func_assignment_operator_move_self)
{
    core::function<int()> f = large_func(42);
    f = std::move(f);
    EXPECT_EQ(42, f());
}

TEST(function_test, large_func_target)
{
    core::function<int()> f = large_func(42);
    EXPECT_EQ(42, f.target<large_func>()->get_value());
    EXPECT_EQ(42, std::as_const(f).target<large_func>()->get_value());
}

struct throwing_move
{
    struct exception final : std::exception
    {
        using std::exception::exception;
    };

    throwing_move() = default;

    int operator()() const
    {
        return 42;
    }

    throwing_move(throwing_move const&)
    {
        if (enable_exception)
            throw exception();
    }

    static bool enable_exception;
};

bool throwing_move::enable_exception = false;

TEST(function_test, throwing_move)
{
    core::function<int()> f = throwing_move();
    core::function<int()> g;
    throwing_move::enable_exception = true;
    try
    {
        EXPECT_NO_THROW(g = std::move(f));
    }
    catch (...)
    {
        throwing_move::enable_exception = false;
        throw;
    }
    throwing_move::enable_exception = false;
    EXPECT_EQ(42, g());
}

struct throwing_copy
{
    struct exception final : std::exception
    {
        using std::exception::exception;
    };

    throwing_copy() = default;

    int operator()() const
    {
        return 42;
    }

    throwing_copy(throwing_copy&&) noexcept
    {}

    throwing_copy(throwing_copy const&)
    {
        throw exception();
    }
};

TEST(function_test, throwing_copy)
{
    core::function<int()> f = large_func(42);
    core::function<int()> g = throwing_copy();

    EXPECT_THROW(f = g, throwing_copy::exception);
    EXPECT_EQ(42, f());
}

TEST(function_test, arguments)
{
    core::function<int(int, int)> f = [](int a, int b) { return a + b; };

    EXPECT_EQ(42, f(40, 2));
}

TEST(function_test, arguments_ref)
{
    int x = 42;
    core::function<int& (int&)> f = [](int& a) -> int& { return a; };

    EXPECT_EQ(&x, &f(x));
}

TEST(function_test, arguments_cref)
{
    int const x = 42;
    core::function<int const& (int const&)> f = [](int const& a) -> int const& { return a; };

    EXPECT_EQ(&x, &f(x));
}

struct non_copyable
{
    non_copyable()
    {}

    non_copyable(non_copyable const&) = delete;
    non_copyable(non_copyable&&) = default;
};

TEST(function_test, argument_by_value)
{
    core::function<non_copyable(non_copyable)> f = [](non_copyable a) { return std::move(a); };
    non_copyable a = f(non_copyable());
}

TEST(function_test, argument_by_value_large)
{
    int big_array[1000];
    core::function<non_copyable(non_copyable)> f = [big_array](non_copyable a) { return std::move(a); };
    non_copyable a = f(non_copyable());
}

struct foo
{
    void operator()() const
    {}
};

struct bar
{
    void operator()() const
    {}
};

TEST(function_test, target)
{
    core::function<void()> f = foo();
    EXPECT_NE(nullptr, f.target<foo>());
    EXPECT_EQ(nullptr, f.target<bar>());
    EXPECT_NE(nullptr, std::as_const(f).target<foo>());
    EXPECT_EQ(nullptr, std::as_const(f).target<bar>());
    f = bar();
    EXPECT_EQ(nullptr, f.target<foo>());
    EXPECT_NE(nullptr, f.target<bar>());
    EXPECT_EQ(nullptr, std::as_const(f).target<foo>());
    EXPECT_NE(nullptr, std::as_const(f).target<bar>());
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
