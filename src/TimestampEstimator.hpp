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
        /** To avoid loss of precision while manipulating doubles, we move all
        * times to be relative to this time
        *
        * It gets added back when returning from update() and updateReference()
        */
        base::Time start_time;

        /** The last estimated timestamp, without latency
        *
        * The current best estimate for the next sample, with no new
        * information, is always last_time + getPeriod() - latency
        */
        double last_time;


        /** Initial values **/
        TimestampConfig configuration;

        /** Current status values **/
        TimestampStatus status;

        /** The buffer of samples **/
        CircularArray<double> samples;

    public:
        /** Constructor
         * **/
        TimestampEstimator(TimestampConfig &initial_config);

        /** Updates the estimate and return the actual timestamp for +ts+ */
        base::Time update(base::Time ts);

        /** Updates the estimate and return the actual timestamp for +ts+,
        *  calculating lost samples from the index
        */
        base::Time update(base::Time ts, int64_t index);

        /** The currently estimated period
         * @return the period in case the buffer
         * is full or the configuration period otherwise
         * **/
        base::Time getPeriod() const;

        /** Returns the current latency estimate. This is valid only if
         * updateReference() is called
         */
        base::Time getLatency() const;

        /** Returns the maximum jitter duration estimated so far
         *
         * It is reset to zero only on reset
         */
        base::Time getMaxJitter() const;

        /** The current status
         * @return the status
         * **/
        TimestampStatus getStatus() const;




    protected:
        bool push (const double time);

    };
}

#endif
