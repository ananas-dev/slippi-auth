#pragma once

#include "SlippiAuth/Client/ClientConfig.h"
#include "SlippiAuth/Client/ClientPool.h"
#include "SlippiAuth/WebSocketServer/WebSocketServer.h"
#include "SlippiAuth/Events/ServerEvent.h"
#include "SlippiAuth/Events/ClientEvent.h"


namespace SlippiAuth {

    class Application
    {
    public:
        Application();
        ~Application();

        [[noreturn]] void Run();

        void OnEvent(Event& e);
    private:
        bool OnQueue(QueueEvent& e);
        bool OnClientSpawn(ClientSpawnEvent& e);
        bool OnAuthenticated(AuthenticatedEvent& e);
    private:
        ClientPool m_ClientPool;
        WebSocketServer m_WebSocketServer;
    };

}