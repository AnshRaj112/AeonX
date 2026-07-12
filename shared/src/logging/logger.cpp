#include "myc/logging/logger.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace myc::logging {

StreamLogSink::StreamLogSink(std::ostream& stream) : stream_(stream) {}

void StreamLogSink::Write(LogLevel level, std::string_view message) {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time{};
#if defined(_WIN32)
    localtime_s(&local_time, &time);
#else
    localtime_r(&time, &local_time);
#endif

    std::ostringstream timestamp;
    timestamp << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");

    stream_ << '[' << timestamp.str() << "] [" << ToString(level) << "] "
            << message << '\n';
}

Logger::Logger() {
    sinks_.push_back(std::make_shared<StreamLogSink>(std::cerr));
}

void Logger::SetLevel(LogLevel level) {
    std::lock_guard lock(mutex_);
    level_ = level;
}

LogLevel Logger::GetLevel() const {
    std::lock_guard lock(mutex_);
    return level_;
}

void Logger::AddSink(std::shared_ptr<LogSink> sink) {
    std::lock_guard lock(mutex_);
    sinks_.push_back(std::move(sink));
}

void Logger::ClearSinks() {
    std::lock_guard lock(mutex_);
    sinks_.clear();
}

void Logger::Log(LogLevel level, std::string_view message) {
    std::lock_guard lock(mutex_);
    if (level > level_) {
        return;
    }
    for (const auto& sink : sinks_) {
        sink->Write(level, message);
    }
}

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

}  // namespace myc::logging
