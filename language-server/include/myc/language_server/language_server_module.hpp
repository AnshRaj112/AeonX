#pragma once
namespace myc::language_server {
class LanguageServerModule {
public:
    [[nodiscard]] static const char* Name() noexcept;
};
}  // namespace myc::language_server
