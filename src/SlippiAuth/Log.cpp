#include "Log.h"

namespace SlippiAuth {

    std::vector<std::shared_ptr<spdlog::logger>> Log::s_ClientLoggers;
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ServerLogger;

    void Log::Init(size_t clientPoolSize)
    {
        spdlog::set_pattern("%^ [%T] %n: %v%$");

        // Allocate enough memory to hold all the loggers
        s_ClientLoggers.reserve(clientPoolSize);

        for (int index = 0; index < clientPoolSize; index++)
        {
            s_ClientLoggers.emplace_back(spdlog::stdout_color_mt("CLIENT (" + std::to_string(index) + ")"));
            s_ClientLoggers[index]->set_level(spdlog::level::trace);
        }

        s_CoreLogger = spdlog::stdout_color_mt("CORE");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ServerLogger = spdlog::stdout_color_mt("SERVER");
        s_ServerLogger->set_level(spdlog::level::trace);
    }

}
