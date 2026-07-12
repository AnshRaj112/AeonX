#pragma once

#include "myc/types.hpp"

#include <compare>
#include <cstdint>

namespace myc::source {

/// Strong identifier for a loaded source file within a SourceManager.
class SourceFileID {
public:
    constexpr SourceFileID() noexcept = default;

    constexpr explicit SourceFileID(FileId value) noexcept : value_(value) {}

    [[nodiscard]] constexpr FileId Value() const noexcept { return value_; }
    [[nodiscard]] constexpr bool IsValid() const noexcept { return value_ != 0; }

    [[nodiscard]] constexpr auto operator<=>(const SourceFileID& other) const noexcept =
        default;

    [[nodiscard]] constexpr bool operator==(const SourceFileID& other) const noexcept = default;

private:
    FileId value_{0};
};

[[nodiscard]] constexpr SourceFileID InvalidSourceFileID() noexcept { return SourceFileID{}; }

}  // namespace myc::source
