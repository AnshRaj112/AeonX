#pragma once
namespace myc::backend {
class BackendModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::backend
