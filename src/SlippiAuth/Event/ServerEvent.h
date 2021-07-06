#pragma once

#include "Event.h"

#include <utility>

namespace SlippiAuth {

    class QueueEvent : public Event
    {
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

}