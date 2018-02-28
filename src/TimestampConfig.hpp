#ifndef STREAM_ALIGNER_TIMESTAMP_CONFIG_HPP
#define STREAM_ALIGNER_TIMESTAMP_CONFIG_HPP

#include <base/Time.hpp>

namespace stream_aligner
{
    struct TimestampConfig
    {
        base::Time period;
        base::Time latency;
    };
}

#endif
