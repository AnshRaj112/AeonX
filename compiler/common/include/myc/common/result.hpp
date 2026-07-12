#pragma once

#include <expected>
#include <string>
#include <utility>

namespace myc::common {

/// Unified result type for recoverable operations across the compiler driver.
template <typename T>
using Result = std::expected<T, std::string>;

}  // namespace myc::common
