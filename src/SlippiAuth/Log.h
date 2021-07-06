#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace SlippiAuth
{
    class Log
    {
    public:
        static void Init(size_t clientPoolSize);

        inline static std::shared_ptr<spdlog::logger>& GetClientLogger(size_t core) { return s_ClientLoggers[core]; }
        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetWsServerLogger() { return s_WsServerLogger; }

    private:
        static std::vector<std::shared_ptr<spdlog::logger>> s_ClientLoggers;
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_WsServerLogger;
    };

}

// Core logs macro
#define CORE_TRACE(...) ::SlippiAuth::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)  ::SlippiAuth::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)  ::SlippiAuth::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...) ::SlippiAuth::Log::GetCoreLogger()->error(__VA_ARGS__)

// Clients logs macro
#define CLIENT_TRACE(client,...)  ::SlippiAuth::Log::GetClientLogger(client)->trace(__VA_ARGS__)
#define CLIENT_INFO(client, ...)  ::SlippiAuth::Log::GetClientLogger(client)->info(__VA_ARGS__)
#define CLIENT_WARN(client, ...)  ::SlippiAuth::Log::GetClientLogger(client)->warn(__VA_ARGS__)
#define CLIENT_ERROR(client, ...) ::SlippiAuth::Log::GetClientLogger(client)->error(__VA_ARGS__)

// Websocket logs macro
#define WS_TRACE(...) ::SlippiAuth::Log::GetWsServerLogger()->trace(__VA_ARGS__)
#define WS_INFO(...)  ::SlippiAuth::Log::GetWsServerLogger()->info(__VA_ARGS__)
#define WS_WARN(...)  ::SlippiAuth::Log::GetWsServerLogger()->warn(__VA_ARGS__)
#define WS_ERROR(...) ::SlippiAuth::Log::GetWsServerLogger()->trace(__VA_ARGS__)
