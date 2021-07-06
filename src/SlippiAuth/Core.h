#pragma once

namespace SlippiAuth {

    using Json = nlohmann::json;
    using EventCallbackFn = std::function<void(Event&)>;
    using namespace std::chrono_literals;

}