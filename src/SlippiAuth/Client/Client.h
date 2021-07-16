#pragma once

#include "ClientConfig.h"
#include "SlippiAuth/Core.h"

#include <enet/enet.h>

namespace SlippiAuth
{

    enum class ProcessState
    {
        Idle,
        Initializing,
        Matchmaking,
        ConnectionSuccess,
        ErrorEncountered,
        Timeout,
    };

    class Client
    {
    public:
        explicit Client(uint16_t id);

        ~Client();

        [[nodiscard]] bool IsReady() const
        {
            return m_Ready;
        }

        [[nodiscard]] uint16_t GetId() const
        {
            return m_Id;
        }

        [[nodiscard]] Json& GetConfig()
        {
            return m_Config;
        }

        void PreStart(const std::string& connectCode, uint32_t timeout, uint64_t discordId)
        {
            m_Timeout = timeout;
            m_TargetConnectCode = connectCode;
            m_DiscordId = discordId;
            m_Ready = false;
        }

        void Start();

        inline void SetEventCallback(const EventCallbackFn& callback)
        {
            m_EventCallback = callback;
        }

    private:
        void SendMessage(const Json& msg);
        int ReceiveMessage(Json& msg, int timeoutMs);

        void Disconnect();
        void DisconnectFromServer();
        void DisconnectFromOpponent();

        void StartSearching();
        void HandleSearching();
        void HandleConnecting();

    private:
        bool m_Ready = true;

        uint16_t m_Id;

        // Connect code the client has to connect to
        std::string m_TargetConnectCode;

        ProcessState m_State;

        const std::string m_SlippiApiBaseUrl = "https://users-rest-dot-slippi.uc.r.appspot.com/user";
        std::string m_SlippiLatestVersion{};

        Json m_Config{};

        bool m_Searching = false;

        const std::string m_ServerHost = "mm.slippi.gg";
        const uint16_t m_ServerPort = 43113;

        uint16_t m_HostPort{};

        struct Remote
        {
            std::string host;
            uint16_t port;
        } m_Remote{};

        ENetHost* m_Client = nullptr;
        ENetPeer* m_Server = nullptr;
        ENetPeer* m_Opponent = nullptr;

        EventCallbackFn m_EventCallback;

        // Timeout in seconds
        uint32_t m_Timeout{};

        // For codeman purposes
        uint64_t m_DiscordId{};
    };

}
