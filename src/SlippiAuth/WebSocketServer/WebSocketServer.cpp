#include "WebSocketServer.h"

#include <nlohmann/json.hpp>

namespace SlippiAuth
{
    using Json = nlohmann::json;

    WebSocketServer::WebSocketServer()
    {
        m_Server.init_asio();

        // Handlers
        m_Server.set_message_handler([this](auto&& hdl, auto&& msg)
        {
            return OnMessage(std::forward<decltype(hdl)>(hdl), std::forward<decltype(msg)>(msg));
        });
        m_Server.set_fail_handler([this](auto&& hdl)
        {
            return OnFail(std::forward<decltype(hdl)>(hdl));
        });

        m_Server.listen(9002);

        // Remove websocketpp logger
        m_Server.set_access_channels(websocketpp::log::alevel::all);
        m_Server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_Server.clear_access_channels(websocketpp::log::alevel::frame_header);
        m_Server.clear_access_channels(websocketpp::log::alevel::message_header);
    }

    WebSocketServer::~WebSocketServer()
    {
        if (m_ServerThread.joinable())
            m_ServerThread.join();
    }

    void WebSocketServer::OnMessage(const websocketpp::connection_hdl& hdl, const MessagePtr& msg)
    {
        WS_INFO("Websocket message received: {}", msg->get_payload());
        Json response;

        // Deserialize json
        try
        {
            Json message = Json::parse(msg->get_payload());

            if (message["type"] == "queue")
            {
                if (message.contains("code"))
                {
                    WS_TRACE("Received code: {}", message["code"]);
                    /*
                     * DO WAIT FOR AN EVENT
                     */
                }
            }
            else if (message["type"] == "stop-listening")
            {
                WS_INFO("TEST 1");
                m_Server.stop_listening();
            }
            else
            {
                response["type"] = "unknown-command";
            }
        }
        catch (const nlohmann::detail::parse_error& e)
        {
            response["type"] = "jsonerr";
        }

        // Send the response
        try
        {
            m_Server.send(hdl, response.dump(), msg->get_opcode());
        }
        catch (const websocketpp::exception& e)
        {
            WS_ERROR("Failed to send message: {}", e.what());
        }
    }

    void WebSocketServer::OnFail(const websocketpp::connection_hdl& hdl)
    {
        Server::connection_ptr con = m_Server.get_con_from_hdl(hdl);
        WS_ERROR("{} {}", con->get_ec(), con->get_ec().message());
    }

    void WebSocketServer::Start()
    {
        m_Server.start_accept();
        m_ServerThread = std::thread([this](){ m_Server.run(); });
    }
}
