#include "myc/driver/cli_parser.hpp"

#include <iostream>

namespace myc::driver {

namespace {

bool IsSourceFile(std::string_view arg) {
    return arg.size() >= 4 && arg.substr(arg.size() - 4) == ".myc";
}

bool IsFlag(std::string_view arg) { return !arg.empty() && arg[0] == '-'; }

}  // namespace

CliOptions CliParser::Parse(int argc, char* argv[]) {
    CliOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            options.show_help = true;
            return options;
        }
        if (arg == "--version" || arg == "-V") {
            options.show_version = true;
            return options;
        }

        if (options.command.empty() && !IsFlag(arg)) {
            options.command = arg;
            continue;
        }

        if (IsSourceFile(arg)) {
            options.source_files.push_back(arg);
        } else {
            options.positional_args.push_back(arg);
        }
    }

    return options;
}

std::string CliParser::GetUsage() {
    return R"(Myc — systems programming language compiler

Usage:
  myc [command] [options] [files...]

Commands:
  build       Compile .myc source files
  run         Build and run a .myc program
  fmt         Format source code
  lint        Run static analysis
  benchmark   Run compiler benchmarks

Options:
  -h, --help       Show this help message
  -V, --version    Show version information

Examples:
  myc build main.myc
  myc run examples/hello.myc
  myc fmt src/
)";
}

}  // namespace myc::driver
