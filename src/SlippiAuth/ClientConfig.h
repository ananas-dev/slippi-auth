#pragma once

#include <nlohmann/json.hpp>

namespace SlippiAuth {

    using json = nlohmann::json;

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
        static const json& Get() { return GetInstance().IGet(); }
    private:
        void ILoad(const std::string& path);
        const json& IGet() { return m_Data; }
        ClientConfig() = default;

        json m_Data;

        static ClientConfig s_Instance;
    };

}