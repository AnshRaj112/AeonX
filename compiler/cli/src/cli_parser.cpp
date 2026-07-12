#include "myc/cli/cli_parser.hpp"

#include <string_view>

namespace myc::cli {

namespace {

bool IsSourceFile(std::string_view arg) {
    return arg.size() >= 4 && arg.ends_with(".myc");
}

bool IsFlag(std::string_view arg) { return !arg.empty() && arg[0] == '-'; }

std::optional<std::string> SplitOptionValue(std::string_view arg,
                                            std::string_view prefix) {
    if (!arg.starts_with(prefix)) {
        return std::nullopt;
    }
    return std::string(arg.substr(prefix.size()));
}

bool ParseGlobalFlag(std::string_view arg, CliOptions& options, int argc, int& index,
                     char* argv[], std::string& error) {
    if (arg == "--help" || arg == "-h") {
        options.show_help = true;
        return true;
    }
    if (arg == "--version" || arg == "-V") {
        options.show_version = true;
        return true;
    }
    if (arg == "--verbose" || arg == "-v") {
        options.verbose = true;
        return true;
    }
    if (arg == "--quiet" || arg == "-q") {
        options.quiet = true;
        return true;
    }
    if (arg == "--color") {
        options.color_mode = ColorMode::Always;
        return true;
    }
    if (arg == "--no-color") {
        options.color_mode = ColorMode::Never;
        return true;
    }
    if (arg == "--json") {
        options.json_output = true;
        return true;
    }
    if (arg == "--time") {
        options.time_report = true;
        return true;
    }
    if (arg == "--trace") {
        options.trace = true;
        return true;
    }
    if (arg == "--diagnostics") {
        options.diagnostics_output = true;
        return true;
    }

    if (auto value = SplitOptionValue(arg, "--profile=")) {
        options.profile = std::move(*value);
        return true;
    }
    if (arg == "--profile") {
        if (index + 1 >= argc) {
            error = "missing value for --profile";
            return false;
        }
        options.profile = argv[++index];
        return true;
    }

    if (auto value = SplitOptionValue(arg, "--target=")) {
        options.target = std::move(*value);
        return true;
    }
    if (arg == "--target") {
        if (index + 1 >= argc) {
            error = "missing value for --target";
            return false;
        }
        options.target = argv[++index];
        return true;
    }

    if (auto value = SplitOptionValue(arg, "--platform=")) {
        options.platform = std::move(*value);
        return true;
    }
    if (arg == "--platform") {
        if (index + 1 >= argc) {
            error = "missing value for --platform";
            return false;
        }
        options.platform = argv[++index];
        return true;
    }

    if (arg == "-O0" || arg == "-O1" || arg == "-O2" || arg == "-O3" || arg == "-Os" ||
        arg == "-Oz") {
        options.optimization = std::string(arg.substr(1));
        return true;
    }
    if (auto value = SplitOptionValue(arg, "-O")) {
        options.optimization = std::move(*value);
        return true;
    }
    if (auto value = SplitOptionValue(arg, "--opt=")) {
        options.optimization = std::move(*value);
        return true;
    }

    if (arg.starts_with("--") && arg.find('=') != std::string_view::npos) {
        const auto eq = arg.find('=');
        const std::string key(arg.substr(2, eq - 2));
        const std::string value(arg.substr(eq + 1));
        options.options.emplace(key, value);
        return true;
    }

    if (arg.starts_with("--")) {
        options.unknown_flags.emplace_back(arg);
        return true;
    }

    if (arg.starts_with('-')) {
        options.unknown_flags.emplace_back(arg);
        return true;
    }

    return false;
}

}  // namespace

common::Result<CliOptions> CliParser::Parse(int argc, char* argv[]) {
    CliOptions options;
    std::string error;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (IsFlag(arg)) {
            if (!ParseGlobalFlag(arg, options, argc, i, argv, error)) {
                return std::unexpected(error);
            }
            continue;
        }

        if (options.command.empty()) {
            if (IsSourceFile(arg)) {
                options.source_files.push_back(arg);
            } else {
                options.command = arg;
            }
            continue;
        }

        if (IsSourceFile(arg)) {
            options.source_files.push_back(arg);
            continue;
        }

        options.positional_args.push_back(arg);
    }

    if (options.verbose && options.quiet) {
        return std::unexpected(
            "cannot use --verbose and --quiet at the same time");
    }

    return options;
}

std::string CliParser::GetUsage() {
    return R"(Myc — systems programming language toolchain

Usage:
  myc [global-options] <command> [command-options] [files...]

Global Options:
  -h, --help            Show usage information
  -V, --version         Show version information
  -v, --verbose         Enable verbose output
  -q, --quiet           Suppress non-essential output
      --color           Force colored output
      --no-color        Disable colored output
      --json            Emit machine-readable JSON output (future)
      --profile <name>  Select build profile (debug, release, ...)
      --target <triple> Select target architecture
      --platform <name> Select target platform
  -O<level>             Optimization level (O0, O1, O2, O3, Os, Oz)
      --time            Report timing information (future)
      --trace           Enable trace logging
      --diagnostics     Emit structured diagnostics (future)

Commands:
  build       Compile .myc source files
  run         Build and run a .myc program
  fmt         Format source code
  lint        Run static analysis
  benchmark   Run compiler benchmarks
  test        Run project tests
  docs        Generate documentation
  new         Create a new project
  clean       Remove build artifacts
  doctor      Diagnose toolchain installation
  package     Manage packages
  init        Initialize a project manifest
  repl        Interactive read-eval-print loop
  ast         Dump abstract syntax trees
  tokens      Dump lexer tokens
  ir          Dump intermediate representation
  cfg         Dump control-flow graphs
  help        Show help for a command

Examples:
  myc build main.myc
  myc run examples/hello.myc
  myc --profile release build src/
  myc fmt -- src/*.myc
)";
}

std::string CliParser::GetVersionUsage() {
    return "myc --version\n";
}

}  // namespace myc::cli
