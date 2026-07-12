#include "myc_test.hpp"

#include "myc/config/compiler_config.hpp"

MYC_TEST(config_default_profile_is_debug) {
    myc::config::CompilerConfig config;
    MYC_ASSERT_EQ(config.GetBuildProfile(), myc::config::BuildProfile::Debug);
    MYC_ASSERT_EQ(config.GetOptimizationLevel(), myc::config::OptimizationLevel::O0);
}

MYC_TEST(config_release_profile_sets_o3) {
    myc::config::CompilerConfig config;
    config.ApplyProfileDefaults(myc::config::BuildProfile::Release);
    MYC_ASSERT_EQ(config.GetOptimizationLevel(), myc::config::OptimizationLevel::O3);
}

MYC_TEST(config_gpu_placeholder_disabled_by_default) {
    myc::config::CompilerConfig config;
    MYC_ASSERT(!config.GetGpuConfiguration().enabled);
}

MYC_TEST(config_ai_placeholder_disabled_by_default) {
    myc::config::CompilerConfig config;
    MYC_ASSERT(!config.GetAiConfiguration().enabled);
}
