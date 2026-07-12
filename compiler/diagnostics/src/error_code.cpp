#include "myc/diagnostics/error_code.hpp"

#include <format>

namespace myc::diagnostics {

ErrorCode ErrorCode::FromNumber(const std::uint32_t number) {
    return ErrorCode(std::format("MYC{:04}", number));
}

namespace codes {

const ErrorCode SourceFileNotFound{"MYC8001"};
const ErrorCode SourceFileReadError{"MYC8002"};
const ErrorCode SourceInvalidEncoding{"MYC8003"};
const ErrorCode SourcePathInvalid{"MYC8004"};

}  // namespace codes

}  // namespace myc::diagnostics
