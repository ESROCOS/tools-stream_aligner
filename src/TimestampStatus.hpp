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
        /** Estimated period */
        base::Time period;
        /** Estimated latency (always null if no hardware timestamps are
         * provided)
         */
        base::Time latency;

        /** Last input time provided to the input estimator
         */
        base::Time time_raw;
    };
}
#endif

