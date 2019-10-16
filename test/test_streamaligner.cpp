#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "test_streamaligner"
#define BOOST_AUTO_TEST_MAIN

#include <iostream>
#include <numeric>

#include <boost/bind.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/execution_monitor.hpp>

#include <stream_aligner/StreamAligner.hpp>
#include "PullStreamAligner.hpp"

using namespace stream_aligner;

static const size_t NUMBER_OF_STREAMS = 10;

std::string last_sample;

void test_callback( const base::Time &time, const std::string& sample )
{
    last_sample = sample;
    std::cout<<"last_sample["<<time.toString()<<"]: "<<last_sample<<"\n";
}


BOOST_AUTO_TEST_CASE(order_test)
{
    const size_t N = 4;
    std::cout<<"\n*** STREAM_ALIGNER [TEST 1] ***\n";
    stream_aligner::StreamAligner<NUMBER_OF_STREAMS> aligner; 
    aligner.setTimeout( base::Time::fromSeconds(2.0) );

    /** callback, period_time, (optional) priority **/
    int s1 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2)); 
    int s2 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2), 1);

    /** Stream 1 **/
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(1.0), std::string("a"));
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(3.0), std::string("c"));
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(4.0), std::string("e"));

    /** Stream 2 **/
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(2.0), std::string("b"));
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(3.0), std::string("d"));
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(4.0), std::string("f"));

    last_sample = ""; aligner.step(); BOOST_CHECK( last_sample == "a" );
    last_sample = ""; aligner.step(); BOOST_CHECK( last_sample == "b" );
    last_sample = ""; aligner.step(); BOOST_CHECK( last_sample == "c" );
    last_sample = ""; aligner.step(); BOOST_CHECK( last_sample == "d" );
    last_sample = ""; aligner.step(); BOOST_CHECK( last_sample == "e" );
    last_sample = ""; aligner.step(); BOOST_CHECK( last_sample == "f" );
    last_sample = ""; aligner.step(); BOOST_CHECK( last_sample == "" );
}

/** test case for enabling/disabling streams **/
BOOST_AUTO_TEST_CASE(stream_enable_test)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 2] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner; 
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, period_time, (optional) priority **/
    const size_t N = 4;
    int s1 = aligner.registerStream<std::string, N>( &test_callback, base::Time::fromSeconds(1.0)); 
    int s2 = aligner.registerStream<std::string, N>( &test_callback, base::Time::fromSeconds(0), 1);

    aligner.push<std::string, N>( s1, base::Time::fromSeconds(1.0), std::string("a")); 
    aligner.push<std::string, N>( s2, base::Time::fromSeconds(2.0), std::string("b")); 

    BOOST_CHECK(aligner.isStreamActive(s2) == true);
    aligner.disableStream(s2);
    BOOST_CHECK(aligner.isStreamActive(s2) == false);

    /** we should still be able to read out the disabled stream **/
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "b");

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(3.0), std::string("c"));

    /** since s2 is disabled, we directly get c, without waiting for the timout **/
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "c");

    /** this should reenable s2 **/
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(4.0), std::string("d")); 
    BOOST_CHECK(aligner.isStreamActive(s2) == true);
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(5.0), std::string("e")); 

    /** and we get the s2 sample **/
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "d");

    /** but have to wait for the next s1 sample, since timout is not over **/
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");
}

BOOST_AUTO_TEST_CASE(clear_test)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 3] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner;
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, period_time, (optional) priority **/
    const size_t N = 4;
    int s1 = aligner.registerStream<std::string, 4>(&test_callback, base::Time::fromSeconds(2));
    int s2 = aligner.registerStream<std::string, 4>(&test_callback, base::Time::fromSeconds(2), 1);

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(1.0), std::string("a"));
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(3.0), std::string("c"));
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(2.0), std::string("b"));
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(3.0), std::string("d"));
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(4.0), std::string("f"));
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(4.0), std::string("e"));

    aligner.clear();

    std::cout << aligner.getStatus() << std::endl;

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(1.0), std::string("a")); 
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(3.0), std::string("c")); 
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(2.0), std::string("b")); 
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(3.0), std::string("d")); 
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(4.0), std::string("f")); 
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(4.0), std::string("e")); 

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "b");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "c");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "d");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "e");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "f");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "" );
}

BOOST_AUTO_TEST_CASE(remove_stream)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 4] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner; 
    aligner.setTimeout( base::Time::fromSeconds(2.0) );

    /** callback, period_time, (optional) priority **/
    const size_t N = 4;
    int s1 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2));
    int s3 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2));
    int s2 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2), 1);

    aligner.push<std::string, N>(s3, base::Time::fromSeconds(1.0), std::string("a")); 

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(1.0), std::string("a")); 
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(3.0), std::string("c")); 
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(2.0), std::string("b")); 
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(3.0), std::string("d")); 
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(4.0), std::string("f")); 
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(4.0), std::string("e")); 

    aligner.unregisterStream(s3);
    std::cout << aligner.getStatus() << std::endl;

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "b");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "c");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "d");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "e");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "f");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");

    int s3_new = aligner.registerStream<std::string, 4>( &test_callback, base::Time::fromSeconds(2)); 
    BOOST_CHECK(s3 == s3_new);
}

