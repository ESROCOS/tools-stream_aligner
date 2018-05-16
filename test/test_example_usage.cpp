#include <stream_aligner/TimestampEstimator.hpp>
#include <stream_aligner/StreamAligner.hpp>

#include <iostream>

/** Number of streams or provided interfaces (config value) **/
#define NUMBER_OF_STREAMS 10
#define BUFFER_SIZE_FACTOR  2

/** Stream aligner timeout (config value) **/
#define TIMEOUT 10

/** Stream periods (configuration value) **/
#define S1_PERIOD 2.0
#define S2_PERIOD 0.5
#define S3_PERIOD 1.0

/** Buffer size as a computation of timeout and period scaled witha factor
 * typical two in order to store two cycles of timeout**/
#define BUFFER_SIZE_S1 BUFFER_SIZE_FACTOR * TIMEOUT/S1_PERIOD
#define BUFFER_SIZE_S2 BUFFER_SIZE_FACTOR * TIMEOUT/S2_PERIOD
#define BUFFER_SIZE_S3 BUFFER_SIZE_FACTOR * TIMEOUT/S3_PERIOD

/** When samples have the same time, the priority defines which one to choose
 * at first **/
#define LOW_PRIORITY 1
#define MEDIUM_PRIORITY 2
#define HIGH_PRIORITY 3

std::string last_sample;

void string_callback( const base::Time &time, const std::string& sample )
{
    last_sample = sample;
    std::cout<<"String last_sample["<<time.toString()<<"]: "<<last_sample<<"\n";
}

void double_callback( const base::Time &time, const double& sample )
{
    last_sample = std::to_string(sample);
    std::cout<<"Double last_sample["<<time.toString()<<"]: "<<last_sample<<"\n";
}

void int_callback( const base::Time &time, const int& sample )
{
    last_sample = std::to_string(sample);
    std::cout<<"Integer last_sample["<<time.toString()<<"]: "<<last_sample<<"\n";
}

int main()
{

    /** The aligner has a NUMBER_OF_STREAMS fixed size **/
    stream_aligner::StreamAligner<NUMBER_OF_STREAMS> aligner;
    const size_t N_S1 = static_cast<size_t>(BUFFER_SIZE_S1);
    const size_t N_S2 = static_cast<size_t>(BUFFER_SIZE_S2);
    const size_t N_S3 = static_cast<size_t>(BUFFER_SIZE_S3);

    /** Each stream can have a different size. The size is a factor of the
    * stream aligner timeout and stream period **/
    /** callback, period_time, (optional) priority **/
    int s1 = aligner.registerStream<std::string, N_S1>(&string_callback, base::Time::fromSeconds(S1_PERIOD), LOW_PRIORITY);
    int s2 = aligner.registerStream<double, N_S2>(&double_callback, base::Time::fromSeconds(S2_PERIOD), HIGH_PRIORITY);
    int s3 = aligner.registerStream<int, N_S3>(&int_callback, base::Time::fromSeconds(S3_PERIOD), MEDIUM_PRIORITY);


    /** Push samples in stream 1 **/
    aligner.push<std::string, N_S1>(s1, base::Time::fromSeconds(1.0), std::string("a"));
    aligner.push<std::string, N_S1>(s1, base::Time::fromSeconds(3.0), std::string("b"));
    aligner.push<std::string, N_S1>(s1, base::Time::fromSeconds(5.0), std::string("c"));

    /** Push samples in stream 2 **/
    aligner.push<double, N_S2>(s2, base::Time::fromSeconds(2.0), 0.3386);
    aligner.push<double, N_S2>(s2, base::Time::fromSeconds(2.5), 0.3555);
    aligner.push<double, N_S2>(s2, base::Time::fromSeconds(3.0), 0.3689);
    aligner.push<double, N_S2>(s2, base::Time::fromSeconds(3.5), 0.3756);

    /** Push samples in stream 3 **/
    aligner.push<int, N_S3>(s3, base::Time::fromSeconds(2.0), 21);
    aligner.push<int, N_S3>(s3, base::Time::fromSeconds(3.0), 22);
    aligner.push<int, N_S3>(s3, base::Time::fromSeconds(4.0), 23);

    /** Process the samples **/
    while(aligner.step());
}
