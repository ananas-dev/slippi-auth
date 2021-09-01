#pragma once

#include "Event.h"

namespace SlippiAuth {

    class SearchingEvent : public Event
    {
    public:
        explicit SearchingEvent(uint64_t discordId, std::string botConnectCode, std::string userConnectCode)
            : m_DiscordId(discordId),
            m_UserConnectCode(std::move(userConnectCode)),
            m_BotConnectCode(std::move(botConnectCode)) {};

        [[nodiscard]] inline uint64_t GetDiscordId() const
        {
            return m_DiscordId;
        }

        inline const std::string& GetUserConnectCode()
        {
            return m_UserConnectCode;
        }

        inline const std::string& GetBotConnectCode()
        {
            return m_BotConnectCode;
        }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "SearchingEvent: " << "(" << m_DiscordId << ", " << m_BotConnectCode
               << ", " << m_UserConnectCode << ")";
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryClient);
        EVENT_CLASS_TYPE(Searching);
    private:
        uint64_t m_DiscordId;
        std::string m_BotConnectCode;
        std::string m_UserConnectCode;
    };

    class AuthenticatedEvent : public Event
    {
    public:
        explicit AuthenticatedEvent(uint64_t discordId, std::string userConnectCode, std::string userName, std::string userIp)
            : m_DiscordId(discordId),
            m_UserConnectCode(std::move(userConnectCode)),
            m_UserName(std::move(userName)),
            m_UserIp(std::move(userIp)) {};

        [[nodiscard]] inline uint64_t GetDiscordId() const
        {
            return m_DiscordId;
        }

        inline const std::string& GetUserConnectCode()
        {
            return m_UserConnectCode;
        }

        inline const std::string& GetUserName()
        {
            return m_UserName;
        }

        inline const std::string& GetUserIp()
        {
            return m_UserIp;
        }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "AuthenticatedEvent: " << "(" << m_DiscordId
               << ", " << m_UserConnectCode << ", " << m_UserName
               << ", " << m_UserIp << ")";
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryClient);
        EVENT_CLASS_TYPE(Authenticated);
    private:
        uint64_t m_DiscordId;
        std::string m_UserConnectCode;
        std::string m_UserName;
        std::string m_UserIp;
    };

    class SlippiErrorEvent : public Event
    {
    public:
        explicit SlippiErrorEvent(uint64_t discordId, std::string userConnectCode)
            : m_DiscordId(discordId),
              m_UserConnectCode(std::move(userConnectCode)) {};

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
            ss << "SlippiErrorEvent: " << "(" << m_DiscordId
               << ", " << m_UserConnectCode << ")";
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryClient);
        EVENT_CLASS_TYPE(SlippiError);
    private:
        uint64_t m_DiscordId;
        std::string m_UserConnectCode;
    };

    class TimeoutEvent : public Event
    {
    public:
        explicit TimeoutEvent(uint64_t discordId, std::string userConnectCode)
                : m_DiscordId(discordId),
                  m_UserConnectCode(std::move(userConnectCode)) {};

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
            ss << "TimeoutEvent: " << "(" << m_DiscordId
               << ", " << m_UserConnectCode << ")";
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryClient);
        EVENT_CLASS_TYPE(Timeout);
    private:
        uint64_t m_DiscordId;
        std::string m_UserConnectCode;
    };

}
