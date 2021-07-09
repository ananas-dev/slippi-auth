#include "LogBindings.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/extensions/permessage_deflate/enabled.hpp>

namespace SlippiAuth {

    // Custom server config based on bundled asio config
    struct Config : public websocketpp::config::asio {
        typedef websocketpp::log::WebSocketServerLogger<concurrency_type, websocketpp::log::elevel> elog_type;
        typedef websocketpp::log::WebSocketServerLogger<concurrency_type, websocketpp::log::alevel> alog_type;
    };

    typedef websocketpp::server<Config> WsServer;
    typedef WsServer::message_ptr MessagePtr;

}
