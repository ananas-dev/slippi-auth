#pragma once

#include "Event.h"

namespace SlippiAuth {

    class QueueEvent : public Event
    {
    public:
        explicit QueueEvent(std::string connectCode)
            : m_ConnectCode(std::move(connectCode)) {};

        inline const std::string& GetConnectCode()
        {
            return m_ConnectCode;
        }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "QueueEvent: " << m_ConnectCode;
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryServer);
        EVENT_CLASS_TYPE(Queue);
    private:
        std::string m_ConnectCode;
    };

    class SetTimeoutEvent : public Event
    {
    public:
        explicit SetTimeoutEvent(uint32_t seconds)
                : m_Seconds(seconds) {};

        [[nodiscard]] inline uint32_t GetSeconds() const
        {
            return m_Seconds;
        }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "SetTimeoutEvent: " << m_Seconds;
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryServer);
        EVENT_CLASS_TYPE(Timeout);
    private:
        uint32_t m_Seconds;
    };

}