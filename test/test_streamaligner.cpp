#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "test_streamaligner"
#define BOOST_AUTO_TEST_MAIN

#include <iostream>
#include <numeric>

#include <boost/test/unit_test.hpp>
#include <boost/test/execution_monitor.hpp>

#include <stream_aligner/StreamAligner.hpp>

using namespace stream_aligner;

std::string last_sample;

void test_callback( const base::Time &time, const std::string& sample )
{
    last_sample = sample;
    std::cout<<"last_sample["<<time.toString()<<"]: "<<last_sample<<"\n";
}


BOOST_AUTO_TEST_CASE( order_test )
{
    stream_aligner::StreamAligner reader; 
    reader.setTimeout( base::Time::fromSeconds(2.0) );

    /** callback, period_time, (optional) priority **/
    int s1 = reader.registerStream<std::string, 4>(&test_callback, base::Time::fromSeconds(2)); 
    int s2 = reader.registerStream<std::string, 4>(&test_callback, base::Time::fromSeconds(2), 1);

    /** Stream 1 **/
    reader.push<std::string, 4>(s1, base::Time::fromSeconds(1.0), std::string("a"));
    reader.push<std::string, 4>(s1, base::Time::fromSeconds(3.0), std::string("c"));
    reader.push<std::string, 4>(s1, base::Time::fromSeconds(4.0), std::string("e"));

    /** Stream 2 **/
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(2.0), std::string("b"));
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(3.0), std::string("d"));
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(4.0), std::string("f"));

    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK_EQUAL( last_sample, "a" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK_EQUAL( last_sample, "b" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK_EQUAL( last_sample, "c" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK_EQUAL( last_sample, "d" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK_EQUAL( last_sample, "e" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK_EQUAL( last_sample, "f" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK_EQUAL( last_sample, "" );
}
