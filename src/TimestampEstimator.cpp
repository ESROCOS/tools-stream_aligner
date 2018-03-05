#include "TimestampEstimator.hpp"

using namespace stream_aligner;

TimestampEstimator::TimestampEstimator(TimestampConfig &initial_config)
{
    this->reset(initial_config);
}

void TimestampEstimator::reset(TimestampConfig &reset_config)
{
    samples.clear();

    this->configuration = reset_config;
    this->start_time = base::Time();
    this->last_time = 0;
    this->last_index = 0;
    this->have_last_index = false;
    this->last_reference = base::Time();
    this->expected_losses = 0;
    this->cumulative_expected_losses = 0;
    this->expected_loss_timeout = 0;
}



base::Time TimestampEstimator::update(base::Time ts)
{
    if (start_time.isNull())
    {
        start_time = ts;
    }

    // We use doubles internally. Convert to it.
    double current = (ts - start_time).toSeconds();

    if (this->samples.empty())
    {
        // reset base Time
    }

    this->samples.push(current);

    // Recompute the period
    double period = getPeriod().toSeconds();

    /** Check for lost samples

    This is the distance, in periods, between the last received timestamp and
    the one we just received

    What m_lost_threshold represents is the number of samples that seem to be
    lost that are required before we decide that we indeed did lose a sample.
    This is made so that we are robust to losing a single sample even if
    m_lost_threshold > 10 (for instance)

    It is therefore also the number of calls to update() that are required
    before we can detect a lost sample.
    **/
    if (this->expected_loss_timeout == 0)
    {
        this->cumulative_expected_losses += this->expected_losses;
        this->expected_losses = 0;
    }
    else
        --this->expected_loss_timeout;

    int lost_count = 0;
    if (this->expected_losses > 0)
    {
        /** We calculate a different sample_distance. If we have some suspicion
        that we did lose samples, we take timestamps that are at 1.9 * period
        as distance=2 instead of 1 in the normal case.
        **/
        int sample_distance = (current - this->last_time + period * 0.1) / period;
        if (sample_distance > 1)
        {
            lost_count = std::min(sample_distance - 1, this->expected_losses);
            this->expected_losses -= lost_count;
        }
    }

    /** m_last is tracking the current base time, i.e. the best estimate for the
    next sample is always m_last + period

    If this condition is true, it means that the current time stream is
    actually too late (we are receiving a sample that is earlier than m_last
    + period). We therefore need to update m_last to reflect that fact.

    To avoid resetting the base time unnecessarily, consider that we
    "reset" it as soon as we are within 1e-4 of it.
    **/
    if (this->last_time + period > current - period * 1e-4)
        this->resetBaseTime(current, current);
    else
        this->last_time = this->last_time + period;

    if (!this->last_reference.isNull())
        this->latency_raw = this->last_time - (this->last_reference - this->start_time).toSeconds();
    return base::Time::fromSeconds(this->last_time - this->latency) + this->start_time;
}

base::Time TimestampEstimator::update(base::Time ts, int64_t index)
{
    if (!this->have_last_index || index <= this->last_index)
    {
	    this->have_last_index = true;
        this->last_index = index;
        return update(ts);
    }

    int64_t lost = index - this->last_index - 1;
    this->last_index = index;
    while(lost > 0)
    {
	    lost--;
	    updateLoss();
    }
    return this->update(ts);
}
void TimestampEstimator::updateLoss()
{
    this->expected_losses++;
    this->expected_loss_timeout = 5;
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

void TimestampEstimator::resetBaseTime(double new_value, double reset_time)
{
    if (this->last_time != 0)
        m_base_time_reset_offset = new_value - m_last;
    this->last_time = new_value;
    m_base_time_reset = reset_time;
    if (!this->last_reference.isNull())
        updateReference(this->last_reference);
}


TimestampStatus TimestampEstimator::getStatus() const
{
    return this->status;
}
