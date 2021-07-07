#include "SlippiAuth/Client/ClientConfig.h"
#include "SlippiAuth/WebSocketServer/WebSocketServer.h"

#include "Application.h"

#include "Events/ClientEvent.h"
#include "Events/ServerEvent.h"


#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace SlippiAuth {

    Application::Application() : m_WebSocketServer(9002)
    {
        for (auto& Client : m_ClientPool.GetClients())
        {
            Client.SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
        }

        m_WebSocketServer.SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
    }

    Application::~Application()
    = default;

    void Application::OnEvent(Event& e)
    {
        CORE_TRACE(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<QueueEvent>(BIND_EVENT_FN(Application::OnQueue));
        dispatcher.Dispatch<SearchingEvent>(BIND_EVENT_FN(Application::OnClientSpawn));
        dispatcher.Dispatch<AuthenticatedEvent>(BIND_EVENT_FN(Application::OnAuthenticated));
        dispatcher.Dispatch<SlippiErrorEvent>(BIND_EVENT_FN(Application::OnSlippiError));
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

    bool Application::OnClientSpawn(SearchingEvent& e)
    {
        Json message = {
            {"type", "searching"},
            {"id", e.GetClientId()},
            {"code", e.GetConnectCode()},
            {"targetCode", e.GetTargetConnectCode()}
        };

        m_WebSocketServer.SendMessage(message);
        return true;
    }

    bool Application::OnAuthenticated(AuthenticatedEvent& e)
    {
        Json message = {
            {"type", "authenticated"},
            {"id", e.GetClientId()},
            {"targetCode", e.GetTargetConnectCode()}
        };

        m_WebSocketServer.SendMessage(message);
        return true;
    }

    bool Application::OnSlippiError(SlippiErrorEvent& e)
    {
        Json message = {
            {"type", "slippiErr"},
            {"id", e.GetClientId()},
            {"targetCode", e.GetTargetConnectCode()}
        };

        m_WebSocketServer.SendMessage(message);
        return true;
    }

}
