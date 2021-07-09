#include "Server.h"

#include <nlohmann/json.hpp>

namespace SlippiAuth
{
    Server::Server(uint16_t port) : m_Port(port)
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

    Server::~Server()
    {
        if (m_ServerThread.joinable())
            m_ServerThread.join();
    }


    void Server::OnEvent(Event& e)
    {
        CORE_TRACE(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<SearchingEvent>(BIND_EVENT_FN(Server::OnClientSpawn));
        dispatcher.Dispatch<AuthenticatedEvent>(BIND_EVENT_FN(Server::OnAuthenticated));
        dispatcher.Dispatch<SlippiErrorEvent>(BIND_EVENT_FN(Server::OnSlippiError));
    }

    bool Server::OnClientSpawn(SearchingEvent& e)
    {
        Json message = {
                {"type", "searching"},
                {"id", e.GetClientId()},
                {"code", e.GetConnectCode()},
                {"targetCode", e.GetTargetConnectCode()}
        };

        SendMessage(message);
        return true;
    }

    bool Server::OnAuthenticated(AuthenticatedEvent& e)
    {
        Json message = {
                {"type", "authenticated"},
                {"id", e.GetClientId()},
                {"targetCode", e.GetTargetConnectCode()}
        };

        SendMessage(message);
        return true;
    }

    bool Server::OnSlippiError(SlippiErrorEvent& e)
    {
        Json message = {
                {"type", "slippiErr"},
                {"id", e.GetClientId()},
                {"targetCode", e.GetTargetConnectCode()}
        };

        SendMessage(message);
        return true;
    }

    void Server::OnOpen(const websocketpp::connection_hdl& hdl)
    {
        m_Hdls.push_back(hdl);
    }

    void Server::OnMessage(const websocketpp::connection_hdl& hdl, const MessagePtr& msg)
    {
        try
        {
            SERVER_INFO("Websocket message received: {}", msg->get_payload());

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
                    else
                    {
                        OnMissingArg(hdl, "code");
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
        }
        catch (const websocketpp::exception& e)
        {
            SERVER_ERROR("Failed to send message: {}", e.what());
        }
    }

    void Server::OnFail(const websocketpp::connection_hdl& hdl)
    {
        WsServer::connection_ptr con = m_Server.get_con_from_hdl(hdl);
        SERVER_ERROR("{} {}", con->get_ec(), con->get_ec().message());
    }

    void Server::Start()
    {
        SERVER_INFO("Server started on port {}", m_Port);
        m_Server.start_accept();
        m_Server.run();
    }

    void Server::SendMessage(const Json& message)
    {
        try
        {
            for (auto& hdl : m_Hdls)
                m_Server.send(hdl, message.dump(), websocketpp::frame::opcode::text);
        }
        catch (const websocketpp::exception& e)
        {
            SERVER_ERROR("Failed to send message: {}", e.what());
        }
    }

    void Server::SendMessage(const websocketpp::connection_hdl& hdl, const Json& message)
    {
        try
        {
            m_Server.send(hdl, message.dump(), websocketpp::frame::opcode::text);
        }
        catch (const websocketpp::exception& e)
        {
            SERVER_ERROR("Failed to send message: {}", e.what());
        }
    }

    void Server::OnMissingArg(const websocketpp::connection_hdl& hdl, const std::string& argName)
    {
        Json message = {
                {"type", "missingArg"},
                {"what", argName}
        };

        SendMessage(hdl, message);
    }

}
