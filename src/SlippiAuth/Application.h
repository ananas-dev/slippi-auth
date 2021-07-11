#pragma once

#include "SlippiAuth/Client/ClientConfig.h"
#include "SlippiAuth/Client/ClientPool.h"
#include "SlippiAuth/Server/Server.h"
#include "SlippiAuth/Events/ServerEvent.h"
#include "SlippiAuth/Events/ClientEvent.h"
#include "SlippiAuth/Events/ClientPoolEvent.h"


namespace SlippiAuth {

    class Application
    {
    public:
        Application();
        ~Application();

        void Run();

        void OnEvent(Event& e);
    private:
        ClientPool m_ClientPool;
        Server m_Server;
    };

}