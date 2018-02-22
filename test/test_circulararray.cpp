#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "test_circulararray"
#define BOOST_AUTO_TEST_MAIN

#include <iostream>
#include <numeric>
#include <unistd.h>

#include <boost/test/unit_test.hpp>
#include <boost/test/execution_monitor.hpp>

#include <stream_aligner/CircularArray.hpp>

BOOST_AUTO_TEST_CASE(test_perfect_circular_array)
{
    base::Time time_stamp;
    stream_aligner::CircularArray buffer;

    for (int i = 0; i<10; ++i)
    {
        time_stamp = base::Time::now();
        std::cout<<"Time stamp "<<time_stamp.toString()<<std::endl;
        buffer.insert(time_stamp);
        sleep(1);
    }

}

