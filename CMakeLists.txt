cmake_minimum_required(VERSION 3.11)
project(SlippiAuth)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

# Copy clients.json into the bin dir
if (EXISTS "${CMAKE_SOURCE_DIR}/clients.json")
    configure_file("${CMAKE_SOURCE_DIR}/clients.json" "${CMAKE_BINARY_DIR}/clients.json" COPYONLY)
endif()

# Fetch dependencies
include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.9.1)

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
        GIT_TAG v1.3.17
        )

FetchContent_MakeAvailable(enet)

FetchContent_Declare(websocketpp
        GIT_REPOSITORY https://github.com/zaphoyd/websocketpp
        GIT_TAG 56123c87598f8b1dd471be83ca841ceae07f95ba
        )

FetchContent_MakeAvailable(websocketpp)

FetchContent_Declare(asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio
        GIT_TAG asio-1-18-2
        )

FetchContent_MakeAvailable(asio)

file(GLOB SRCS
        "${SRC_DIR}/SlippiAuth/Application.cpp"
        "${SRC_DIR}/SlippiAuth/Log.cpp"
        "${SRC_DIR}/SlippiAuth/Client/ClientConfig.cpp"
        "${SRC_DIR}/SlippiAuth/Client/Client.cpp"
        "${SRC_DIR}/SlippiAuth/Client/ClientPool.cpp"
        "${SRC_DIR}/SlippiAuth/WebSocketServer/WebSocketServer.cpp"
        )

add_executable(SlippiAuth ${SRCS} src/SlippiAuth/main.cpp)

# Precompiled header
target_precompile_headers(SlippiAuth PRIVATE "${SRC_DIR}/pch.h")

# Include directories
target_include_directories(SlippiAuth PUBLIC "${SRC_DIR}")

# Dependencies includes dir
target_include_directories(SlippiAuth PUBLIC
        "${enet_SOURCE_DIR}/include"
        "${websocketpp_SOURCE_DIR}"
        "${asio_SOURCE_DIR}/asio/include"
        )

# Link dependencies
target_link_libraries(SlippiAuth PRIVATE
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        enet
        )