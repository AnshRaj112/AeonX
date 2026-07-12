#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <ostream>
#include <string_view>
#include <vector>

namespace myc::logging {

/// Log severity levels ordered from least to most verbose.
enum class LogLevel : std::uint8_t {
    Error = 0,
    Warning,
    Info,
    Debug,
    Trace,
};

[[nodiscard]] constexpr std::string_view ToString(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Trace:
            return "TRACE";
    }
    return "UNKNOWN";
}

/// Abstract sink for log output; enables future replacement (file, JSON, etc.).
class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void Write(LogLevel level, std::string_view message) = 0;
};

/// Writes log messages to a standard output stream.
class StreamLogSink final : public LogSink {
public:
    explicit StreamLogSink(std::ostream& stream);

    void Write(LogLevel level, std::string_view message) override;

private:
    std::ostream& stream_;
};

/// Thread-safe logging facade with pluggable sinks.
class Logger {
public:
    Logger();

    void SetLevel(LogLevel level);
    [[nodiscard]] LogLevel GetLevel() const;

    void AddSink(std::shared_ptr<LogSink> sink);
    void ClearSinks();

    void Log(LogLevel level, std::string_view message);

    void Error(std::string_view message) { Log(LogLevel::Error, message); }
    void Warning(std::string_view message) { Log(LogLevel::Warning, message); }
    void Info(std::string_view message) { Log(LogLevel::Info, message); }
    void Debug(std::string_view message) { Log(LogLevel::Debug, message); }
    void Trace(std::string_view message) { Log(LogLevel::Trace, message); }

    /// Returns the process-wide default logger instance.
    static Logger& Instance();

private:
    mutable std::mutex mutex_;
    LogLevel level_{LogLevel::Info};
    std::vector<std::shared_ptr<LogSink>> sinks_;
};

}  // namespace myc::logging
