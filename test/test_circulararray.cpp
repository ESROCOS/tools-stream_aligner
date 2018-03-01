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

    std::cout<<"\n*** CIRCULAR ARRAY [TEST 1] ***\n";

    base::Time time_stamp, time_start =  base::Time::now();
    base::Time step = base::Time::fromSeconds(1);
    stream_aligner::CircularArray<> buffer;

    BOOST_TEST (buffer.empty());
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.max_size()+1; ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i >= buffer.max_size())
        {
            BOOST_TEST (buffer.full());
        }
        buffer.push(time_stamp);

        time_stamp = time_stamp + step;
    }

    BOOST_TEST (buffer.full());
    for (size_t i = 0; i<buffer.max_size()+1; ++i)
    {
        time_stamp = buffer.pop();
        if (time_stamp.isNull())
        {
            BOOST_TEST (buffer.empty());
            //std::cout<<"["<< i <<"]Pop buffer is empty" <<std::endl;
        }
        else
        {
            BOOST_TEST (!time_stamp.isNull());
            BOOST_CHECK (time_stamp  == time_start + base::Time::fromSeconds(i+1 * step.toSeconds()));
            //std::cout<<"["<< i <<"]Pop time_stamp "<<time_stamp.toString()<<std::endl;
        }
    }

}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_of_size_5)
{
    std::cout<<"\n*** CIRCULAR ARRAY [TEST 2] ***\n";

    base::Time time_stamp, time_start =  base::Time::now();
    base::Time step = base::Time::fromSeconds(1);
    stream_aligner::CircularArray<5> buffer;

    BOOST_TEST (buffer.empty());
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.max_size()+1; ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i == buffer.max_size())
        {
            BOOST_TEST (buffer.full());
        }
        buffer.push(time_stamp);

        time_stamp = time_stamp + step;
    }

    BOOST_TEST (buffer.full());
    buffer.clear();
    BOOST_TEST (buffer.empty());
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_of_size_1)
{
    std::cout<<"\n*** CIRCULAR ARRAY [TEST 3] ***\n";

    base::Time time_stamp, time_start =  base::Time::now();
    base::Time step = base::Time::fromSeconds(1);
    stream_aligner::CircularArray<1> buffer;

    BOOST_TEST (buffer.empty());
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.max_size()+10; ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i == buffer.max_size())
        {
            BOOST_TEST (buffer.full());
        }
        buffer.push(time_stamp);

        time_stamp = time_stamp + step;
    }

    BOOST_TEST (buffer.full());
    for (size_t i = 0; i<buffer.max_size()+1; ++i)
    {
        time_stamp = buffer.pop();
        if (time_stamp.isNull())
        {
            BOOST_TEST (buffer.empty());
        }
        else
        {
            BOOST_TEST (!time_stamp.isNull());
            BOOST_CHECK (time_stamp  == time_start + base::Time::fromSeconds(i+10 * step.toSeconds()));
        }
    }

    BOOST_TEST (buffer.empty());
}

