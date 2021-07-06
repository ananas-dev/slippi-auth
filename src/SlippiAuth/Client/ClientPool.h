#pragma once

#include "SlippiAuth/Client/Client.h"

namespace SlippiAuth {

    class ClientPool
    {
    public:
        explicit ClientPool(size_t size);
        ~ClientPool();

        void FreeThread(uint32_t index);

        int64_t FindReadyClientIndex();

        Client& StartClient(const std::string& connectCode);

    private:
        std::vector<Client> m_Clients;
        std::vector<std::thread> m_Threads;
    };

}
