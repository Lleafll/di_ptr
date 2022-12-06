#include "forwarddeclared.h"
#include <catch.hpp>
#include <di/ptr.h>

namespace {

struct NotifyWhenDestroyed final {
    bool& destroyed;

    explicit NotifyWhenDestroyed(bool& destroyed) : destroyed{destroyed} {}

    ~NotifyWhenDestroyed() { destroyed = true; }
    NotifyWhenDestroyed(NotifyWhenDestroyed const&) = delete;
    NotifyWhenDestroyed(NotifyWhenDestroyed&&) noexcept = delete;
    NotifyWhenDestroyed& operator=(NotifyWhenDestroyed const&) = delete;
    NotifyWhenDestroyed& operator=(NotifyWhenDestroyed&&) noexcept = delete;
};

TEST_CASE("Destroy when owning") {
    auto destroyed = false;
    {
        auto const ptr = di::make_owning<NotifyWhenDestroyed>(destroyed);
        REQUIRE_FALSE(destroyed);
    }
    REQUIRE(destroyed);
}

TEST_CASE("Do not destroy when not owning") {
    auto destroyed = false;
    NotifyWhenDestroyed notifier{destroyed};
    {
        di::ptr const ptr{notifier};
        REQUIRE_FALSE(destroyed);
    }
    REQUIRE_FALSE(destroyed);
}

TEST_CASE("Copy construction and assignment are disabled even for trivial "
          "types") {
    using Type = di::ptr<int>;
    REQUIRE_FALSE(std::is_copy_constructible_v<Type>);
    REQUIRE_FALSE(std::is_copy_assignable_v<Type>);
}

struct CountDestructions final {
    int& number_of_destructions;

    explicit CountDestructions(int& number_of_destructions)
        : number_of_destructions{number_of_destructions} {}

    ~CountDestructions() { ++number_of_destructions; }
    CountDestructions(CountDestructions const&) = delete;
    CountDestructions(CountDestructions&&) noexcept = delete;
    CountDestructions& operator=(CountDestructions const&) = delete;
    CountDestructions& operator=(CountDestructions&&) noexcept = delete;
};

TEST_CASE("Move assignment does not double free when owning") {
    int number_of_destructions = 0;
    {
        auto ptr = di::make_owning<CountDestructions>(number_of_destructions);
        REQUIRE(number_of_destructions == 0);
        {
            [[maybe_unused]] auto ptr2 = std::move(ptr);
            REQUIRE(number_of_destructions == 0);
        }
        REQUIRE(number_of_destructions == 1);
    }
    REQUIRE(number_of_destructions == 1);
}

TEST_CASE("Move construction does not double free when owning") {
    int number_of_destructions = 0;
    {
        auto ptr = di::make_owning<CountDestructions>(number_of_destructions);
        REQUIRE(number_of_destructions == 0);
        {
            di::ptr<CountDestructions> const ptr2{std::move(ptr)};
            REQUIRE(number_of_destructions == 0);
        }
        REQUIRE(number_of_destructions == 1);
    }
    REQUIRE(number_of_destructions == 1);
}

TEST_CASE("Self-assignment while owning does not break destruction logic") {
    int number_of_destructions = 0;
    {
        auto ptr = di::make_owning<CountDestructions>(number_of_destructions);
        REQUIRE(number_of_destructions == 0);
#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
        ptr = std::move(ptr);
#ifdef __clang__
#pragma GCC diagnostic pop
#endif
        REQUIRE(number_of_destructions == 0);
    }
    REQUIRE(number_of_destructions == 1);
}

struct NotifyWhenMemberCalled final {
    bool called = false;

    void method() { called = true; }
};

TEST_CASE("Structure dereference operator") {
    NotifyWhenMemberCalled notifier;
    di::ptr ptr{notifier};
    REQUIRE_FALSE(notifier.called);
    ptr->method();
    REQUIRE(notifier.called);
}

TEST_CASE("Dereference operator") {
    NotifyWhenMemberCalled notifier;
    di::ptr ptr{notifier};
    REQUIRE_FALSE(notifier.called);
    (*ptr).method();
    REQUIRE(notifier.called);
}

struct Base {
    virtual ~Base() = default;

    virtual void method() = 0;
};

struct Derived final : Base {
    bool& called;

    explicit Derived(bool& called) : called{called} {}

    ~Derived() override = default;

    void method() override { called = true; }
};

TEST_CASE("Can move assign to base class") {
    auto called = false;
    auto derived = di::make_owning<Derived>(called);
    di::ptr<Base> base = std::move(derived);
    base->method();
    REQUIRE(called);
}

TEST_CASE("Self-move-assigning to base class does not break logic") {
    auto called = false;
    auto derived = di::make_owning<Derived>(called);
    di::ptr<Base> base{*derived};
    base = std::move(derived);
    base->method();
    REQUIRE(called);
}

TEST_CASE("Can move construct to base class") {
    auto called = false;
    auto derived = di::make_owning<Derived>(called);
    di::ptr<Base> base{std::move(derived)};
    base->method();
    REQUIRE(called);
}

TEST_CASE("is_owning does correctly return owning state") {
    REQUIRE(di::make_owning<int>(1).is_owning());
    int on_stack = 1;
    REQUIRE_FALSE(di::ptr{on_stack}.is_owning());
}

struct NotifyAddress final {
    explicit NotifyAddress(NotifyAddress*& address) { address = this; }
};

TEST_CASE("release correctly returns correct pointer and resets owning state") {
    NotifyAddress* address = nullptr;
    auto ptr = di::make_owning<NotifyAddress>(address);
    REQUIRE(ptr.is_owning());
    REQUIRE(address == ptr.release());
    REQUIRE_FALSE(ptr.is_owning());
}

TEST_CASE("Compiles with forward declaration") {
    ditest::ForwardDeclaredContainer const on_stack;
    REQUIRE(sizeof(on_stack) == 16);
}

}  // namespace