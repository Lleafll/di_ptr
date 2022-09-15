#include "forwarddeclared.h"

namespace ditest {

struct ForwardDeclared {};

ForwardDeclaredContainer::ForwardDeclaredContainer()
    : member{di::make_owning<ForwardDeclared>()} {}

ForwardDeclaredContainer::~ForwardDeclaredContainer() = default;

}  // namespace ditest