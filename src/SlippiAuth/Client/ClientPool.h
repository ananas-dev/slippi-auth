#pragma once

#include "SlippiAuth/Client/Client.h"
#include "SlippiAuth/Core.h"

namespace SlippiAuth {

    class ClientPool
    {
    public:
        explicit ClientPool();
        ~ClientPool();

        void FreeThread(uint32_t index);

        int64_t FindReadyClientIndex();

        void StartClient(const std::string& connectCode);

        std::vector<Client>& GetClients()
        {
            return m_Clients;
        }

    private:
        uint32_t m_PoolSize = ClientConfig::Get().size();
        std::vector<Client> m_Clients;
        std::vector<std::thread> m_Threads;

        // Default timeout is 5 min
        std::chrono::seconds m_Timeout = 3000s;
    };

}
