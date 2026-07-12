add_library(myc_shared STATIC
    logging/logger.cpp
    version/version_info.cpp
)

target_include_directories(myc_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

string(TIMESTAMP MYC_BUILD_DATE "%Y-%m-%d %H:%M:%S" UTC)

target_compile_definitions(myc_shared PRIVATE
    MYC_BUILD_DATE="${MYC_BUILD_DATE}"
)

myc_set_target_standard(myc_shared)
myc_set_warnings(myc_shared)

add_library(myc::shared ALIAS myc_shared)
