#pragma once
namespace myc::debugger {
class DebuggerModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::debugger
