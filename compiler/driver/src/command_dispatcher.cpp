#include "myc/driver/command_dispatcher.hpp"

#include "myc/logging/logger.hpp"
#include "myc/version/version.hpp"

#include <iostream>

namespace myc::driver {

CommandDispatcher::CommandDispatcher(config::CompilerConfig& config,
                                   diagnostics::DiagnosticEngine& diagnostics,
                                   source::SourceManager& source_manager)
    : config_(config), diagnostics_(diagnostics), source_manager_(source_manager) {}

int CommandDispatcher::Dispatch(const CliOptions& options) {
    if (options.show_help) {
        std::cout << CliParser::GetUsage();
        return 0;
    }

    if (options.show_version) {
        std::cout << version::GetFullVersionString() << '\n';
        return 0;
    }

    if (options.command.empty()) {
        std::cerr << "error: no command specified\n"
                  << "Run 'myc --help' for usage.\n";
        return 2;
    }

    if (options.command == "build") {
        return RunBuild(options);
    }
    if (options.command == "run") {
        return RunRun(options);
    }
    if (options.command == "fmt") {
        return RunFmt(options);
    }
    if (options.command == "lint") {
        return RunLint(options);
    }
    if (options.command == "benchmark") {
        return RunBenchmark(options);
    }

    std::cerr << "error: unknown command '" << options.command << "'\n"
              << "Run 'myc --help' for usage.\n";
    return 2;
}

int CommandDispatcher::RunBuild(const CliOptions& options) {
    logging::Logger::Instance().Info("build: compilation not yet implemented");
    if (!options.source_files.empty()) {
        for (const auto& file : options.source_files) {
            logging::Logger::Instance().Info("  input: " + file);
            (void)source_manager_.LoadFile(file);
        }
    }
    (void)config_;
    (void)diagnostics_;
    return 0;
}

int CommandDispatcher::RunRun(const CliOptions& options) {
    logging::Logger::Instance().Info("run: execution not yet implemented");
    if (!options.source_files.empty()) {
        logging::Logger::Instance().Info("  entry: " + options.source_files.front());
    }
    (void)config_;
    (void)diagnostics_;
    (void)source_manager_;
    return 0;
}

int CommandDispatcher::RunFmt(const CliOptions& options) {
    logging::Logger::Instance().Info("fmt: formatter not yet implemented");
    for (const auto& file : options.source_files) {
        logging::Logger::Instance().Info("  file: " + file);
    }
    return 0;
}

int CommandDispatcher::RunLint(const CliOptions& options) {
    logging::Logger::Instance().Info("lint: linter not yet implemented");
    for (const auto& file : options.source_files) {
        logging::Logger::Instance().Info("  file: " + file);
    }
    return 0;
}

int CommandDispatcher::RunBenchmark(const CliOptions& options) {
    logging::Logger::Instance().Info("benchmark: benchmarks not yet implemented");
    (void)options;
    return 0;
}

}  // namespace myc::driver
