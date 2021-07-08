#include "WebSocketServer.h"

#include <nlohmann/json.hpp>

namespace SlippiAuth
{

    WebSocketServer::WebSocketServer(uint16_t port) : m_Port(port)
    {
        m_Server.init_asio();

        // Handlers
        m_Server.set_open_handler([this](auto&& hdl)
        {
            return OnOpen(std::forward<decltype(hdl)>(hdl));
        });

        m_Server.set_message_handler([this](auto&& hdl, auto&& msg)
        {
            return OnMessage(std::forward<decltype(hdl)>(hdl), std::forward<decltype(msg)>(msg));
        });
        m_Server.set_fail_handler([this](auto&& hdl)
        {
            return OnFail(std::forward<decltype(hdl)>(hdl));
        });

        m_Server.listen(port);

        // Remove address-in-use exception when restarting
        m_Server.set_reuse_addr(true);

        // Set logger
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

    void WebSocketServer::OnOpen(const websocketpp::connection_hdl& hdl)
    {
        m_Hdls.push_back(hdl);
    }

    void WebSocketServer::OnMessage(const websocketpp::connection_hdl& hdl, const MessagePtr& msg)
    {
        try
        {
            WS_INFO("Websocket message received: {}", msg->get_payload());

            // Deserialize json
            try
            {
                Json message = Json::parse(msg->get_payload());

                if (message["type"] == "queue")
                {
                    if (message.contains("code"))
                    {
                        QueueEvent event(message["code"]);
                        m_EventCallback(event);
                    }
                }
                else if (message["type"] == "stopListening")
                {
                    m_Server.stop_listening();
                }
                else
                {
                    Json response = {{"type", "unknownCommand"}};
                    m_Server.send(hdl, response.dump(), msg->get_opcode());
                }
            }
            catch (const nlohmann::detail::parse_error& e)
            {
                Json response = {{"type", "jsonErr"}};
                m_Server.send(hdl, response.dump(), msg->get_opcode());
            }

            // Send the response

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
        WS_INFO("Server started on port {}", m_Port);
        m_Server.start_accept();
        m_ServerThread = std::thread([this](){ m_Server.run(); });
    }

    void WebSocketServer::SendMessage(const Json& message)
    {
        try
        {
            for (auto& hdl : m_Hdls)
                m_Server.send(hdl, message.dump(), websocketpp::frame::opcode::text);
        }
        catch (const websocketpp::exception& e)
        {
            WS_ERROR("Failed to send message: {}", e.what());
        }
    }

}
