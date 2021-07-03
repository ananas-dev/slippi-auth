#include "ClientConfig.h"

int main()
{
    // Load config
    SlippiAuth::ClientConfig::Load("clients.json");

    size_t poolSize = SlippiAuth::ClientConfig::Get().size();
    SlippiAuth::Log::Init(poolSize);

    std::cout << SlippiAuth::ClientConfig::Get()[0] << std::endl;
}
