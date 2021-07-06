#pragma once

#include <enet/enet.h>

#include "ClientConfig.h"

namespace SlippiAuth {

    enum class ProcessState
    {
        Idle,
        Initializing,
        Matchmaking,
        ConnectionSuccess,
        ErrorEncountered,
    };

    class Client
    {
    public:
        explicit Client(uint32_t id);
        ~Client();

        void SetTargetConnectCode(const std::string& connectCode);

        [[nodiscard]] bool IsReady() const { return m_Ready; }
        [[nodiscard]] uint32_t GetId() const { return m_Id; }
        [[nodiscard]] Json& GetConfig() { return m_Config; }

        void Start();

    private:
        void SendMessage(const Json& msg);
        int ReceiveMessage(Json &msg, int timeoutMs);

        void DisconnectFromServer();
        void TerminateConnection();

        void StartSearching();
        void HandleSearching();
    private:
        bool m_Ready = true;

        uint32_t m_Id;

        // Connect code the client has to connect to
        std::string m_TargetConnectCode;

        ProcessState m_State;

        const std::string m_AppVersion = "2.3.1";

        Json m_Config = {};

        bool m_Connected = false;
        bool m_Searching = false;

        const std::string m_ServerHost = "mm.slippi.gg";
        const uint16_t m_ServerPort = 43113;

        uint16_t m_HostPort = 0;

        ENetHost* m_Client = nullptr;
        ENetPeer* m_Server = nullptr;
    };

}
