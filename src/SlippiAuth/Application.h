#pragma once

#include "SlippiAuth/Client/ClientPool.h"
#include "SlippiAuth/Server/Server.h"


namespace SlippiAuth {

    class Application
    {
    public:
        Application();
        ~Application();

        void Run();
    private:
        ClientPool m_ClientPool;
        Server m_Server;
    };

}