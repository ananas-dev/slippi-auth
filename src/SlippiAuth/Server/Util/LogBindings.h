#include <websocketpp/logger/basic.hpp>
#include <websocketpp/common/cpp11.hpp>
#include <websocketpp/logger/levels.hpp>

// Bind websocket logs to spdlog
namespace websocketpp::log {

    template <typename concurrency, typename names>
    class WebSocketServerLogger : public basic<concurrency, names> {
    public:
        typedef basic<concurrency, names> base;

        explicit WebSocketServerLogger<concurrency,names>(channel_type_hint::value hint = channel_type_hint::access)
            : basic<concurrency,names>(hint),
              m_ChannelTypeHint(hint),
              m_Logger(SlippiAuth::Log::GetServerLogger())
        {}

        explicit WebSocketServerLogger<concurrency,names>(level channels,
            channel_type_hint::value hint = channel_type_hint::access)
            : basic<concurrency,names>(channels, hint),
              m_ChannelTypeHint(hint),
              m_Logger(SlippiAuth::Log::GetServerLogger())
        {}

        void write(level channel, std::string const & msg)
        {
            write(channel, msg.c_str());
        }

        void write(level channel, char const * msg)
        {
            ScopedLogType lock(base::m_lock);
            if (!this->dynamic_test(channel)) { return; }

            if (m_ChannelTypeHint == channel_type_hint::access)
            {
                m_Logger->info(msg);
            }
            else
            {
                switch(channel)
                {
                case elevel::devel:
                    m_Logger->debug(msg);
                    break;
                case elevel::library:
                    m_Logger->debug(msg);
                    break;
                case elevel::info:
                    m_Logger->info(msg);
                    break;
                case elevel::warn:
                    m_Logger->warn(msg);
                    break;
                case elevel::rerror:
                    m_Logger->error(msg);
                    break;
                case elevel::fatal:
                    m_Logger->critical(msg);
                    break;
                default:
                    m_Logger->trace(msg);
                }
            }
        }
    private:
        std::shared_ptr<spdlog::logger> m_Logger;
        typedef typename base::scoped_lock_type ScopedLogType;
        channel_type_hint::value m_ChannelTypeHint;
    };

}

