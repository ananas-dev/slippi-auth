#pragma once

#include "SlippiAuth/Core.h"

namespace SlippiAuth {

    using Json = nlohmann::json;

    class ClientConfig
    {
    public:
        ClientConfig(const ClientConfig&) = delete;

        static ClientConfig& GetInstance()
        {
            static ClientConfig s_Instance;
            return s_Instance;
        }

        static void Load(const std::string& path) { GetInstance().ILoad(path); }
        static const Json& Get() { return GetInstance().IGet(); }
    private:
        void ILoad(const std::string& path);
        const Json& IGet() { return m_Data; }
        ClientConfig() = default;

        Json m_Data;

        static ClientConfig s_Instance;
    };

}