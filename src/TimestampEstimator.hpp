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
        static const size_t  BUFFER_SIZE = 100;

    protected:
        typedef CircularArray<double, BUFFER_SIZE> Buffer;

    protected:
        /** To avoid loss of precision while manipulating doubles, we move all
        * times to be relative to this time
        *
        * It gets added back when returning from update() and updateReference()
        */
        base::Time start_time; //m_zero

        /** The last estimated timestamp, without latency
        *
        * The current best estimate for the next sample, with no new
        * information, is always last_time + getPeriod() - latency
        */
        double last_time; //m_last

        /** The estimation window in seconds */
        double window_time;

        /** The latency, i.e. the fixed (or, more accurately, slow drifting)
        * difference between the incoming timestamps and the actual timestamps
        *
        * An apriori value for it can be provided to the constructor, and it
        * will be updated if updateReference is used (i.e. external timestamps
        * are available).
        *
        * It is used as:
        *
        * <code>
        * actual_timestamp = incoming_timestamp - latency
        * </code>
        */
        double latency;

        /** The raw latency, i.e. the unprocessed difference between the
        * estimator's last estimated time and last received reference time
        */
        double latency_raw;

	    /** number of missing samples recorded in samples array */
	    unsigned int missing_samples;

        /** Total number of missing samples */
	    int missing_samples_total;

	    /** the value of the last index given to us using update */
	    int64_t last_index;

	    /** last_index is initialized */
	    bool have_last_index;

        /** The last time given to updateReference */
        base::Time last_reference;

        /** The count of samples that are expected to be lost within
        * expected_loss_timeout calls to update().
        */
        int expected_losses;

        /** The cumulative count of samples that are lost
         */
        int cumulative_expected_losses;

        /** Count of cycles during which expected_losses is expected to be
        * valid
        */
        int expected_loss_timeout;

        /** During the estimation, we keep track of when we encounter an actual
         * sample that matches the current estimated base time.
         *
         * If we don't encounter one in a whole estimation window, we assume
         * that something is wrong and that we should reset it completely
         */
        double base_time_reset;

        /** The offset between the last base time and the new base time at the
         * last call to resetBaseTime 
         *
         * Used for statistics / monitoring purposes only
         */
        double base_time_reset_offset;

        /** Initial values **/
        TimestampConfig configuration;

        /** The buffer of samples **/
        Buffer buffer_samples;

    public:
        /** Constructor
         * **/
        TimestampEstimator(TimestampConfig &initial_config);

        /** Resets this estimator to an initial state, reusing the same
         * parameters
         */
        void reset(TimestampConfig &reset_config);

        /** Updates the estimate and return the actual timestamp for +ts+ */
        base::Time update(base::Time ts);

        /** Updates the estimate and return the actual timestamp for +ts+,
        *  calculating lost samples from the index
        */
        base::Time update(base::Time ts, int64_t index);

        /** Updates the estimate for a known lost sample */
        void updateLoss();

        /** Updates the estimate using a reference */
        void updateReference(base::Time ts);

        /** The total estimated count of lost samples so far */
        int getLostSampleCount() const;


        void shortSamples (double current);

        /** The currently estimated period
         *
         * @return the period in case the buffer
         * is full or the configuration period otherwise
         * **/
        base::Time getPeriod() const;

        /** Returns true if updateLoss and getPeriod can give valid estimates */
        bool haveEstimate() const;

        /** Returns the current latency estimate. This is valid only if
         * updateReference() is called
         */
        base::Time getLatency() const;

        /** The current status
         * @return the status
         * **/
        TimestampStatus getStatus() const;

    protected:
        /** Set the base time to the given value. reset_time is used in update()
         * to trigger new updates when necessary
         */
        void resetBaseTime(double new_value, double reset_time);



    };
}

#endif
