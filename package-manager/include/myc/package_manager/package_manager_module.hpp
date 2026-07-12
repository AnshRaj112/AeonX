#pragma once
namespace myc::package_manager {
class PackageManagerModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::package_manager
