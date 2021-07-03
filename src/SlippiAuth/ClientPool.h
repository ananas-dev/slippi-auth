#pragma once

#include "Client.h"

namespace SlippiAuth {

    class ClientPool
    {
    public:
        explicit ClientPool(size_t size);
        ~ClientPool();

        Client& FindReadyClient();

        void StartClient(const std::string& connectCode);

    private:
        std::vector<Client> m_Clients;
        std::vector<std::thread> m_Threads;
    };

}
