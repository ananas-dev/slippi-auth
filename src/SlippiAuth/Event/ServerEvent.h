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

    private:
        std::string m_ConnectCode;
    };

}