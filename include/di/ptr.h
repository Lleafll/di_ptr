#pragma once

#include <utility>

namespace di {

template<class T>
class ptr final {
public:
    explicit(false) ptr(T& ptr) : ptr_{&ptr} {}

    explicit ptr(T* const ptr, bool const is_owning)
        : ptr_{ptr}, is_owning_{is_owning} {}

    ptr(ptr&& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(is_owning_, other.is_owning_);
    }

    ptr& operator=(ptr&& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(is_owning_, other.is_owning_);
        return *this;
    }

    template<std::derived_from<T> T2>
    ptr(ptr<T2>&& other) noexcept {
        is_owning_ = other.is_owning();
        ptr_ = other.release();
    }

    template<std::derived_from<T> T2>
    ptr& operator=(ptr<T2>&& other) noexcept {
        if (is_owning_) {
            delete ptr_;
        }
        is_owning_ = other.is_owning();
        ptr_ = other.release();
        return *this;
    }

    ptr(ptr const&) = delete;
    ptr& operator=(ptr const&) = delete;

    ~ptr() {
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

template<class T, class... Params>
ptr<T> make_owning(Params&&... params) {
    return ptr(new T{std::forward<Params>(params)...}, true);
}

}  // namespace di