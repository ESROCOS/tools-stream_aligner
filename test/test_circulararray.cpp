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
    stream_aligner::CircularArray<> buffer;

    BOOST_TEST (buffer.empty());

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = base::Time::now();
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i == buffer.capacity())
        {
            BOOST_TEST (buffer.full());
            BOOST_TEST(!buffer.push(time_stamp));
        }
        else
        {
            BOOST_TEST(buffer.push(time_stamp));
        }

        sleep(1);
    }

    BOOST_TEST (buffer.full());
    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = buffer.pop();
        if (time_stamp.isNull())
        {
            std::cout<<"["<< i <<"]Pop time_stamp is NULL" <<std::endl;
        }
        else
        {
            BOOST_TEST (!time_stamp.isNull());
            std::cout<<"["<< i <<"]Pop time_stamp "<<time_stamp.toString()<<std::endl;
        }
    }

    BOOST_TEST (buffer.empty());
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_of_size_5)
{
    base::Time time_stamp;
    stream_aligner::CircularArray<5> buffer;

    BOOST_TEST (buffer.empty());

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = base::Time::now();
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i == buffer.capacity())
        {
            BOOST_TEST (buffer.full());
            BOOST_TEST(!buffer.push(time_stamp));
        }
        else
        {
            BOOST_TEST(buffer.push(time_stamp));
        }

        sleep(1);
    }

    BOOST_TEST (buffer.full());
    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = buffer.pop();
        if (time_stamp.isNull())
        {
            std::cout<<"["<< i <<"]Pop time_stamp is NULL" <<std::endl;
        }
        else
        {
            BOOST_TEST (!time_stamp.isNull());
            std::cout<<"["<< i <<"]Pop time_stamp "<<time_stamp.toString()<<std::endl;
        }
    }

    BOOST_TEST (buffer.empty());
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_of_size_1)
{
    base::Time time_stamp;
    stream_aligner::CircularArray<1> buffer;

    BOOST_TEST (buffer.empty());

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = base::Time::now();
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i == buffer.capacity())
        {
            BOOST_TEST (buffer.full());
            BOOST_TEST(!buffer.push(time_stamp));
        }
        else
        {
            BOOST_TEST(buffer.push(time_stamp));
        }

        sleep(1);
    }

    BOOST_TEST (buffer.full());
    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = buffer.pop();
        if (time_stamp.isNull())
        {
            std::cout<<"["<< i <<"]Pop time_stamp is NULL" <<std::endl;
        }
        else
        {
            BOOST_TEST (!time_stamp.isNull());
            std::cout<<"["<< i <<"]Pop time_stamp "<<time_stamp.toString()<<std::endl;
        }
    }

    BOOST_TEST (buffer.empty());
}

