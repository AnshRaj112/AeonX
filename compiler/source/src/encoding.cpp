#include "myc/source/encoding.hpp"

namespace myc::source {

namespace {

constexpr unsigned char kUtf8Bom[] = {0xEF, 0xBB, 0xBF};

bool ValidateUtf8(std::string_view text) {
  const auto* bytes = reinterpret_cast<const unsigned char*>(text.data());
  std::size_t i = 0;
  while (i < text.size()) {
    const unsigned char byte = bytes[i];
    if (byte <= 0x7F) {
      ++i;
      continue;
    }

    std::size_t continuation = 0;
    unsigned int min_value = 0;
    unsigned int max_value = 0x10FFFF;

    if ((byte & 0xE0) == 0xC0) {
      continuation = 1;
      min_value = 0x80;
      max_value = 0x7FF;
    } else if ((byte & 0xF0) == 0xE0) {
      continuation = 2;
      min_value = 0x800;
      max_value = 0xFFFF;
    } else if ((byte & 0xF8) == 0xF0) {
      continuation = 3;
      min_value = 0x10000;
      max_value = 0x10FFFF;
    } else {
      return false;
    }

    if (i + continuation >= text.size()) {
      return false;
    }

    unsigned int codepoint = byte & ((1U << (6 - continuation)) - 1U);
    for (std::size_t j = 1; j <= continuation; ++j) {
      if ((bytes[i + j] & 0xC0) != 0x80) {
        return false;
      }
      codepoint = (codepoint << 6) | (bytes[i + j] & 0x3F);
    }

    if (codepoint < min_value || codepoint > max_value) {
      return false;
    }

    i += continuation + 1;
  }
  return true;
}

}  // namespace

std::size_t EncodingDetector::BomLength(std::string_view content) noexcept {
  if (content.size() >= 3 && static_cast<unsigned char>(content[0]) == kUtf8Bom[0] &&
      static_cast<unsigned char>(content[1]) == kUtf8Bom[1] &&
      static_cast<unsigned char>(content[2]) == kUtf8Bom[2]) {
    return 3;
  }
  return 0;
}

bool EncodingDetector::IsValidUtf8(std::string_view content) noexcept {
  return ValidateUtf8(content);
}

SourceEncoding EncodingDetector::Detect(std::string_view content) noexcept {
  const std::size_t bom = BomLength(content);
  const std::string_view payload = content.substr(bom);
  return ValidateUtf8(payload) ? SourceEncoding::UTF8 : SourceEncoding::Invalid;
}

}  // namespace myc::source
