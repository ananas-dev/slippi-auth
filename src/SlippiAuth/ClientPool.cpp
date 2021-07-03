#include "ClientPool.h"

namespace SlippiAuth {

    ClientPool::ClientPool(size_t size)
    {
        // Initialize ENet
        if (enet_initialize() != 0)
        {
            CORE_ERROR("An error occurred while initializing ENet!");
        }

        for (int i = 0; i < size; i++)
        {
            m_Ready.emplace_back(i);
        }
    }

    ClientPool::~ClientPool()
    {
        for (auto& thread : m_ThreadInUse)
        {
            if (thread.joinable())
                thread.join();
            else
                CORE_ERROR("Cannot join thread");
        }
    }

    void ClientPool::StartClient(const std::string& connectCode)
    {
        if (m_Ready.empty())
        {
            CORE_ERROR("Client limit reached");
            return;
        }

        // Move an available client into the InUse stack
        m_InUse.push_back(std::move(m_Ready.back()));
        m_Ready.pop_back();

        auto& client = m_InUse.back();

        // Set the connect code which the client will connect to
        client.SetTargetConnectCode(connectCode);
        m_ThreadInUse.emplace_back(&Client::Start, std::ref(client));
    }

}
