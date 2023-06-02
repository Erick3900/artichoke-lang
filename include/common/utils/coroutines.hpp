#pragma once

#include <coroutine>
#include <exception>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace arti::coro {

    template <typename T>
    class generator;

    namespace impl {

        template <typename T>
        struct generator_promise {
            using value_type = std::remove_reference_t<T>;
            using reference_type = value_type &;
            using pointer_type = value_type *;

            generator_promise() = default;

            generator<T> get_return_object();

            std::suspend_always initial_suspend() const noexcept {
                return {};
            }

            std::suspend_always final_suspend() const noexcept {
                return {};
            }

            void return_void() {
                return;
            }

            void unhandled_exception() {
                exception = std::current_exception();
            }

            void rethrow_if_exception() {
                if (exception) {
                    std::rethrow_exception(exception);
                }
            }

            std::suspend_always yield_value(value_type &v) noexcept {
                value = std::addressof(v);
                return {};
            }

            std::suspend_always yield_value(value_type &&v) noexcept {
                value = std::addressof(v);
                return {};
            }

            template <typename U>
            std::suspend_never await_transform(U &&value) = delete;

            std::exception_ptr exception;
            pointer_type value;
        };

        class generator_sentinel { };

        template <typename T>
        struct generator_iterator {
            using iterator_category = std::input_iterator_tag;

            using promise_type = impl::generator_promise<T>;

            using value_type = typename promise_type::value_type;
            using reference_type = value_type &;
            using pointer_type = value_type *;

            using handle_type = std::coroutine_handle<promise_type>;

            using difference_type = std::ptrdiff_t;

            generator_iterator() = default;
            ~generator_iterator() = default;

            generator_iterator(handle_type handle)
                : handle(handle) { }

            generator_iterator(generator_iterator &&rhs)
                : handle(std::exchange(rhs.handle, nullptr)) { }

            generator_iterator &operator=(generator_iterator &&rhs) {
                handle = std::exchange(rhs.handle, nullptr);
                return *this;
            }

            generator_iterator(const generator_iterator &) = delete;
            generator_iterator &operator=(const generator_iterator &) = delete;

            generator_iterator &operator++() {
                handle.resume();

                if (handle.done()) {
                    handle.promise().rethrow_if_exception();
                }

                return *this;
            }

            void operator++(int) {
                (void)this->operator++();
            }

            reference_type operator*() const noexcept {
                return *handle.promise().value;
            }

            pointer_type operator->() const noexcept {
                return handle.promise().value;
            }

            friend bool operator==(const generator_iterator &it, generator_sentinel) {
                return (!it.handle || it.handle.done());
            }

            friend bool operator!=(const generator_iterator &it, generator_sentinel s) {
                return !(it == s);
            }

            friend bool operator==(generator_sentinel s, const generator_iterator &it) {
                return it == s;
            }

            friend bool operator!=(generator_sentinel s, const generator_iterator &it) {
                return it != s;
            }

            handle_type handle;
        };

    }    // namespace impl

    template <typename T>
    class [[nodiscard]] generator {
      public:
        using promise_type = impl::generator_promise<T>;
        using handle_type = std::coroutine_handle<promise_type>;

        using iterator = impl::generator_iterator<T>;
        using iterator_sentinel = impl::generator_sentinel;

        generator() = default;

        ~generator() {
            if (m_Handle) {
                m_Handle.destroy();
            }
        }

        explicit generator(handle_type handle)
            : m_Handle(handle) { }

        generator(generator &&rhs) noexcept
            : m_Handle(std::exchange(rhs.m_Handle, nullptr)) { }

        generator &operator=(generator &&rhs) noexcept {
            m_Handle = std::exchange(rhs.m_Handle, nullptr);
            return *this;
        }

        generator(const generator &) = delete;
        generator &operator=(const generator &) = delete;

        impl::generator_iterator<T> begin() const {
            if (m_Handle) {
                m_Handle.resume();

                if (m_Handle.done()) {
                    m_Handle.promise().rethrow_if_exception();
                }
            }
            return { m_Handle };
        }

        impl::generator_sentinel end() const {
            return {};
        }

        void swap(generator &other) noexcept {
            std::swap(m_Handle, other.m_Handle);
        }

      private:
        handle_type m_Handle;
    };

    namespace impl {
        template <typename T>
        generator<T> generator_promise<T>::get_return_object() {
            using generator_type = generator<T>;
            using handle_type = typename generator_type::handle_type;

            return generator_type(handle_type::from_promise(*this));
        }
    }    // namespace impl

    template <typename T>
    class recursive_generator;

    namespace impl {

        template <typename T>
        struct recursive_generator_promise {
            using value_type = std::remove_reference_t<T>;
            using reference_type = value_type &;
            using pointer_type = value_type *;

            recursive_generator_promise() noexcept
                : value(nullptr)
                , exception(nullptr)
                , root(this)
                , parentOrLeaf(this) { }

            recursive_generator_promise(recursive_generator_promise &&) = delete;
            recursive_generator_promise(const recursive_generator_promise &) = delete;

            recursive_generator<T> get_return_object();

            std::suspend_always initial_suspend() noexcept {
                return {};
            }

            std::suspend_always final_suspend() noexcept {
                return {};
            }

            void return_void() {
                return;
            }

            void unhandled_exception() {
                exception = std::current_exception();
            }

            void rethrow_if_exception() {
                if (exception) {
                    std::rethrow_exception(exception);
                }
            }

            std::suspend_always yield_value(value_type &v) noexcept {
                value = std::addressof(v);
                return {};
            }

            std::suspend_always yield_value(value_type &&v) noexcept {
                value = std::addressof(v);
                return {};
            }

            auto yield_value(recursive_generator<T> &&generator) noexcept {
                return yield_value(generator);
            }

            auto yield_value(recursive_generator<T> &generator) noexcept {
                struct awaitable {
                    using promise_type = recursive_generator_promise<T>;

                    awaitable(promise_type *childPromise)
                        : m_ChildPromise(childPromise) { }

                    bool await_ready() noexcept {
                        return m_ChildPromise == nullptr;
                    }

                    void await_suspend(std::coroutine_handle<promise_type>) noexcept { }

                    void await_resume() {
                        if (m_ChildPromise != nullptr) {
                            m_ChildPromise->rethrow_if_exception();
                        }
                    }

                  private:
                    promise_type *m_ChildPromise;
                };

                if (generator.m_Promise != nullptr) {
                    root->parentOrLeaf = generator.m_Promise;
                    generator.m_Promise->root = root;
                    generator.m_Promise->parentOrLeaf = this;

                    generator.m_Promise->resume();

                    if (!generator.m_Promise->is_complete()) {
                        return awaitable{ generator.m_Promise };
                    }

                    root->parentOrLeaf = this;
                }

                return awaitable{ nullptr };
            }

            template <typename U>
            std::suspend_never await_transform(U &&value) = delete;

            void destroy() noexcept {
                std::coroutine_handle<recursive_generator_promise<T>>::from_promise(*this).destroy();
            }

            bool is_complete() noexcept {
                return std::coroutine_handle<recursive_generator_promise<T>>::from_promise(*this).done();
            }

            reference_type get() noexcept {
                return *(parentOrLeaf->value);
            }

            void pull() noexcept {
                parentOrLeaf->resume();

                while (parentOrLeaf != this && parentOrLeaf->is_complete()) {
                    parentOrLeaf = parentOrLeaf->parentOrLeaf;
                    parentOrLeaf->resume();
                }
            }

            void resume() noexcept {
                std::coroutine_handle<recursive_generator_promise<T>>::from_promise(*this).resume();
            }

            pointer_type value;
            std::exception_ptr exception;

            recursive_generator_promise<T> *root;
            recursive_generator_promise<T> *parentOrLeaf;
        };

        class recursive_generator_sentinel { };

        template <typename T>
        struct recursive_generator_iterator {
            using iterator_category = std::input_iterator_tag;

            using promise_type = impl::recursive_generator_promise<T>;

            using value_type = typename promise_type::value_type;
            using reference_type = value_type &;
            using pointer_type = value_type *;

            using difference_type = std::ptrdiff_t;

            recursive_generator_iterator() noexcept
                : promise(nullptr) { }

            ~recursive_generator_iterator() = default;

            explicit recursive_generator_iterator(promise_type *promise) noexcept
                : promise(promise) { }

            recursive_generator_iterator(recursive_generator_iterator &&rhs) noexcept
                : promise(std::exchange(rhs.promise, nullptr)) { }

            recursive_generator_iterator &operator=(recursive_generator_iterator &&rhs) noexcept {
                promise = std::exchange(rhs.promise, nullptr);
                return *this;
            }

            recursive_generator_iterator(const recursive_generator_iterator &) = delete;
            recursive_generator_iterator &operator=(const recursive_generator_iterator &) = delete;

            recursive_generator_iterator &operator++() {
                promise->pull();

                if (promise->is_complete()) {
                    auto *temp = promise;
                    promise = nullptr;
                    temp->rethrow_if_exception();
                }

                return *this;
            }

            void operator++(int) {
                (void)this->operator++();
            }

            reference_type operator*() const noexcept {
                return promise->get();
            }

            pointer_type operator->() const noexcept {
                return std::addressof(promise->get());
            }

            bool operator==(const recursive_generator_iterator &it) const noexcept {
                return (promise == it.promise);
            }

            bool operator!=(const recursive_generator_iterator &it) const noexcept {
                return (promise != it.promise);
            }

            friend bool operator==(const recursive_generator_iterator &self, recursive_generator_sentinel) noexcept {
                return self.promise == nullptr;
            }

            friend bool operator!=(const recursive_generator_iterator &self, recursive_generator_sentinel) noexcept {
                return self.promise != nullptr;
            }

            friend bool operator==(recursive_generator_sentinel, const recursive_generator_iterator &self) noexcept {
                return self.promise == nullptr;
            }

            friend bool operator!=(recursive_generator_sentinel, const recursive_generator_iterator &self) noexcept {
                return self.promise != nullptr;
            }

            promise_type *promise;
        };

    }    // namespace impl

    template <typename T>
    class [[nodiscard]] recursive_generator {
      public:
        friend struct impl::recursive_generator_promise<T>;

        using promise_type = impl::recursive_generator_promise<T>;
        using handle_type = std::coroutine_handle<promise_type>;

        using iterator = impl::recursive_generator_iterator<T>;

        recursive_generator() noexcept
            : m_Promise(nullptr) { }

        ~recursive_generator() {
            if (m_Promise) {
                m_Promise->destroy();
            }
        }

        explicit recursive_generator(promise_type &promise)
            : m_Promise(std::addressof(promise)) { }

        recursive_generator(recursive_generator &&rhs) noexcept
            : m_Promise(std::exchange(rhs.m_Promise, nullptr)) { }

        recursive_generator &operator=(recursive_generator &&rhs) noexcept {
            if (this != std::addressof(rhs)) {
                if (m_Promise) {
                    m_Promise->destroy();
                }

                m_Promise = std::exchange(rhs.m_Promise, nullptr);
            }
            return *this;
        }

        recursive_generator(const recursive_generator &) = delete;
        recursive_generator &operator=(const recursive_generator &) = delete;

        impl::recursive_generator_iterator<T> begin() const {
            if (m_Promise) {
                m_Promise->pull();

                if (!m_Promise->is_complete()) {
                    return iterator{ m_Promise };
                }

                m_Promise->rethrow_if_exception();
            }

            return iterator{ nullptr };
        }

        impl::recursive_generator_sentinel end() const {
            return {};
        }

        void swap(recursive_generator &other) noexcept {
            std::swap(m_Promise, other.m_Promise);
        }

      private:
        promise_type *m_Promise;
    };

    namespace impl {
        template <typename T>
        recursive_generator<T> recursive_generator_promise<T>::get_return_object() {
            return recursive_generator<T>{ *this };
        }
    }    // namespace impl

}    // namespace arti::coro

template <typename T>
inline constexpr bool std::ranges::enable_view<arti::coro::generator<T>> = true;

template <typename T>
inline constexpr bool std::ranges::enable_view<arti::coro::recursive_generator<T>> = true;
