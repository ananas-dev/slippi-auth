#pragma once

#include "Event.h"

namespace SlippiAuth
{

    class NoReadyClientEvent : public Event
    {
    public:
        explicit NoReadyClientEvent(uint64_t discordId, std::string userConnectCode)
            : m_DiscordId(discordId),
            m_UserConnectCode(std::move(userConnectCode)) {}

        [[nodiscard]] inline uint64_t GetDiscordId() const
        {
            return m_DiscordId;
        }

        inline const std::string& GetUserConnectCode()
        {
            return m_UserConnectCode;
        }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "NoReadyClientEvent: (" << m_DiscordId << ", " << m_UserConnectCode << ")";
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryClientPool);
        EVENT_CLASS_TYPE(NoReadyClient);
    private:
        uint64_t m_DiscordId;
        std::string m_UserConnectCode;
    };

}
