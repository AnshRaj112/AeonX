#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace myc::config {

/// Build profile controlling debug info and default optimization.
enum class BuildProfile : std::uint8_t {
    Debug,
    Release,
    RelWithDebInfo,
    MinSizeRel,
    Custom,
};

[[nodiscard]] constexpr std::string_view ToString(BuildProfile profile) noexcept {
    switch (profile) {
        case BuildProfile::Debug:
            return "debug";
        case BuildProfile::Release:
            return "release";
        case BuildProfile::RelWithDebInfo:
            return "relwithdebinfo";
        case BuildProfile::MinSizeRel:
            return "minsizerel";
        case BuildProfile::Custom:
            return "custom";
    }
    return "unknown";
}

/// Optimization level for code generation passes.
enum class OptimizationLevel : std::uint8_t {
    O0,
    O1,
    O2,
    O3,
    Os,
    Oz,
};

[[nodiscard]] constexpr std::string_view ToString(OptimizationLevel level) noexcept {
    switch (level) {
        case OptimizationLevel::O0:
            return "O0";
        case OptimizationLevel::O1:
            return "O1";
        case OptimizationLevel::O2:
            return "O2";
        case OptimizationLevel::O3:
            return "O3";
        case OptimizationLevel::Os:
            return "Os";
        case OptimizationLevel::Oz:
            return "Oz";
    }
    return "unknown";
}

/// Target CPU architecture.
enum class TargetArchitecture : std::uint8_t {
    Unknown,
    X86_64,
    AArch64,
    Wasm32,
    Wasm64,
    RiscV64,
};

[[nodiscard]] constexpr std::string_view ToString(TargetArchitecture arch) noexcept {
    switch (arch) {
        case TargetArchitecture::Unknown:
            return "unknown";
        case TargetArchitecture::X86_64:
            return "x86_64";
        case TargetArchitecture::AArch64:
            return "aarch64";
        case TargetArchitecture::Wasm32:
            return "wasm32";
        case TargetArchitecture::Wasm64:
            return "wasm64";
        case TargetArchitecture::RiscV64:
            return "riscv64";
    }
    return "unknown";
}

/// Target operating system and ABI.
enum class TargetPlatform : std::uint8_t {
    Unknown,
    Linux,
    Windows,
    MacOS,
    Wasm,
    BareMetal,
};

[[nodiscard]] constexpr std::string_view ToString(TargetPlatform platform) noexcept {
    switch (platform) {
        case TargetPlatform::Unknown:
            return "unknown";
        case TargetPlatform::Linux:
            return "linux";
        case TargetPlatform::Windows:
            return "windows";
        case TargetPlatform::MacOS:
            return "macos";
        case TargetPlatform::Wasm:
            return "wasm";
        case TargetPlatform::BareMetal:
            return "baremetal";
    }
    return "unknown";
}

/// Controls diagnostic emission behavior.
struct DiagnosticConfiguration {
    bool warnings_as_errors{false};
    bool suppress_warnings{false};
    bool show_notes{true};
    bool show_hints{true};
    bool error_recovery{true};
    std::uint32_t max_diagnostics{0};  // 0 = unlimited
    std::uint32_t error_limit{0};       // 0 = unlimited
    std::uint32_t warning_limit{0};
};

/// Placeholder for future GPU backend configuration.
struct GpuConfiguration {
    bool enabled{false};
    std::string device_type;
    std::uint32_t max_workgroup_size{0};
};

/// Placeholder for future AI-assisted compilation settings.
struct AiConfiguration {
    bool enabled{false};
    std::string model_path;
    std::optional<float> temperature;
};

/// Immutable compiler configuration loaded before compilation begins.
class CompilerConfig {
public:
    CompilerConfig();

    [[nodiscard]] BuildProfile GetBuildProfile() const noexcept;
    [[nodiscard]] OptimizationLevel GetOptimizationLevel() const noexcept;
    [[nodiscard]] TargetArchitecture GetTargetArchitecture() const noexcept;
    [[nodiscard]] TargetPlatform GetTargetPlatform() const noexcept;
    [[nodiscard]] const DiagnosticConfiguration& GetDiagnosticConfiguration() const noexcept;
    [[nodiscard]] const GpuConfiguration& GetGpuConfiguration() const noexcept;
    [[nodiscard]] const AiConfiguration& GetAiConfiguration() const noexcept;

    void SetBuildProfile(BuildProfile profile);
    void SetOptimizationLevel(OptimizationLevel level);
    void SetTargetArchitecture(TargetArchitecture architecture);
    void SetTargetPlatform(TargetPlatform platform);
    void SetDiagnosticConfiguration(DiagnosticConfiguration config);
    void SetGpuConfiguration(GpuConfiguration config);
    void SetAiConfiguration(AiConfiguration config);

    /// Applies sensible defaults for the given build profile.
    void ApplyProfileDefaults(BuildProfile profile);

private:
    BuildProfile build_profile_{BuildProfile::Debug};
    OptimizationLevel optimization_level_{OptimizationLevel::O0};
    TargetArchitecture target_architecture_{TargetArchitecture::Unknown};
    TargetPlatform target_platform_{TargetPlatform::Unknown};
    DiagnosticConfiguration diagnostic_config_{};
    GpuConfiguration gpu_config_{};
    AiConfiguration ai_config_{};
};

}  // namespace myc::config
