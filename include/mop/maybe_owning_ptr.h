#pragma once

#include <utility>

namespace mop {

template<class T>
class maybe_owning_ptr final {
public:
    template<class... Params>
    explicit maybe_owning_ptr(Params&&... params)
        : ptr_{new T{std::forward<Params>(params)...}}, is_owning_{true} {}

    explicit(false) maybe_owning_ptr(T& ptr) : ptr_{&ptr} {}

    template<std::derived_from<T> T2>
    explicit(false) maybe_owning_ptr(T2& ptr) : ptr_{&ptr} {}

    maybe_owning_ptr(maybe_owning_ptr&& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(is_owning_, other.is_owning_);
    }

    maybe_owning_ptr& operator=(maybe_owning_ptr&& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(is_owning_, other.is_owning_);
        return *this;
    }

    template<std::derived_from<T> T2>
    maybe_owning_ptr(maybe_owning_ptr<T2>&& other) noexcept {
        if (is_owning_) {
            delete ptr_;
        }
        is_owning_ = other.is_owning();
        ptr_ = other.release();
    }

    template<std::derived_from<T> T2>
    maybe_owning_ptr& operator=(maybe_owning_ptr<T2>&& other) noexcept {
        if (is_owning_) {
            delete ptr_;
        }
        is_owning_ = other.is_owning();
        ptr_ = other.release();
        return *this;
    }

    maybe_owning_ptr(maybe_owning_ptr const&) = delete;
    maybe_owning_ptr& operator=(maybe_owning_ptr const&) = delete;

    ~maybe_owning_ptr() {
        if (is_owning_) {
            delete ptr_;
        }
    }

    T* operator->() const { return ptr_; }

    T& operator*() const { return *ptr_; }

    T* release() {
        is_owning_ = false;
        return std::exchange(ptr_, nullptr);
    }

    [[nodiscard]] bool is_owning() const { return is_owning_; }

private:
    T* ptr_ = nullptr;
    bool is_owning_ = false;
};

}  // namespace mop