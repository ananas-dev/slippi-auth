#pragma once

#include "SlippiAuth/WebSocketServer/Util/LogBindings.h"
#include "SlippiAuth/Client/ClientPool.h"
#include "SlippiAuth/Client/ClientConfig.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/logger/syslog.hpp>
#include <websocketpp/extensions/permessage_deflate/enabled.hpp>

namespace SlippiAuth {

    class WebSocketServer
    {
    public:
        explicit WebSocketServer();
        ~WebSocketServer();

        // Custom server config based on bundled asio config
        struct Config : public websocketpp::config::asio {
            typedef websocketpp::log::WebSocketServerLogger<concurrency_type, websocketpp::log::elevel> elog_type;
            typedef websocketpp::log::WebSocketServerLogger<concurrency_type, websocketpp::log::alevel> alog_type;
        };

        typedef websocketpp::server<Config> Server;
        typedef Server::message_ptr MessagePtr;

        void OnMessage(const websocketpp::connection_hdl& hdl, const MessagePtr& msg);
        void OnFail(const websocketpp::connection_hdl& hdl);

        void Start();
    protected:
        Server m_Server;
        std::thread m_ServerThread;
    };

}

