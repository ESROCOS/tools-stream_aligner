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
    if (m_expected_loss_timeout == 0)
    {
        m_rejected_expected_losses += m_expected_losses;
        m_expected_losses = 0;
    }
    else
        --m_expected_loss_timeout;

    int lost_count = 0;
    if (m_expected_losses > 0)
    {
        /** In principle, we should do both the expected_loss and m_lost
        detection at the same time. But it's complicated and I'm lazy, so I'd
        rather KISS it.

        We calculate a different sample_distance. If we have some suspicion
        that we did lose samples, we take timestamps that are at 1.9 * period
        as distance=2 instead of 1 in the normal case.
        **/
        int sample_distance = (current - m_last + period * 0.1) / period;
        if (sample_distance > 1)
        {
            lost_count = std::min(sample_distance - 1, m_expected_losses);
            m_expected_losses -= lost_count;
        }
    }
    else if (m_lost_threshold != INT_MAX)
    {
        int sample_distance = (current - m_last) / period;
        if (sample_distance > 1)
        {
            m_lost.push_back(sample_distance - 1);
            if (static_cast<int>(m_lost.size()) > m_lost_threshold)
                lost_count += *std::min_element(m_lost.begin(), m_lost.end());
        }
    }

    if (lost_count > 0)
    {
        m_samples.pop_back();
        for (int i = 0; i < lost_count; ++i)
        {
            m_missing_samples++;
            m_missing_samples_total++;
            pushSample(base::unset<double>());
            m_last += period;
        }
        pushSample(current);
        m_lost.clear();
    }

    /** m_last is tracking the current base time, i.e. the best estimate for the
    next sample is always m_last + period

    If this condition is true, it means that the current time stream is
    actually too late (we are receiving a sample that is earlier than m_last
    + period). We therefore need to update m_last to reflect that fact.

    To avoid resetting the base time unnecessarily, consider that we
    "reset" it as soon as we are within 1e-4 of it.
    **/
    if (m_last + period > current - period * 1e-4)
        resetBaseTime(current, current);
    else
        m_last = m_last + period;

    if (!m_last_reference.isNull())
        m_latency_raw = m_last - (m_last_reference - m_zero).toSeconds();
    return base::Time::fromSeconds(m_last - m_latency) + m_zero;
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
