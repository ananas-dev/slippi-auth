#define ENET_IMPLEMENTATION
#include "Client.h"

namespace SlippiAuth {

    Client::Client(uint32_t id)
    {
        m_Id = id;
        m_Config = ClientConfig::Get()[id];
        m_State = ProcessState::Idle;

        // TODO: move initialization elsewhere
        if (enet_initialize() != 0)
        {
            CLIENT_ERROR(m_Id, "An error occurred while initializing ENet!");
        }
    }

    Client::~Client()
    = default;

    void Client::Start(const std::string& connectCode)
    {
        m_Connected = false;
        m_ConnectCode = connectCode;
        CLIENT_INFO(m_Id, "Starting AUTH for {}", m_ConnectCode);

        m_State = ProcessState::Initializing;

        while (IsSearching())
        {
            switch(m_State)
            {
                case ProcessState::Initializing:
                    Connect();
                    break;

                    /*
                     * IMPLEMENT THE REST
                     */

                default:
                    m_State = ProcessState::ErrorEncountered;
            }
        }
    }

    bool Client::IsSearching()
    {
        return m_SearchingStates.count(m_State) != 0;
    }

    void Client::SendMessage(const json& msg)
    {
        enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE;
        uint8_t channelId = 0;

        std::string msgContents = msg.dump();

        ENetPacket* epac = enet_packet_create(msgContents.c_str(), msgContents.length(), flags);
        enet_peer_send(m_Server, channelId, epac);
    }

    int Client::ReceiveMessage(json &msg, int timeoutMs)
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
                    msg = json::parse(str);

                    enet_packet_destroy(netEvent.packet);
                    return 0;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                    // Return -2 code to indicate we have lost connection to the server
                    return -2;
            }
        }

        return -1;

    }

    void Client::Connect()
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
        enet_address_set_host_new(&addr, m_ServerHost.c_str());
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
        connectCodeBuf.insert(connectCodeBuf.end(),  m_ConnectCode.begin(),
                m_ConnectCode.end());

        json request = {
                {"type", "create-ticket"},
                {"user", {{"uid", m_Config["uid"]}, {"playKey", m_Config["playKey"]}}},
                {"search", {{"mode", 2}, {"connectCode", connectCodeBuf}}},
                {"appVersion", m_AppVersion},
                {"ipAddressLan", "127.0.0.1:" + std::to_string(m_HostPort)},
        };

        SendMessage(request);

        json response;
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
        CLIENT_INFO(m_Id, "The ticket request succeeded");
    }

}

