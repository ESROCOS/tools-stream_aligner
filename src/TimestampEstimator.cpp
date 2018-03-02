#include "TimestampEstimator.hpp"

using namespace stream_aligner;

TimestampEstimator::TimestampEstimator(TimestampConfig &initial_config)
{
    samples.clear();

    this->configuration = initial_config;
    this->start_time = base::Time();
    this->last_time = 0;
}

base::Time TimestampEstimator::getPeriod() const
{
    if (!this->samples.full() && this->configuration.period.toSeconds())
    {
        return this->configuration.period;
    }
    else
    {
        double latest = samples.back();
        double earliest = samples.front();

        if (base::isUnset(latest))
        {
            return this->configuration.period;
        }
        else if (base::isUnset(earliest))
        {
            return this->configuration.period;
        }
        return base::Time::fromSeconds((latest - earliest) / samples.size()-1);
    }
}

TimestampStatus TimestampEstimator::getStatus() const
{
    return this->status;
}
