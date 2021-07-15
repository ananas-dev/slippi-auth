#include "SlippiAuth/Client/Client.h"

#include "SlippiAuth/Events/ClientEvent.h"

namespace SlippiAuth {

    Client::Client(uint16_t id) :
        m_Id(id),
        m_Config(ClientConfig::Get()[id]),
        m_State(ProcessState::Idle) {}

    Client::~Client()
    {
        m_State = ProcessState::ErrorEncountered;
        TerminateConnection();
    }

    void Client::Start()
    {
        m_Connected = false;

        CLIENT_INFO(m_Id, "Starting [{}]...", m_Config["connectCode"].get<std::string>());

        m_State = ProcessState::Initializing;
        m_Searching = true;

        uint32_t TimeoutTime = enet_time_get() + m_Timeout * 10;

        while (m_Searching)
        {
            if (TimeoutTime <= enet_time_get())
                m_State = ProcessState::Timeout;

            switch (m_State)
            {
                case ProcessState::Initializing:
                {
                    StartSearching();
                    SearchingEvent clientSpawnEvent(m_DiscordId, m_Config["connectCode"], m_TargetConnectCode);
                    m_EventCallback(clientSpawnEvent);
                    break;
                }
                case ProcessState::Matchmaking:
                {
                    HandleSearching();
                    break;
                }
                case ProcessState::FoundOpponent:
                {
                    TerminateConnection();
                    m_Searching = false;
                    HandleConnecting();
                    break;
                }
                case ProcessState::Timeout:
                {
                    TimeoutEvent timeoutEvent(m_DiscordId, m_TargetConnectCode);
                    m_EventCallback(timeoutEvent);
                    TerminateConnection();
                    m_Searching = false;
                    break;
                }
                case ProcessState::ConnectionSuccess:
                {
                    AuthenticatedEvent authenticatedEvent(m_DiscordId, m_TargetConnectCode);
                    m_EventCallback(authenticatedEvent);
                    break;
                }

                case ProcessState::ErrorEncountered:
                {
                    SlippiErrorEvent slippiErrorEvent(m_DiscordId, m_TargetConnectCode);
                    m_EventCallback(slippiErrorEvent);
                    TerminateConnection();
                    m_Searching = false;
                    break;
                }
                default:
                    m_State = ProcessState::ErrorEncountered;
            }
        }

        m_State = ProcessState::Idle;
        m_Ready = true;
    }

    void Client::SendMessage(const Json& msg)
    {
        enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE;
        uint8_t channelId = 0;

        std::string msgContents = msg.dump();

        ENetPacket* epac = enet_packet_create(msgContents.c_str(), msgContents.length(), flags);
        enet_peer_send(m_Server, channelId, epac);
    }

