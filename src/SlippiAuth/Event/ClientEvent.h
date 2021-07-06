#pragma once

#include "Event.h"

class QueueEvent : public Event
{
public:
    DemoEvent();
    virtual ~DemoEvent();

    static constexpr DescriptorType descriptor = "QueueEvent";

    virtual DescriptorType Type() const
    {
        return descriptor;
    }
};