#include "ClientConfig.h"

namespace SlippiAuth {

    void ClientConfig::ILoad(const std::string& path)
    {
        // Read a JSON file
        std::ifstream clientsJson(path);
        clientsJson >> m_Data;
        clientsJson.close();
    }

}