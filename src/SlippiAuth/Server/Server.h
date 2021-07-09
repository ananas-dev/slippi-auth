#pragma once

// Use standalone asio instead of boost::asio
#define ASIO_STANDALONE

#include "Util/CustomConfig.h"
#include "SlippiAuth/Events/ServerEvent.h"
#include "SlippiAuth/Events/ClientEvent.h"
#include "SlippiAuth/Core.h"

namespace SlippiAuth {

    class Server
    {
    public:
        explicit Server(uint16_t port);
        ~Server();

        void OnEvent(Event& e);
        inline void SetEventCallback(const EventCallbackFn& callback)
        {
            m_EventCallback = callback;
        }

        void Start();
    private:
        // Events coming from clients
        bool OnClientSpawn(SearchingEvent& e);
        bool OnAuthenticated(AuthenticatedEvent& e);
        bool OnSlippiError(SlippiErrorEvent& e);

        // Core server handlers
        void OnOpen(const websocketpp::connection_hdl& hdl);
        void OnMessage(const websocketpp::connection_hdl& hdl, const MessagePtr& msg);
        void OnFail(const websocketpp::connection_hdl& hdl);

        // Other server handlers
        void OnMissingArg(const websocketpp::connection_hdl& hdl, const std::string& argName);

        void SendMessage(const Json& message);
        void SendMessage(const websocketpp::connection_hdl& hdl, const Json& message);
    private:
        std::vector<websocketpp::connection_hdl> m_Hdls;
        WsServer m_Server;
        uint16_t m_Port;

        std::thread m_ServerThread;
        EventCallbackFn m_EventCallback;
    };

}

