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


BOOST_AUTO_TEST_CASE(order_test)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 1] ***\n";
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

    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK( last_sample == "a" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK( last_sample == "b" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK( last_sample == "c" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK( last_sample == "d" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK( last_sample == "e" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK( last_sample == "f" );
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK( last_sample == "" );
}

/** test case for enabling/disabling streams **/
BOOST_AUTO_TEST_CASE(stream_enable_test)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 2] ***\n";
    StreamAligner reader; 
    reader.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, period_time, (optional) priority **/
    int s1 = reader.registerStream<std::string, 4>( &test_callback, base::Time::fromSeconds(1.0)); 
    int s2 = reader.registerStream<std::string, 4>( &test_callback, base::Time::fromSeconds(0), 1);

    reader.push<std::string, 4>( s1, base::Time::fromSeconds(1.0), std::string("a")); 
    reader.push<std::string, 4>( s2, base::Time::fromSeconds(2.0), std::string("b")); 

    BOOST_CHECK(reader.isStreamActive(s2) == true);
    reader.disableStream(s2);
    BOOST_CHECK(reader.isStreamActive(s2) == false);

    /** we should still be able to read out the disabled stream **/
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "b");

    reader.push<std::string, 4>(s1, base::Time::fromSeconds(3.0), std::string("c"));

    /** since s2 is disabled, we directly get c, without waiting for the timout **/
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "c");

    /** this should reenable s2 **/
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(4.0), std::string("d")); 
    BOOST_CHECK(reader.isStreamActive(s2) == true);
    reader.push<std::string, 4>(s1, base::Time::fromSeconds(5.0), std::string("e")); 

    /** and we get the s2 sample **/
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "d");

    /** but have to wait for the next s1 sample, since timout is not over **/
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "");
}

BOOST_AUTO_TEST_CASE(clear_test)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 3] ***\n";
    StreamAligner reader;
    reader.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, period_time, (optional) priority **/
    int s1 = reader.registerStream<std::string, 4>(&test_callback, base::Time::fromSeconds(2));
    int s2 = reader.registerStream<std::string, 4>(&test_callback, base::Time::fromSeconds(2), 1);

    reader.push<std::string, 4>(s1, base::Time::fromSeconds(1.0), std::string("a"));
    reader.push<std::string, 4>(s1, base::Time::fromSeconds(3.0), std::string("c"));
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(2.0), std::string("b"));
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(3.0), std::string("d"));
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(4.0), std::string("f"));
    reader.push<std::string, 4>(s1, base::Time::fromSeconds(4.0), std::string("e"));

    reader.clear();

    std::cout << reader.getStatus() << std::endl;

    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "");

    reader.push<std::string, 4>(s1, base::Time::fromSeconds(1.0), std::string("a")); 
    reader.push<std::string, 4>(s1, base::Time::fromSeconds(3.0), std::string("c")); 
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(2.0), std::string("b")); 
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(3.0), std::string("d")); 
    reader.push<std::string, 4>(s2, base::Time::fromSeconds(4.0), std::string("f")); 
    reader.push<std::string, 4>(s1, base::Time::fromSeconds(4.0), std::string("e")); 

    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "b");
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "c");
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "d");
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "e");
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "f");
    last_sample = ""; reader.step<std::string, 4>(); BOOST_CHECK(last_sample == "" );
}

