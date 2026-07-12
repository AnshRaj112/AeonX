#pragma once
namespace myc::ide {
class IdeModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::ide
