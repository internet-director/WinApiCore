#pragma once
#include <core/config.h>
#include <core/wobf/wobf.h>
#include <core/types.h>
#include <type_traits>

namespace core {
    struct bad_function_call {
        char const* what() const noexcept;
    };
    namespace function_details {
        template<typename R, typename... Args>
        struct storage;

        constexpr static size_t INPLACE_BUFFER_SIZE = sizeof(void*);
        constexpr static size_t INPLACE_BUFFER_ALIGNMENT = alignof(void*);
        using buffer_t = core::aligned_storage_t<
            INPLACE_BUFFER_SIZE,
            INPLACE_BUFFER_ALIGNMENT>;

        template<typename R, typename... Args>
        struct storage;

        template<typename T>
        constexpr static bool small_storage =
            sizeof(T) <= sizeof(void*)
            && core::is_nothrow_move_constructible<T>::value
            && (alignof(void*) % alignof(T) == 0 || alignof(void*) <= alignof(T));

        template<typename R, typename... Args>
        struct type_descriptor {
            using storage_t = storage<R, Args...>;
            void (*copy)(storage_t* dest, storage_t const* src);
            void (*move)(storage_t* dest, storage_t* src);
            R(*invoke)(storage_t const* src, Args...);
            void (*destroy)(storage_t*);
        };

        template<typename R, typename... Args>
        type_descriptor<R, Args...> const* empty_descriptor() {
            using storage_t = storage<R, Args...>;
            constexpr static type_descriptor<R, Args...> impl =
            {
                    [](storage_t* dest, storage_t const* src) {
                        dest->descriptor = src->descriptor;
                    },
                    [](storage_t* dest, storage_t* src) {
                        dest->descriptor = core::move(src->descriptor);
                    },
                    [](storage_t const* src, Args...) -> R {
                        throw bad_function_call();
                    },
                    [](storage_t*) {}
            };

            return &impl;
        }


        template<typename T, typename R, typename... Args>
        type_descriptor<R, Args...> const* get_descriptor() noexcept {
            using storage_t = storage<R, Args...>;

            constexpr static type_descriptor<R, Args...> impl = {
                [](storage_t* dest, storage_t const* src) {
                    if constexpr (small_storage<T>) {
                        new(&dest->buffer) T(*src->template target<T>());
                    }
                    else {
                        reinterpret_cast<void*&>(dest->buffer) = (new T(*src->template target<T>()));
                    }
                    dest->descriptor = src->descriptor;
                },
                [](storage_t* dest, storage_t* src) noexcept {
                    if constexpr (small_storage<T>) {
                        new(&dest->buffer) T(core::move(*src->template target<T>()));
                    }
                    else {
                        reinterpret_cast<void*&>(dest->buffer) = (src->template target<T>());
                    }
                    dest->descriptor = src->descriptor;
                    src->descriptor = empty_descriptor<R, Args...>();
                },
                [](storage_t const* src, Args... args) -> R {
                    return (*src->template target<T>())(core::forward<Args>(args)...);
                },
                [](storage_t* src) {
                    src->template target<T>()->~T();
                    src->descriptor = empty_descriptor<R, Args...>();
                }
            };

            return &impl;
        }


        template<typename R, typename... Args>
        struct storage {
            storage() noexcept
                : buffer(),
                descriptor(empty_descriptor<R, Args...>()) {}

            template<typename T>
            storage(T val)
                : buffer(),
                descriptor(get_descriptor<T, R, Args...>()) {
                if constexpr (small_storage<T>) {
                    new(&(buffer)) T(core::move(val));
                }
                else {
                    reinterpret_cast<void*&>(buffer) = new T(core::move(val));
                }
            }

            storage(storage const& other) {
                other.descriptor->copy(this, &other);
            }
            storage(storage&& other) noexcept {
                other.descriptor->move(this, &other);
            }
            storage& operator=(storage const& other) {
                if (this != &other) {
                    storage temp(other);
                    core::swap(buffer, temp.buffer);
                    core::swap(descriptor, temp.descriptor);
                }
                return *this;
            }
            storage& operator=(storage&& other) noexcept {
                if (this != &other) {
                    descriptor->destroy(this);
                    other.descriptor->move(this, &other);
                }
                return *this;
            }

            ~storage() {
                descriptor->destroy(this);
            }

            explicit operator bool() const noexcept {
                return descriptor != empty_descriptor<R, Args...>();
            }
            R invoke(Args... args) const {
                return (*descriptor->invoke)(this, core::forward<Args>(args)...);
            }
            template<typename T>
            T* target() noexcept {
                return const_cast<T*>(get_target<T>());
            }
            template<typename T>
            T const* target() const noexcept {
                return get_target<T>();
            }

        private:
            template<typename T>
            T const* get_target() const noexcept {
                if (get_descriptor<T, R, Args...>() != descriptor) return nullptr;
                if constexpr (small_storage<T>) {
                    return reinterpret_cast<T const*>(&buffer);
                }
                else {
                    return *reinterpret_cast<T* const*>(&buffer);
                }
            }

            template<typename RO, typename... ArgsO>
            friend type_descriptor<RO, ArgsO...> const* empty_descriptor();


            template<typename U, typename RO, typename... ArgsO>
            friend type_descriptor<RO, ArgsO...> const* get_descriptor() noexcept;

            core::aligned_storage_t<sizeof(void*), alignof(void*)> buffer;
            type_descriptor<R, Args...> const* descriptor;
        };
    }

    template<typename F>
    struct function;

    template<typename R, typename... Args>
    struct function<R(Args...)> {
        function() noexcept = default;

        function(function const& other) = default;
        function(function&& other) noexcept = default;

        function& operator=(function const& rhs) = default;
        function& operator=(function&& rhs) noexcept = default;

        template<typename T>
        function(T val)
            : fStorage(core::move(val)) {}

        ~function() = default;

        explicit operator bool() const noexcept {
            return static_cast<bool>(fStorage);
        }

        R operator()(Args... args) const {
            return fStorage.invoke(core::forward<Args>(args)...);
        }

        template<typename T>
        T* target() noexcept {
            return fStorage.template target<T>();
        }

        template<typename T>
        T const* target() const noexcept {
            return fStorage.template target<T>();
        }

    private:
        core::function_details::storage<R, Args...> fStorage;
    };
}
