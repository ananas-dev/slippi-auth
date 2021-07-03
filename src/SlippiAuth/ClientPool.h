#pragma once

#include "Client.h"

namespace SlippiAuth {

    class ClientPool
    {
    public:
        explicit ClientPool(size_t size);

        size_t GetReadyClientNumber() { return m_Ready.size(); };
        size_t GetClientNumber() { return m_Ready.size() + m_InUse.size(); }

        void StartClient(const std::string& connectCode);

    private:
        std::stack<Client> m_Ready;
        std::stack<Client> m_InUse;
    };

}
