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
};

}  // namespace ditest