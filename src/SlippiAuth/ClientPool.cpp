#include "ClientPool.h"

namespace SlippiAuth {

    ClientPool::ClientPool(size_t size)
    {
        for (int i = 0; i < size; i++)
        {
            m_Clients.emplace_back(i);
        }
    }

    ClientPool::~ClientPool()
    {
        for (auto& thread : m_Threads)
        {
            if (thread.joinable())
                thread.join();
            else
                CORE_ERROR("Cannot join thread");
        }
    }

    Client& ClientPool::FindReadyClient()
    {
        for (auto& client : m_Clients)
        {
            if (client.IsReady())
                return client;
        }
        CORE_ERROR("No clients are ready");
    }

    void ClientPool::StartClient(const std::string& connectCode)
    {
        auto& client = FindReadyClient();
        // Set the connect code which the client will connect to
        client.SetTargetConnectCode(connectCode);
        m_Threads.emplace_back(&Client::Start, std::ref(client));
    }
}
