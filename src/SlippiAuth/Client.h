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
        explicit Client(int id);

        void Connect();

    private:
        json m_Config;
        bool m_Connected = false;

        const std::string m_HostServer = "mm.slippi.gg";
        const uint16_t m_HostPort = 43113;

        const std::unordered_map<ProcessState, bool> m_SearchingStates = {
                {ProcessState::Initializing, true},
                {ProcessState::Matchmaking, true},
                {ProcessState::OpponentConnecting, true},
        };

        std::shared_ptr<ENetHost> m_Client;
        std::shared_ptr<ENetPeer> m_Server;

    };

}
