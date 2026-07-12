#pragma once
namespace myc::linter {
class LinterModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::linter
