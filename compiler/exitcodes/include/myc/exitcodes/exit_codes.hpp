#pragma once

#include <cstdint>
#include <string_view>

namespace myc::exitcodes {

/// Standardized process exit codes for the Myc toolchain.
enum class ExitCode : int {
    Success = 0,
    GeneralError = 1,
    InvalidArguments = 2,
    FileNotFound = 3,
    ConfigurationError = 4,
    InternalCompilerError = 5,
    Reserved = 6,
};

[[nodiscard]] constexpr int ToInt(ExitCode code) noexcept {
    return static_cast<int>(code);
}

[[nodiscard]] constexpr std::string_view ToString(ExitCode code) noexcept {
    switch (code) {
        case ExitCode::Success:
            return "success";
        case ExitCode::GeneralError:
            return "general error";
        case ExitCode::InvalidArguments:
            return "invalid arguments";
        case ExitCode::FileNotFound:
            return "file not found";
        case ExitCode::ConfigurationError:
            return "configuration error";
        case ExitCode::InternalCompilerError:
            return "internal compiler error";
        case ExitCode::Reserved:
            return "reserved";
    }
    return "unknown";
}

}  // namespace myc::exitcodes
