#pragma once

#include <di/ptr.h>

namespace ditest {
struct ForwardDeclared;
}  // namespace ditest

namespace ditest {

struct ForwardDeclaredContainer final {
    di::ptr<ForwardDeclared> member;

    ForwardDeclaredContainer();

    ~ForwardDeclaredContainer();
    ForwardDeclaredContainer(ForwardDeclaredContainer const&) = delete;
    ForwardDeclaredContainer(ForwardDeclaredContainer&&) noexcept = default;
    ForwardDeclaredContainer&
    operator=(ForwardDeclaredContainer const&) = delete;
    ForwardDeclaredContainer&
    operator=(ForwardDeclaredContainer&&) noexcept = default;
};

}  // namespace ditest