#pragma once
namespace myc::hir {
class HirModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::hir
