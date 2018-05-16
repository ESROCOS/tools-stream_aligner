#include <stream_aligner/TimestampEstimator.hpp>
#include <stream_aligner/StreamAligner.hpp>

#include <iostream>

static const size_t NUMBER_OF_STREAMS = 10;

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

    /** Each stream can have a different size **/
    const size_t N_S1 = 5;
    const size_t N_S2 = 10;
    const size_t N_S3 = 6;

    /** callback, period_time, (optional) priority **/
    int s1 = aligner.registerStream<std::string, N_S1>(&string_callback, base::Time::fromSeconds(2)); 
    int s2 = aligner.registerStream<double, N_S2>(&double_callback, base::Time::fromSeconds(0.5), 1);
    int s3 = aligner.registerStream<int, N_S3>(&int_callback, base::Time::fromSeconds(1), 1);


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
