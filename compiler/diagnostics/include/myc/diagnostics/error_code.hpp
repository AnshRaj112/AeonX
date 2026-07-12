#pragma once

#include <compare>
#include <cstdint>
#include <string>
#include <string_view>

namespace myc::diagnostics {

/// Unique diagnostic identifier (e.g. "MYC0001").
class ErrorCode {
public:
    constexpr explicit ErrorCode(std::string_view code) : code_(code) {}

    [[nodiscard]] static ErrorCode FromNumber(std::uint32_t number);

    [[nodiscard]] std::string_view GetCode() const noexcept { return code_; }
    [[nodiscard]] const std::string& AsString() const noexcept { return code_; }

    [[nodiscard]] auto operator<=>(const ErrorCode& other) const = default;

private:
    std::string code_;
};

/// Centrally managed diagnostic codes. Extend this registry as phases are added.
namespace codes {

inline const ErrorCode UnexpectedToken{"MYC0001"};
inline const ErrorCode EmptyExpression{"MYC0002"};
inline const ErrorCode InvalidSyntax{"MYC0100"};
inline const ErrorCode TypeMismatch{"MYC1000"};

extern const ErrorCode SourceFileNotFound;
extern const ErrorCode SourceFileReadError;
extern const ErrorCode SourceInvalidEncoding;
extern const ErrorCode SourcePathInvalid;

inline const ErrorCode DriverFileNotFound{"MYC9001"};
inline const ErrorCode DriverNoCommand{"MYC9002"};
inline const ErrorCode DriverUnknownCommand{"MYC9003"};
inline const ErrorCode DriverInvalidProfile{"MYC9004"};
inline const ErrorCode DriverInvalidTarget{"MYC9005"};
inline const ErrorCode DriverInvalidPlatform{"MYC9006"};
inline const ErrorCode DriverInvalidOptimization{"MYC9007"};
inline const ErrorCode DriverInternalError{"MYC9999"};

}  // namespace codes

}  // namespace myc::diagnostics
