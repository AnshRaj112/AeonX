#pragma once
namespace myc::runtime {
class RuntimeModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::runtime
