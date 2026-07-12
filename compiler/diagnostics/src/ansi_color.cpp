#include "myc/diagnostics/ansi_color.hpp"

namespace myc::diagnostics::ansi {

std::string Colorize(const std::string_view text, const std::string_view color,
                     const bool enabled) {
    if (!enabled || color.empty()) {
        return std::string(text);
    }
    return std::string(color) + std::string(text) + std::string(Reset);
}

std::string_view SeverityColor(const Severity severity) noexcept {
    switch (severity) {
        case Severity::Fatal:
        case Severity::Error:
            return Red;
        case Severity::Warning:
            return Yellow;
        case Severity::Note:
            return Blue;
        case Severity::Hint:
            return Cyan;
        case Severity::Info:
            return Gray;
    }
    return Reset;
}

}  // namespace myc::diagnostics::ansi
