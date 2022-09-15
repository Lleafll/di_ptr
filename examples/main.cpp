#include <di/ptr.h>
#include <iostream>
#include <memory>
#include <string_view>

namespace {

struct Logger {
    virtual ~Logger() = default;

    virtual void log(std::string_view message) = 0;
};

struct StreamLogger final : Logger {
    ~StreamLogger() override = default;

    void log(std::string_view const message) override {
        std::cout << message << '\n';
    }
};

struct ReferenceDI final {
    Logger& logger;
};

void reference_di() {
    // Only non-ownership possible
    StreamLogger logger;
    ReferenceDI di{logger};
}

struct SharedDI final {
    std::shared_ptr<Logger> logger;
};

void shared_di() {
    // Shared ownership (e.g. in tests)
    auto logger = std::make_shared<StreamLogger>();
    SharedDI shared_di{logger};
    // Effectively exclusive ownership
    SharedDI unshared_di{std::make_shared<StreamLogger>()};
    // Non-ownership not possible
    // Client dictates how service is allocated, but does not prevent copy
    // construction and assignment of client
}

struct UniqueDI final {
    std::unique_ptr<Logger> logger;
};

void unique_di() {
    // Observing (e.g. in tests), leaves us with moved-from container
    auto container = std::make_unique<StreamLogger>();
    auto* const observing = container.get();
    UniqueDI observed_unique_di{std::move(container)};
    // Exclusive ownership
    UniqueDI unique_di{std::make_unique<StreamLogger>()};
    // Non-ownership not possible
    // Client dictates how service is allocated
}

struct MaybeOwningDI final {
    di::ptr<Logger> logger;
};

void maybe_owning_di() {
    // Non-ownership (e.g. tests), also avoids heap allocation
    StreamLogger logger;
    MaybeOwningDI non_owning{logger};
    // Exclusive ownership
    MaybeOwningDI owning{di::make_owning<StreamLogger>()};
}

}  // namespace

int main() {
    reference_di();
    shared_di();
    unique_di();
    maybe_owning_di();
}