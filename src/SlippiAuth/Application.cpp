#include "ClientConfig.h"
#include "ClientPool.h"

int main()
{
    // Load config
    SlippiAuth::ClientConfig::Load("clients.json");

    size_t poolSize = SlippiAuth::ClientConfig::Get().size();
    SlippiAuth::Log::Init(poolSize);

    SlippiAuth::ClientPool pool(poolSize);
    pool.StartClient("NNAS#975");
    pool.StartClient("FLCD#407");
}
