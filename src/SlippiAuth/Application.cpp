#include "Application.h"

namespace SlippiAuth {

    Application::Application() : m_Server(9002)
    {
        // Bind all the events
        for (auto& Client : m_ClientPool.GetClients())
        {
            Client.SetEventCallback([this](auto&& event)
            {
                m_Server.OnEvent(std::forward<decltype(event)>(event));
            });
        }

        m_Server.SetEventCallback([this](auto&& event)
        {
            m_ClientPool.OnEvent(std::forward<decltype(event)>(event));
        });

        m_ClientPool.SetEventCallback([this](auto&& event)
        {
            m_Server.OnEvent(std::forward<decltype(event)>(event));
        });
    }

    Application::~Application() = default;

    // TODO: Implement the application events
    void Application::OnEvent(Event& e)
    {
        CORE_TRACE(e);
        EventDispatcher dispatcher(e);

        // App events
    }

    void Application::Run()
    {
        m_Server.Start();
    }

}