BOOST_AUTO_TEST_CASE(drop_test)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 5] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner;
    aligner.setTimeout( base::Time::fromSeconds(2.0));

    /** callback, period_time **/
    const size_t N = 5;
    int s1 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2,0));

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(10.0), std::string("a"));
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(11.0), std::string("b"));

    /** The behaviour of the streamaligner has changed here.  Out of order
     samples don't throw anymore, but will be discarded and counted.
     BOOST_REQUIRE_THROW(aligner.push( s1, base::Time::fromSeconds(10.0), std::string("3") ), std::runtime_error); **/

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(10.0), std::string("3"));

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "b");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");
}

BOOST_AUTO_TEST_CASE(copy_state_test)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 6] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner; 
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, period_time **/
    const size_t N = 5;
    int s1 = aligner.registerStream<std::string, N>( &test_callback, base::Time::fromSeconds(2,0)); 

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(10.0), std::string("a"));
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(11.0), std::string("b"));

    StreamAligner<NUMBER_OF_STREAMS> aligner2;
    aligner2.registerStream<std::string, N>( &test_callback, base::Time::fromSeconds(2,0) ); 
    aligner2.copyState(aligner);

    BOOST_CHECK(aligner.getLatency().toSeconds() == aligner2.getLatency().toSeconds());

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "b");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");

    last_sample = ""; aligner2.step(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; aligner2.step(); BOOST_CHECK(last_sample == "b");
    last_sample = ""; aligner2.step(); BOOST_CHECK(last_sample == "");
}

BOOST_AUTO_TEST_CASE(data_on_same_time_zero_lookahead)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 7] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner;
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, period_time **/
    const size_t N = 5;
    int s1 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2,0) );
    int s2 = aligner.registerStream<std::string, N>(&test_callback, base::Time() );

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(2.0), std::string("a"));
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(2.0), std::string("b"));

    last_sample = "";
    aligner.step();
    BOOST_CHECK( last_sample == "a" );
    last_sample = "";
    aligner.step();
    BOOST_CHECK( last_sample == "b" );
}

BOOST_AUTO_TEST_CASE( data_on_same_time_zero_lookahead_advanced )
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 8] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner;
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, buffer_size, period_time **/
    const size_t N = 5;
    int s1 = aligner.registerStream<std::string, N>( &test_callback, base::Time::fromSeconds(2,0));
    int s2 = aligner.registerStream<std::string, N>( &test_callback, base::Time());
    int s3 = aligner.registerStream<std::string, N>( &test_callback, base::Time());
    int s4 = aligner.registerStream<std::string, N>( &test_callback, base::Time());

    aligner.push<std::string, N>(s4, base::Time::fromSeconds(2.0), std::string("d")); 
    aligner.push<std::string, N>(s3, base::Time::fromSeconds(2.0), std::string("c")); 
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(2.0), std::string("a")); 
    aligner.push<std::string, N>(s2, base::Time::fromSeconds(2.0), std::string("b")); 

    last_sample = "";
    aligner.step();
    BOOST_CHECK(last_sample != "");

    last_sample = "";
    aligner.step();
    BOOST_CHECK( last_sample != "" );

    last_sample = "";
    aligner.step();
    BOOST_CHECK( last_sample != "" );

    last_sample = "";
    aligner.step();
    BOOST_CHECK( last_sample != "" );
}


BOOST_AUTO_TEST_CASE(get_status)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 9] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner;
    aligner.setTimeout( base::Time::fromSeconds(2.0) );

    /** callback, period_time **/
    const size_t N = 5;
    aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2,0));
    aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(0,0));

    std::cout << aligner.getStatus();
}

/**
 * This testcase checks if data is replayed, if there is 
 * only data on one stream available
 * */
BOOST_AUTO_TEST_CASE( data_on_one_stream_test )
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 10] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner;
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, period_time **/
    const size_t N = 5;
    aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(2,0)); 
    int s2 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(0,0)); 

    aligner.push<std::string, 5>(s2, base::Time::fromSeconds(1.0), std::string("a"));

    /** instant replay, as perios of s2 is zero **/
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
}

/**
 * This testcase checks, if all samples are replayed,
 * if a newer sample is given to the aligner first.  
 * This test case is about the inital case.
 * */
