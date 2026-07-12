#pragma once
namespace myc::ir {
class IrModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::ir
