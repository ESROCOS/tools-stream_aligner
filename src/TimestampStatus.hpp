#ifndef STREAM_ALIGNER_TIMESTAMP_STATUS_HPP
#define STREAM_ALIGNER_TIMESTAMP_STATUS_HPP

#include <base/Time.hpp>

namespace stream_aligner {
    /** Structure used to report the internal state of a TimestampEstimator
     * instance
     */
    struct TimestampStatus
    {
        /** When this data structure has been generated */
        base::Time stamp;

        /** Last estimated period */
        base::Time period;

        /** Estimated latency (always null if no hardware timestamps are
         * provided)
         */
        base::Time latency;

        /** Window of the Timestamp estimator **/
        size_t window;

        /** Count of samples currently stored in the estimator
         * */
        int lost_samples;

        /** Total count of lost samples
         */
        int lost_samples_total;

        /** Time at which the base time got reset last
         */
        base::Time base_time;
        /** Offset at the last reset of base_time
         */
        base::Time base_time_reset_offset;

        /** Last input time provided to the input estimator
         */
        base::Time time_raw;

        /** Last reference time provided to the input estimator
         */
        base::Time reference_time_raw;
    };

    std::ostream& operator << (std::ostream& stream, TimestampStatus const& status)
    {
        stream << "== Timestamp estimator status\n"
            << "stamp: " << status.stamp.toString() << "\n"
            << "period: " << status.period.toSeconds() << "\n"
            << "latency: " << status.latency.toSeconds() << "\n"
            << "lost_samples: " << status.lost_samples << "\n"
            << "window: " << status.window << std::endl;
        return stream;
    };
}
#endif