    int Client::ReceiveMessage(Json &msg, int timeoutMs)
    {
        int hostServiceTimeoutMs = 250;

        // Make sure loop runs at least once
        if (timeoutMs < hostServiceTimeoutMs)
            timeoutMs = hostServiceTimeoutMs;

        // This is not a perfect way to timeout but hopefully it's close enough?
        int maxAttempts = timeoutMs / hostServiceTimeoutMs;

        for (int i = 0; i < maxAttempts; i++)
        {
            ENetEvent netEvent;
            int net = enet_host_service(m_Client, &netEvent, hostServiceTimeoutMs);
            if (net <= 0)
                continue;

            switch (netEvent.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                {

                    std::vector<uint8_t> buf;
                    buf.insert(buf.end(), netEvent.packet->data, netEvent.packet->data + netEvent.packet->dataLength);

                    std::string str(buf.begin(), buf.end());
                    msg = Json::parse(str);

                    enet_packet_destroy(netEvent.packet);
                    return 0;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                    // Return -2 code to indicate we have lost connection to the server
                    return -2;
                default:
                    return -1;
            }
        }

        return -1;
    }

    void Client::DisconnectFromServer()
    {
        m_Connected = false;

        if (m_Server)
            enet_peer_disconnect(m_Server, 0);
        else
            return;

        ENetEvent netEvent;
        while (enet_host_service(m_Client, &netEvent, 3000) > 0)
        {
            switch (netEvent.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(netEvent.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                m_Server = nullptr;
                return;
            default:
                break;
            }
        }

        // didn't disconnect gracefully force disconnect
        enet_peer_reset(m_Server);
        m_Server = nullptr;
    }

    void Client::TerminateConnection()
    {
        // Disconnect from server
        DisconnectFromServer();

        // Destroy client
        if (m_Client)
        {
            enet_host_destroy(m_Client);
            m_Client = nullptr;
        }
    }

    void Client::StartSearching()
    {
        int retryCount = 0;
        while (m_Client == nullptr && retryCount < 15)
        {
            m_HostPort = 41000 + m_Id;
            CLIENT_INFO(m_Id, "Port in use: {}", m_HostPort);

            ENetAddress clientAddr;
            clientAddr.host = ENET_HOST_ANY;
            clientAddr.port = m_HostPort;

            m_Client = enet_host_create(&clientAddr, 1, 3, 0, 0);
            retryCount++;
        }

        if (m_Client == nullptr)
        {
            m_State = ProcessState::ErrorEncountered;
            CLIENT_ERROR(m_Id, "Failed to create client");
            return;
        }

        ENetAddress addr;
        enet_address_set_host(&addr, m_ServerHost.c_str());
        addr.port = m_ServerPort;

        m_Server = enet_host_connect(m_Client, &addr, 3, 0);

        if (m_Server == nullptr)
        {
            m_State = ProcessState::ErrorEncountered;
            CLIENT_ERROR(m_Id, "Failed to start connection to {}:{}", m_ServerHost, m_ServerPort);
            return;
        }

        int connectAttemptCount = 0;
        while(!m_Connected)
        {
            ENetEvent  netEvent;
            int net = enet_host_service(m_Client, &netEvent, 500);
            if (net <= 0 || netEvent.type != ENET_EVENT_TYPE_CONNECT)
            {
                // Not connected yet, will retry
                connectAttemptCount++;
                if (connectAttemptCount >= 20)
                {
                    m_State = ProcessState::ErrorEncountered;
                    CLIENT_ERROR(m_Id, "Failed to connect to {}:{}", m_ServerHost, m_ServerPort);
                    return;
                }
            }

            m_Connected = true;

            CLIENT_INFO(m_Id, "Connected to {}:{}", m_ServerHost, m_ServerPort);
        }

        // Buffering the connect code
        std::vector<uint8_t> connectCodeBuf;
        connectCodeBuf.insert(connectCodeBuf.end(),  m_TargetConnectCode.begin(),
                m_TargetConnectCode.end());

        Json request = {
                {"type", "create-ticket"},
                {"user", {{"uid", m_Config["uid"]}, {"playKey", m_Config["playKey"]}}},
                {"search", {{"mode", 2}, {"connectCode", connectCodeBuf}}},
                {"appVersion", m_AppVersion},
                {"ipAddressLan", "127.0.0.1:" + std::to_string(m_HostPort)},
        };

        SendMessage(request);

        Json response;
        int rcvRes = ReceiveMessage(response, 5000);
        if (rcvRes != 0)
        {
            m_State = ProcessState::ErrorEncountered;
            CLIENT_ERROR(m_Id, "Did not receive response from server for create-ticket");
            return;
        }

        std::string respType = response["type"];
        if (respType != "create-ticket-resp")
        {
            m_State = ProcessState::ErrorEncountered;
            CLIENT_ERROR(m_Id, "Received incorrect response from create-ticket");
            CLIENT_ERROR(m_Id, "{}", response.dump());
        }

        std::string err = response.value("error", "");
        if (err.length() > 0)
        {
            CLIENT_ERROR(m_Id, "Received error from server for create-ticket: {}", err);
        }

        m_State = ProcessState::Matchmaking;
        CLIENT_INFO(m_Id, "Searching [{}]...", m_TargetConnectCode);
    }

    void Client::HandleSearching()
    {
        // Get response from the server
        Json getResp;
        int rcvRes = ReceiveMessage(getResp, 2000);

        if (rcvRes == -1) { return; }
        else if (rcvRes != 0)
        {
            // Only other code is -2 meaning the server dies probably
            CLIENT_ERROR(m_Id, "Lost connection to the mm server");
            m_State = ProcessState::ErrorEncountered;
            return;
        }

        std::string respType = getResp["type"];
        if (respType != "get-ticket-resp")
        {
            CLIENT_ERROR(m_Id, "Received incorrect response from ticket");
            m_State = ProcessState::ErrorEncountered;
            return;
        }

        std::string err = getResp.value("error", "");
        std::string latestVersion = getResp.value("latestVersion", "");
        if (err.length() > 0)
        {
            if (!latestVersion.empty())
            {
                CLIENT_ERROR(m_Id, "Update slippi version to: {}", latestVersion);
            }

            CLIENT_ERROR(m_Id, "Received error from the server for get ticket: {}", err);
            m_State = ProcessState::ErrorEncountered;
            return;
        }

        for (auto& player : getResp["players"])
        {
            if (player["connectCode"] == m_TargetConnectCode)
            {
                m_State = ProcessState::FoundOpponent;

                std::string fullIpAddress = player["ipAddress"];

                // Get ip address
                m_Remote.host = fullIpAddress.substr(0, fullIpAddress.find(':'));

                fullIpAddress.erase(0, fullIpAddress.find(':') + 1);

                // Get port
                m_Remote.port = std::stoi(fullIpAddress.substr(0, fullIpAddress.find(':')));

                CLIENT_INFO(m_Id, "Remote ip is {}:{}", m_Remote.host, m_Remote.port);
                CLIENT_INFO(m_Id, "Successfully authenticated!");

                return;
            }
        }
    }

    void Client::HandleConnecting()
    {
        ENetAddress addr;
        enet_address_set_host(&addr, m_Remote.host.c_str());
        addr.port = m_Remote.port;

        ENetAddress localaddr;
        localaddr.host = ENET_HOST_ANY;
        localaddr.port = m_HostPort;

        m_Client = enet_host_create(&localaddr, 10, 3, 0, 0);
        m_Opponent = enet_host_connect(m_Client, &addr, 3, 0);

        if (m_Client == nullptr)
            CLIENT_ERROR(m_Id, "m_Client is NULL!");

        if (m_Opponent == nullptr)
            CLIENT_ERROR(m_Id, "m_Opponent is NULL!");

        bool connected = false;
        while (!connected)
        {
            ENetEvent netEvent;
            int net = enet_host_service(m_Client, &netEvent, 500);

            if (netEvent.type == ENET_EVENT_TYPE_CONNECT)
            {
                connected = true;
                m_State = ProcessState::ConnectionSuccess;
            }
        }
    }
}