BOOST_AUTO_TEST_CASE(newer_data_first_init_case)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 11] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner; 
    aligner.setTimeout( base::Time::fromSeconds(2.0) );

    /** callback, period_time **/
    const size_t N = 5;
    int s1 = aligner.registerStream<std::string, N>( &test_callback, base::Time::fromSeconds(2,0));
    int s2 = aligner.registerStream<std::string, N>( &test_callback, base::Time::fromSeconds(0,0));
    aligner.push<std::string, N>( s1, base::Time::fromSeconds(1.1), std::string("b"));

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");

    aligner.push<std::string, N>(s2, base::Time::fromSeconds(1.0), std::string("a"));

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
}

/**
 * This test case check weather the aligner waits
 * the full timeout again after he replayed a sample
 * from a stream
 * */
BOOST_AUTO_TEST_CASE(advanced_timeout)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 12] ***\n";
    StreamAligner<NUMBER_OF_STREAMS> aligner;
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, period_time **/
    const size_t N = 5;
    int s1 = aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(1,0));
    aligner.registerStream<std::string, N>(&test_callback, base::Time::fromSeconds(0,0));
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(1.0), std::string("a"));

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(1.1), std::string("b"));
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");

    aligner.push<std::string, N>(s1, base::Time::fromSeconds(3.1),std::string("c"));
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "b");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "c");

    /** bigger than period, bus smaller than timeout, do not replay **/
    aligner.push<std::string, N>(s1, base::Time::fromSeconds(4.2), std::string("d"));
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");
}


BOOST_AUTO_TEST_CASE(maximum_number_streams)
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 13] ***\n";
    static const size_t NUMBER_OF_STREAMS = 4;
    StreamAligner<NUMBER_OF_STREAMS> aligner;
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    /** callback, buffer_size, period_time **/
    const size_t N = 5;
    int s1 = aligner.registerStream<std::string, N>( &test_callback, base::Time::fromSeconds(2,0)); BOOST_CHECK(s1 != -1);
    int s2 = aligner.registerStream<std::string, N>( &test_callback, base::Time()); BOOST_CHECK(s2 != -1);
    int s3 = aligner.registerStream<std::string, N>( &test_callback, base::Time()); BOOST_CHECK(s3 != -1);
    int s4 = aligner.registerStream<std::string, N>( &test_callback, base::Time()); BOOST_CHECK(s4 != -1);
    int s5 = -1;
    try
    {
        s5 = aligner.registerStream<std::string, N>( &test_callback, base::Time());
    }
    catch(const std::exception& e)
    {
        std::cout<<"FULL SIZE: Exception expected!!\n";
        BOOST_CHECK(s5 == -1);
    }
    aligner.unregisterStream(s3);

    s5 = aligner.registerStream<std::string, N>( &test_callback, base::Time());
    BOOST_CHECK(s5 == s3);
    int s6 = -1;
    try
    {
        s6 = aligner.registerStream<std::string, N>( &test_callback, base::Time());
    }
    catch(const std::exception& e)
    {
        std::cout<<"FULL SIZE: Exception expected!!\n";
        BOOST_CHECK(s6 == -1);
    }
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");
}


template <class T>
struct pull_object
{
    pull_object() : hasNext( false ) {}

    void setNext(base::Time ts, const T& next)
    {
        next_ts = ts;
        next_value = next;
        hasNext = true;
    }

    bool getNext(base::Time& ts, T& next)
    {
        if( hasNext )
        {
            next = next_value;
            ts = next_ts;
            hasNext = false;
            return true;
        }
        return false;
    }

    bool hasNext;
    T next_value;
    base::Time next_ts;
};


BOOST_AUTO_TEST_CASE( pull_stream_test )
{
    std::cout<<"\n*** STREAM_ALIGNER [TEST 14] ***\n";
    PullStreamAligner<2> aligner;
    aligner.setTimeout(base::Time::fromSeconds(2.0));

    pull_object<std::string> p1;
    pull_object<std::string> p2;

    /** callback, period_time, (optional) priority **/
    const size_t N = 4;
    aligner.registerPullStream<std::string, N>(boost::bind( &pull_object<std::string>::getNext, &p1, _1, _2 ), &test_callback, base::Time::fromSeconds(2));
    aligner.registerPullStream<std::string, N>(boost::bind( &pull_object<std::string>::getNext, &p2, _1, _2 ), &test_callback, base::Time::fromSeconds(2), 1);

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "");

    p1.setNext(base::Time::fromSeconds(2.0), std::string("b"));
    p2.setNext(base::Time::fromSeconds(1.0), std::string("a"));
    while(aligner.pull());

    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "a");
    last_sample = ""; aligner.step(); BOOST_CHECK(last_sample == "b");

    PullStreamAligner<2> other;
    other.registerPullStream<std::string, N>(boost::bind( &pull_object<std::string>::getNext, &p1, _1, _2 ), &test_callback, base::Time::fromSeconds(2));
    other.registerPullStream<std::string, N>(boost::bind( &pull_object<std::string>::getNext, &p2, _1, _2 ), &test_callback, base::Time::fromSeconds(2), 1);
    other.copyState(aligner);
}

