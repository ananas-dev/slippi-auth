#include "Client.h"

namespace SlippiAuth {

    Client::Client(int id)
    {
        m_Config = ClientConfig::Get()[id];
    }

    void Client::Connect()
    {


    }
}
