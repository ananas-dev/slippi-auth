#pragma once

#include "Event.h"

namespace SlippiAuth
{

    class NoReadyClientEvent : public Event
    {
    public:
        explicit NoReadyClientEvent(std::string targetConnectCode)
            : m_TargetConnectCode(std::move(targetConnectCode)) {}

        inline const std::string& GetTargetConnectCode()
        {
            return m_TargetConnectCode;
        }

        [[nodiscard]] std::string ToString() const override
        {
            std::stringstream ss;
            ss << "NoReadyClientEvent: " << m_TargetConnectCode;
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryClientPool);
        EVENT_CLASS_TYPE(NoReadyClient);
    private:
        std::string m_TargetConnectCode;
    };

}
