#include "ClientPool.h"

namespace SlippiAuth {

    ClientPool::ClientPool(size_t size)
    {
        // Initialize ENet
        if (enet_initialize() != 0)
        {
            CORE_ERROR("An error occurred while initializing ENet!");
        }

        m_Clients.reserve(size);
        m_Threads.reserve(size);
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

    void ClientPool::FreeThread(uint32_t index)
    {
        if (m_Threads[index].joinable())
            m_Threads[index].join();

        m_Threads.erase(std::next(m_Threads.begin(), index));
    }

    int64_t ClientPool::FindReadyClientIndex()
    {
        for (auto& client : m_Clients)
        {
            if (client.IsReady())
                return client.GetId();
        }
        CORE_ERROR("No clients are ready");
        return -1;
    }

    Client& ClientPool::StartClient(const std::string& connectCode)
    {
        uint64_t clientIndex = FindReadyClientIndex();
        if (clientIndex != -1)
        {
            auto& client = m_Clients[FindReadyClientIndex()];

            // Set the connect code which the client will connect to
            client.SetTargetConnectCode(connectCode);
            m_Threads.emplace_back(&Client::Start, std::ref(client));

            return client;
        }
    }

}
