#define CATCH_CONFIG_MAIN

#include "mop/maybe_owning_ptr.h"
#include <catch2/catch.hpp>
#include <mop/maybe_owning_ptr.h>

namespace {

struct NotifyWhenDestroyed final {
    bool& destroyed;

    ~NotifyWhenDestroyed() { destroyed = true; }
};

TEST_CASE("Destroy when owning") {
    auto destroyed = false;
    {
        mop::maybe_owning_ptr<NotifyWhenDestroyed> const ptr{destroyed};
        REQUIRE_FALSE(destroyed);
    }
    REQUIRE(destroyed);
}

TEST_CASE("Do not destroy when not owning") {
    auto destroyed = false;
    NotifyWhenDestroyed notifier{destroyed};
    {
        mop::maybe_owning_ptr const ptr{notifier};
        REQUIRE_FALSE(destroyed);
    }
    REQUIRE_FALSE(destroyed);
}

TEST_CASE("Copy construction and assignment are disabled even for trivial "
          "types") {
    using Type = mop::maybe_owning_ptr<int>;
    REQUIRE_FALSE(std::is_copy_constructible_v<Type>);
    REQUIRE_FALSE(std::is_copy_assignable_v<Type>);
}

struct NotifyWhenDestroyedAndDoubleFreed final {
    bool& destroyed;
    bool& double_freed;

    ~NotifyWhenDestroyedAndDoubleFreed() {
        if (destroyed) {
            double_freed = true;
        }
        destroyed = true;
    }
};

TEST_CASE("Move assignment does not double free when owning") {
    auto destroyed = false;
    auto double_freed = false;
    {
        mop::maybe_owning_ptr<NotifyWhenDestroyedAndDoubleFreed> ptr{
                destroyed, double_freed};
        REQUIRE_FALSE(destroyed);
        {
            auto ptr2 = std::move(ptr);
            REQUIRE_FALSE(destroyed);
        }
        REQUIRE(destroyed);
        REQUIRE_FALSE(double_freed);
    }
    REQUIRE(destroyed);
    REQUIRE_FALSE(double_freed);
}

TEST_CASE("Move construction does not double free when owning") {
    auto destroyed = false;
    auto double_freed = false;
    {
        mop::maybe_owning_ptr<NotifyWhenDestroyedAndDoubleFreed> ptr{
                destroyed, double_freed};
        REQUIRE_FALSE(destroyed);
        {
            mop::maybe_owning_ptr<NotifyWhenDestroyedAndDoubleFreed> ptr2{
                    std::move(ptr)};
            REQUIRE_FALSE(destroyed);
        }
        REQUIRE(destroyed);
        REQUIRE_FALSE(double_freed);
    }
    REQUIRE(destroyed);
    REQUIRE_FALSE(double_freed);
}

TEST_CASE("Self-assignment while owning does not break destruction logic") {
    auto destroyed = false;
    auto double_freed = false;
    {
        mop::maybe_owning_ptr<NotifyWhenDestroyedAndDoubleFreed> ptr{
                destroyed, double_freed};
        REQUIRE_FALSE(destroyed);
        ptr = std::move(ptr);
        REQUIRE_FALSE(destroyed);
        REQUIRE_FALSE(double_freed);
    }
    REQUIRE(destroyed);
    REQUIRE_FALSE(double_freed);
}

}  // namespace