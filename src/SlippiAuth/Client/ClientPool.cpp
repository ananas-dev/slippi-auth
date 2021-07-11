#include "ClientPool.h"

#include "SlippiAuth/Events/ClientPoolEvent.h"

namespace SlippiAuth {

    ClientPool::ClientPool()
    {
        // Initialize ENet
        if (enet_initialize() != 0)
        {
            CORE_ERROR("An error occurred while initializing ENet!");
        }

        m_Clients.reserve(m_PoolSize);
        m_Threads.reserve(m_PoolSize);
        for (int i = 0; i < m_PoolSize; i++)
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

    void ClientPool::OnEvent(Event& e)
    {
        CORE_TRACE(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<QueueEvent>(BIND_EVENT_FN(ClientPool::OnQueue));
        dispatcher.Dispatch<SetTimeoutEvent>(BIND_EVENT_FN(ClientPool::OnSetTimeout));
    }

    bool ClientPool::OnQueue(QueueEvent& e)
    {
        StartClient(e.GetConnectCode());
        return true;
    }

    bool ClientPool::OnSetTimeout(SetTimeoutEvent& e)
    {
        m_Timeout = e.GetSeconds();
        return true;
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

    void ClientPool::StartClient(const std::string& connectCode)
    {
        uint64_t clientIndex = FindReadyClientIndex();
        if (clientIndex != -1)
        {
            auto& client = m_Clients[clientIndex];

            // Sets the connect code and the timeout
            client.PreStart(connectCode, m_Timeout);

            // Automatically remove the thread when its job is done
            std::lock_guard<std::mutex> lock(m_ThreadMutex);
            m_Threads.emplace_back([&client, this]() {
                    client.Start();
                    std::thread([this] {
                        RemoveThread(std::this_thread::get_id());
                    }).detach();
                }
            );
        }
        else
        {
            NoReadyClientEvent event(connectCode);
            m_EventCallback(event);
        }
    }

    void ClientPool::RemoveThread(std::thread::id id)
    {
        std::lock_guard<std::mutex> lock(m_ThreadMutex);
        auto iter = std::find_if(m_Threads.begin(), m_Threads.end(), [=](std::thread &t)
        {
            return (t.get_id() == id);
        });

        if (iter != m_Threads.end())
        {
            iter->join();
            m_Threads.erase(iter);
        }
    }

}
