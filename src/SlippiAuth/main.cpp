#include "Application.h"

int main()
{
    // Load config
    SlippiAuth::ClientConfig::Load("clients.json");

    // Init logs
    size_t poolSize = SlippiAuth::ClientConfig::Get().size();
    SlippiAuth::Log::Init(poolSize);

    // Start application
    SlippiAuth::Application application;
    application.Run();
}
