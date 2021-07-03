#include "ClientPool.h"

namespace SlippiAuth {

    ClientPool::ClientPool(size_t size)
    {
        for (int i = 0; i < size; i++)
        {
            m_Ready.emplace(Client(i));
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
        m_InUse.push(std::move(m_Ready.top()));
        m_Ready.pop();

        auto& client = m_InUse.top();

        // TODO: Run this on another thread
        client.Start(connectCode);
    }

}
