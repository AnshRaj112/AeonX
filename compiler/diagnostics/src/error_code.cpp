#include "myc/diagnostics/error_code.hpp"

#include <format>

namespace myc::diagnostics {

ErrorCode ErrorCode::FromNumber(const std::uint32_t number) {
    return ErrorCode(std::format("MYC{:04}", number));
}

}  // namespace myc::diagnostics
