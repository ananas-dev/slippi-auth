#pragma once

#include <enet/enet.h>

#include "ClientConfig.h"

namespace SlippiAuth {

    enum class ProcessState
    {
        Idle,
        Initializing,
        Matchmaking,
        OpponentConnecting,
        ConnectionSuccess,
        ErrorEncountered,
    };

    class Client
    {
    public:
        explicit Client(uint32_t id);
        ~Client();

        void SetTargetConnectCode(const std::string& connectCode);

        bool IsReady() const { return m_Ready; }
        void Start();

        bool IsSearching();

        void SendMessage(const json& msg);
        int ReceiveMessage(json &msg, int timeoutMs);

        void Connect();
    private:
        bool m_Ready = true;

        uint32_t m_Id;

        // Connect code the client has to connect to
        std::string m_TargetConnectCode;

        ProcessState m_State;

        const std::string m_AppVersion = "2.3.1";

        json m_Config = {};

        bool m_Connected = false;

        const std::string m_ServerHost = "mm.slippi.gg";
        const uint16_t m_ServerPort = 43113;

        uint16_t m_HostPort = 0;

        const std::unordered_map<ProcessState, bool> m_SearchingStates = {
                {ProcessState::Initializing, true},
                {ProcessState::Matchmaking, true},
                {ProcessState::OpponentConnecting, true},
        };

        ENetHost* m_Client = nullptr;
        ENetPeer* m_Server = nullptr;
    };

}
