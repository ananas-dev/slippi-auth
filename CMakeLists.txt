cmake_minimum_required(VERSION 3.19)
project(SlippiVerification)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

# Fetch dependencies
include(FetchContent)

FetchContent_Declare(cpr
        GIT_REPOSITORY https://github.com/whoshuu/cpr.git
        GIT_TAG c8d33915dbd88ad6c92b258869b03aba06587ff9)

FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.8.5
)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(enet
        GIT_REPOSITORY https://github.com/lsalzman/enet
        GIT_TAG v1.3.7
        )

FetchContent_MakeAvailable(enet)

# Files
file(GLOB SRCS
        "${SRC_DIR}/SlippiVerification/Application.cpp"
        )


add_executable(SlippiVerification ${SRCS})

# Precompiled header
target_precompile_headers(SlippiVerification PRIVATE "${SRC_DIR}/pch.h")

# Include directories
target_include_directories(SlippiVerification PUBLIC "${SRC_DIR}")

# Link dependencies

target_link_libraries(SlippiVerification PRIVATE
        cpr::cpr
        nlohmann_json
        spdlog::spdlog
        enet
        )