#include "TimestampEstimator.hpp"

using namespace stream_aligner;

TimestampEstimator::TimestampEstimator(TimestampConfig &initial_config)
{
    samples.clear();

    this->configuration =initial_config;
}

TimestampStatus TimestampEstimator::getStatus() const
{
    return this->status;
}
