#include "Application.h"

using namespace SlippiAuth;

int main()
{
    // Load config
    SlippiAuth::ClientConfig::Load("clients.json");

    // Init logs
    size_t poolSize = SlippiAuth::ClientConfig::Get().size();
    SlippiAuth::Log::Init(poolSize);

    // Start application
    Application application;
    application.Run();
}
