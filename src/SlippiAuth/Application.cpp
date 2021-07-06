#include "SlippiAuth/Client/ClientConfig.h"
#include "SlippiAuth/WebSocketServer/WebSocketServer.h"

#include "Application.h"

#include "Events/ClientEvent.h"
#include "Events/ServerEvent.h"


#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace SlippiAuth {

    Application::Application()
    {
        for (auto& Client : m_ClientPool.GetClients())
        {
            Client.SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
        }

        m_WebSocketServer.SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
    }

    Application::~Application()
    {
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<QueueEvent>(BIND_EVENT_FN(Application::OnQueue));
        dispatcher.Dispatch<ClientSpawnEvent>(BIND_EVENT_FN(Application::OnClientSpawn));

        CORE_TRACE(e);
    }

    [[noreturn]] void Application::Run()
    {
        m_WebSocketServer.Start();
        while(true);
    }

    bool Application::OnQueue(QueueEvent& e)
    {
        m_ClientPool.StartClient(e.GetConnectCode());
        return true;
    }

    bool Application::OnClientSpawn(ClientSpawnEvent& e)
    {
        // TODO: Move this logic in the server class
        Json message = {
            {"type", "queued"},
            {"id", e.GetClientId()},
            {"code", e.GetConnectCode()},
            {"targetCode", e.GetTargetConnectCode()}
        };

        m_WebSocketServer.SendMessage(message);
        return true;
    }

}

