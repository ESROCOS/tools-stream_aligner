#ifndef STREAM_ALIGNER_TIMESTAMP_ESTIMATOR_HPP
#define STREAM_ALIGNER_TIMESTAMP_ESTIMATOR_HPP

#include "TimestampConfig.hpp"
#include "TimestampStatus.hpp"
#include "CircularArray.hpp"

namespace stream_aligner
{
    class TimestampEstimator
    {

    protected:
        TimestampConfig configuration;
        TimestampStatus status;
        CircularArray<> samples;

    public:
        TimestampEstimator(TimestampConfig &initial_config);

        TimestampStatus getStatus() const;


    };
}

#endif
