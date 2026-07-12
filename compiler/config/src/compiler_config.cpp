#include "myc/config/compiler_config.hpp"

namespace myc::config {

CompilerConfig::CompilerConfig() { ApplyProfileDefaults(BuildProfile::Debug); }

BuildProfile CompilerConfig::GetBuildProfile() const noexcept { return build_profile_; }

OptimizationLevel CompilerConfig::GetOptimizationLevel() const noexcept {
    return optimization_level_;
}

TargetArchitecture CompilerConfig::GetTargetArchitecture() const noexcept {
    return target_architecture_;
}

TargetPlatform CompilerConfig::GetTargetPlatform() const noexcept {
    return target_platform_;
}

const DiagnosticConfiguration& CompilerConfig::GetDiagnosticConfiguration() const noexcept {
    return diagnostic_config_;
}

const GpuConfiguration& CompilerConfig::GetGpuConfiguration() const noexcept {
    return gpu_config_;
}

const AiConfiguration& CompilerConfig::GetAiConfiguration() const noexcept {
    return ai_config_;
}

void CompilerConfig::SetBuildProfile(BuildProfile profile) { build_profile_ = profile; }

void CompilerConfig::SetOptimizationLevel(OptimizationLevel level) {
    optimization_level_ = level;
}

void CompilerConfig::SetTargetArchitecture(TargetArchitecture architecture) {
    target_architecture_ = architecture;
}

void CompilerConfig::SetTargetPlatform(TargetPlatform platform) {
    target_platform_ = platform;
}

void CompilerConfig::SetDiagnosticConfiguration(DiagnosticConfiguration config) {
    diagnostic_config_ = std::move(config);
}

void CompilerConfig::SetGpuConfiguration(GpuConfiguration config) {
    gpu_config_ = std::move(config);
}

void CompilerConfig::SetAiConfiguration(AiConfiguration config) {
    ai_config_ = std::move(config);
}

void CompilerConfig::ApplyProfileDefaults(BuildProfile profile) {
    build_profile_ = profile;
    switch (profile) {
        case BuildProfile::Debug:
            optimization_level_ = OptimizationLevel::O0;
            break;
        case BuildProfile::Release:
            optimization_level_ = OptimizationLevel::O3;
            break;
        case BuildProfile::RelWithDebInfo:
            optimization_level_ = OptimizationLevel::O2;
            break;
        case BuildProfile::MinSizeRel:
            optimization_level_ = OptimizationLevel::Os;
            break;
        case BuildProfile::Custom:
            break;
    }
}

}  // namespace myc::config
