#pragma once

#include "Event.h"

namespace SlippiAuth {

    class QueueEvent : public Event
    {
    public:
        explicit QueueEvent(std::string userConnectCode, uint32_t timeout, uint64_t discordId)
            : m_UserConnectCode(std::move(userConnectCode)),
            m_Timeout(timeout),
            m_DiscordId(discordId) {}

        inline const std::string& GetUserConnectCode()
        {
            return m_UserConnectCode;
        }

        [[nodiscard]] inline uint32_t GetTimeout() const
        {
            return m_Timeout;
        }

        [[nodiscard]] inline uint64_t GetDiscordId() const
        {
            return m_DiscordId;
        }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "QueueEvent: (" << m_UserConnectCode << ", " << m_Timeout << ")";
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryServer);
        EVENT_CLASS_TYPE(Queue);
    private:
        std::string m_UserConnectCode;
        uint32_t m_Timeout;
        uint64_t m_DiscordId;
    };

}