#include "SlippiAuth/Client/ClientConfig.h"
#include "SlippiAuth/Client/ClientPool.h"
#include "SlippiAuth/WebSocketServer/WebSocketServer.h"

#include "Event/ClientEvent.h"
#include "Event/ServerEvent.h"

using namespace SlippiAuth;
using namespace std::chrono_literals;

int main()
{
    // Load config
    SlippiAuth::ClientConfig::Load("clients.json");

    // Init logs
    size_t poolSize = SlippiAuth::ClientConfig::Get().size();
    SlippiAuth::Log::Init(poolSize);

    // Init client pool
    ClientPool pool(poolSize);

    // Start server
    WebSocketServer server;
    server.Start();

    // Start event loop
}
