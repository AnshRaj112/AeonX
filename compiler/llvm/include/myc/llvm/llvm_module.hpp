#pragma once
namespace myc::llvm_backend {
class LlvmModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::llvm_backend
