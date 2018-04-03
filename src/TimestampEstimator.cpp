#include "TimestampEstimator.hpp"

using namespace stream_aligner;

TimestampEstimator::TimestampEstimator(TimestampConfig &initial_config)
{
    this->reset(initial_config);
}

void TimestampEstimator::reset(TimestampConfig &reset_config)
{
    this->start_time = base::Time();
    this->last_time = 0;
    this->latency = reset_config.latency.toSeconds();
    this->latency_raw = latency;
    this->missing_samples_total = 0;
    this->missing_samples = 0;
    this->last_index = 0;
    this->have_last_index = false;
    this->last_reference = base::Time();
    this->expected_losses = 0;
    this->cumulative_expected_losses = 0;
    this->expected_loss_timeout = 0;
    this->base_time_reset = 0;
    this->base_time_reset_offset = 0;
    this->configuration.period = base::Time::fromSeconds(0.0); /** Period to zero as default **/
    this->configuration = reset_config;
    this->window_time = this->configuration.period.toSeconds() * this->samples.capacity();
    this->samples.clear();
}



base::Time TimestampEstimator::update(base::Time ts)
{
    if (start_time.isNull())
    {
        start_time = ts;
    }

    /** We use doubles internally. Convert to it. **/
    double current = (ts - start_time).toSeconds();

    if (this->samples.empty())
    {
        this->resetBaseTime(current, current);
        this->samples.push_back(current);
        return base::Time::fromSeconds(this->last_time - this->latency) + this->start_time;
    }

    this->samples.push_back(current);

    /**  Recompute the period **/
    double period = this->getPeriod().toSeconds();

    /** update the window time **/
    this->window_time = this->samples.capacity() * period;


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

    if (lost_count > 0)
    {
        this->samples.pop_back();
        for (int i = 0; i < lost_count; ++i)
        {
            this->missing_samples++;
            this->missing_samples_total++;
            this->samples.push_back(base::unset<double>());
            this->last_time += period;
        }
        this->samples.push_back(current);
    }

    /** last_time is tracking the current base time, i.e. the best estimate for the
    next sample is always this->last_time + period

    If this condition is true, it means that the current time stream is
    actually too late (we are receiving a sample that is earlier than this->last_time
    + period). We therefore need to update this->last_time to reflect that fact.

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

void TimestampEstimator::updateReference(base::Time ts)
{
    if (!this->samples.full())
        return;

    double period = this->getPeriod().toSeconds();
    double hw_time   = (ts - this->start_time).toSeconds();

    /* Compute first the fractional part of the latency
    Note that floor returns the integer that is smaller or equal to its
    argument, so it works regardless of this->last_time <=> hw_time **/
    double diff_int = floor((this->last_time - hw_time) / period);
    double diff = this->last_time - (hw_time + diff_int * period);

    /** Get the integral part of the latency from the current m_latency value
     * **/
    double latency_int = floor(this->latency / period);

    this->latency = latency_int * period + diff;
    this->last_reference = ts;
}

int TimestampEstimator::getLostSampleCount() const
{
    return this->missing_samples_total;
}

void TimestampEstimator::shortSamples(double current)
{
    if (this->haveEstimate())
    {
        double min_time = current - this->window_time;
        if (this->samples.back() < min_time)
        {
            this->samples.clear();
            this->missing_samples = 0;
            return;
        }
    }

    if (this->samples.size() == this->missing_samples)
    {
        this->samples.clear();
        this->missing_samples = 0;
    }
}

base::Time TimestampEstimator::getPeriod() const
{
    if (this->samples.empty())
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

        double period = (latest - earliest) / static_cast<double>(samples.size()-1.0);

        /** return the period **/
        return base::Time::fromSeconds(period);
    }
}


bool TimestampEstimator::haveEstimate() const
{
    if (this->configuration.period.toSeconds())
        return (this->samples.size() - this->missing_samples) >= 1;
    else
        return (this->samples.size() - this->missing_samples) >= 2;
}

base::Time TimestampEstimator::getLatency() const
{
    return base::Time::fromSeconds(this->latency);
}


TimestampStatus TimestampEstimator::getStatus() const
{
    /** Current status values **/
    TimestampStatus status;

    status.stamp = base::Time::fromSeconds(this->last_time - this->latency) + this->start_time;
    status.period = this->getPeriod();
    status.latency = this->getLatency();
    status.window_time = base::Time::fromSeconds(this->window_time);
    status.capacity = this->samples.capacity();
    status.lost_samples = this->missing_samples;
    status.lost_samples_total = this->missing_samples_total;
    status.base_time = base::Time::fromSeconds(this->base_time_reset) + this->start_time;
    status.base_time_reset_offset = base::Time::fromSeconds(this->base_time_reset_offset);

    if (this->samples.empty())
    {
        status.time_raw = base::Time();
    }
    else
    {
      status.time_raw = base::Time::fromSeconds(this->samples.back()) + this->start_time;
    }

    status.reference_time_raw = this->last_reference;

    return status;
}

void TimestampEstimator::resetBaseTime(double new_value, double reset_time)
{
    if (this->last_time != 0)
        this->base_time_reset_offset = new_value - this->last_time;
    this->last_time = new_value;
    this->base_time_reset = reset_time;
    if (!this->last_reference.isNull())
        updateReference(this->last_reference);
}

