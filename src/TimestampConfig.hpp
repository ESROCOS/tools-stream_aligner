#ifndef STREAM_ALIGNER_TIMESTAMP_CONFIG_HPP
#define STREAM_ALIGNER_TIMESTAMP_CONFIG_HPP

#include <base/Time.hpp>

namespace stream_aligner
{
    struct TimestampConfig
    {
        base::Time period;
        base::Time latency;
        TimestampConfig(){};
        TimestampConfig(base::Time _period, base::Time _latency):
            period(_period), latency(_latency){};
    };
}

#endif
