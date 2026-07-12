#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace myc::source {

/// Detected source file encoding.
enum class SourceEncoding : std::uint8_t {
  UTF8,
  Invalid,
};

[[nodiscard]] constexpr std::string_view ToString(SourceEncoding encoding) noexcept {
  switch (encoding) {
    case SourceEncoding::UTF8:
      return "UTF-8";
    case SourceEncoding::Invalid:
      return "invalid";
  }
  return "unknown";
}

/// Detects and validates source file encodings.
class EncodingDetector {
 public:
  /// Returns the detected encoding. Strips a UTF-8 BOM when present.
  [[nodiscard]] static SourceEncoding Detect(std::string_view content) noexcept;

  /// Validates UTF-8 byte sequences without modifying the input.
  [[nodiscard]] static bool IsValidUtf8(std::string_view content) noexcept;

  /// Returns the number of leading BOM bytes to skip (0 or 3).
  [[nodiscard]] static std::size_t BomLength(std::string_view content) noexcept;
};

}  // namespace myc::source
