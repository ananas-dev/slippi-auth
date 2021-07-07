#pragma once

// Use standalone asio instead of boost::asio
#define ASIO_STANDALONE

#include "Util/LogBindings.h"
#include "SlippiAuth/Events/ServerEvent.h"
#include "SlippiAuth/Core.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/logger/syslog.hpp>
#include <websocketpp/extensions/permessage_deflate/enabled.hpp>

namespace SlippiAuth {

    class WebSocketServer
    {
    public:


        explicit WebSocketServer(uint16_t port);
        ~WebSocketServer();

        // Custom server config based on bundled asio config
        struct Config : public websocketpp::config::asio {
            typedef websocketpp::log::WebSocketServerLogger<concurrency_type, websocketpp::log::elevel> elog_type;
            typedef websocketpp::log::WebSocketServerLogger<concurrency_type, websocketpp::log::alevel> alog_type;
        };

        typedef websocketpp::server<Config> Server;
        typedef Server::message_ptr MessagePtr;

        void OnOpen(const websocketpp::connection_hdl& hdl);
        void OnMessage(const websocketpp::connection_hdl& hdl, const MessagePtr& msg);
        void OnFail(const websocketpp::connection_hdl& hdl);

        void SendMessage(const Json& message);

        inline void SetEventCallback(const EventCallbackFn& callback)
        {
            m_EventCallback = callback;
        }

        void Start();
    private:
        std::vector<websocketpp::connection_hdl> m_Hdls;
        Server m_Server;
        uint16_t m_Port;

        std::thread m_ServerThread;
        EventCallbackFn m_EventCallback;
    };

}

