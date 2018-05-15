#ifndef STREAM_ALIGNER_STREAM_ALIGNER_STATUS_HPP
#define STREAM_ALIGNER_STREAM_ALIGNER_STATUS_HPP

#include <base/Time.hpp>
#include <array>

namespace stream_aligner
{
    /** Debugging structure used to report about the status of a single stream in a stream aligner
     */
    class StreamStatus
    {
    public:
        /** The actual size of the buffer */
        size_t buffer_size;
        /** How many samples are currently waiting inside the stream buffer */
        size_t buffer_fill;
        /** The total number of samples ever received for that stream
         * 
         * The following relationship should hold:
         *   
         *   samples_received == samples_processed +
         * 	samples_dropped_buffer_full +
         * 	samples_dropped_late_arriving
         */
        size_t samples_received;
        /** The total count of samples ever processed by the callbacks of this stream
         * 
         * The total number of samples ever received is
         *   
         *   samples_processed + samples_dropped_buffer_full + samples_dropped_late_arriving
         */
        size_t samples_processed;
        /** Count of samples dropped because the buffer was full
         * 
         * Should be zero on streams that have dynamically resized buffers
         */
        size_t samples_dropped_buffer_full;
        /** Count of samples dropped because their timestamp was earlier than
         * the stream aligner current time
         */
        size_t samples_dropped_late_arriving;
        /** Count of samples dropped because their timestamp was not properly ordered
         * 
         * I.e. samples for which the timestamp was later than the previous
         * sample received for that stream
         */
        size_t samples_backward_in_time;
        /** Time of the newest sample currently stored in the stream buffer.
         * Null time if the stream is empty
         */
        base::Time latest_data_time;
        /** Time of the oldest sample currently stored in the stream buffer.
         * Null time if the stream is empty
         */
        base::Time earliest_data_time;	
        /** Time of the last sample received for this stream, regardless of
         * whether it has been dropped or pushed to the stream
         */
        base::Time latest_sample_time;
        /** True if the stream is being used by the stream aligner */
        bool active;
        /** The stream name. In the case of the oroGen plugin, this is set to
         * the port name
         */
        std::string name;
        /** The priority at which this stream is processed. When samples of the
         * same timestamp are available on two different streams, the stream
         * with the lower priority value is processed first.
         */
        int64_t priority;

    public:
        StreamStatus() : buffer_size(0), buffer_fill(0), samples_received(0), 
                samples_processed(0), samples_dropped_buffer_full(0), 
                samples_dropped_late_arriving(0), 
                samples_backward_in_time(0), active(true), priority(0)
        {
        }
    };

    /** Structure used to report the complete state of a stream aligner
     * 
     * The stream aligner latency is time - current_time
     */
    template<size_t NUMBER_STREAMS>
    class StreamAlignerStatus
    {
    public:
        typedef std::array<StreamStatus, NUMBER_STREAMS> StatusVector;

    public:

        /** Time at which this data structure got generated
         */
        base::Time time;
        /** The name of the stream aligner. In the case of the oroGen plugin,
         * this is the name of the underlying oroGen task
         */
        std::string name;
        /** The stream aligner's time
         * 
         * This is the time of the last sample that has been given to a stream
         * aligner callback
         */
        base::Time current_time;
        /** Time of the last sample that got in the stream aligner
         */
        base::Time latest_time;
        /** Count of samples that got dropped because, at the time they arrived,
         * they were older than the stream aligner's current time.
         * 
         * This happens if: the stream aligner timed out or if a sample arrived
         * earlier than the stream's declared period (i.e. the period is too big).
         */
        size_t samples_dropped_late_arriving;
        /** Status of each individual streams
         */
        StatusVector streams;

    public:
        StreamAlignerStatus() : samples_dropped_late_arriving(0)
        {
        }	

    };
}

template <size_t NUMBER_STREAMS> inline std::ostream &operator<<(std::ostream &os, const stream_aligner::StreamAlignerStatus<NUMBER_STREAMS> &status)
{
    using ::operator <<;
    os
	<< "time agg: \t"
	<< "current time: \t" 
	<< " latest time: \t" 
	<< " dropped late samples: \t" << status.samples_dropped_late_arriving 
	<< " latency: \t" 
	<< std::endl
	<<  status.time
	<< "\t" << status.current_time 
	<< "\t" << status.latest_time 
	<< "\t" << status.samples_dropped_late_arriving 
	<< "\t" << status.latest_time - status.current_time 
	<< std::endl;
	
    if( status.streams.empty() )
    	return os; 

    os << "idx\tname\t\tbsize\tbfill\treceived\tprocessed\tdr_bfull\tdr_late\tbackward time" << std::endl;

    int cnt = 0;
    for(typename stream_aligner::StreamAlignerStatus<NUMBER_STREAMS>::StatusVector::const_iterator it = status.streams.begin(); it != status.streams.end(); it++)
    {
        if(it->active)
        {
            os << cnt << "\t"
            << it->name << "\t\t"
        	<< it->buffer_size << "\t"
        	<< it->buffer_fill << "\t"
        	<< it->samples_received << "\t"
        	<< it->samples_processed << "\t"
        	<< it->samples_dropped_buffer_full << "\t"
        	<< it->samples_dropped_late_arriving << "\t"
        	<< it->samples_backward_in_time << "\t"
        	<< std::endl;
        }
	cnt++;
    }

    os << "idx\tname\t\tlatest sample\tearliers data\tlatest data\tlatency" << std::endl;

    for(typename stream_aligner::StreamAlignerStatus<NUMBER_STREAMS>::StatusVector::const_iterator it = status.streams.begin(); it != status.streams.end(); it++)
    {
        if(it->active)
            {
                os << cnt << "\t"
                << it->latest_sample_time << "\t"
                << it->earliest_data_time << " \t "
                << it->latest_data_time << " \t "
                << it->latest_sample_time - status.current_time
                << std::endl;
            }
        cnt++;
    }
    return os;
};

std::ostream &operator<<(std::ostream &os, const stream_aligner::StreamStatus &status);



#endif
