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

    BOOST_CHECK (buffer.empty() == true);
    BOOST_CHECK(buffer.size() == 0);
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i >= buffer.capacity())
        {
            BOOST_CHECK (buffer.full() == true);
            BOOST_CHECK(buffer.size() == buffer.capacity());
        }
        else
        {
            BOOST_CHECK(buffer.size() == i);
        }
        buffer.push_back(time_stamp);

        time_stamp = time_stamp + step;
        std::cout<<"buffer.size():"<<buffer.size()<<"\n";
    }

    BOOST_CHECK (buffer.full() == true);
    BOOST_CHECK(buffer.size() == buffer.capacity());
    BOOST_CHECK (buffer.front()  == time_start + base::Time::fromSeconds(step.toSeconds()));
    BOOST_CHECK (buffer.back()  == time_start + base::Time::fromSeconds(buffer.capacity() * step.toSeconds()));
    BOOST_CHECK(*(buffer.begin()) == buffer.front());

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = buffer.pop_front();
        if (time_stamp.isNull())
        {
            BOOST_CHECK (buffer.empty() == true);
            //std::cout<<"["<< i <<"]Pop buffer is empty" <<std::endl;
        }
        else
        {
            BOOST_CHECK (!time_stamp.isNull() == true);
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
    stream_aligner::CircularArray<base::Time, 5> buffer;

    BOOST_CHECK (buffer.empty() == true);
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i >= buffer.capacity())
        {
            BOOST_CHECK (buffer.full() == true);
            BOOST_CHECK(buffer.size() == buffer.capacity());
        }
        else
        {
            BOOST_CHECK(buffer.size() == i);
        }
        buffer.push_back(time_stamp);

        time_stamp = time_stamp + step;
    }

    BOOST_CHECK (buffer.front()  == time_start + base::Time::fromSeconds(step.toSeconds()));
    BOOST_CHECK (buffer.back()  == time_start + base::Time::fromSeconds(buffer.capacity() * step.toSeconds()));


    BOOST_CHECK (buffer.full() == true);
    buffer.clear();
    BOOST_CHECK (buffer.empty() == true);
    BOOST_CHECK(buffer.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_of_size_1)
{
    std::cout<<"\n*** CIRCULAR ARRAY [TEST 3] ***\n";

    base::Time time_stamp, time_start =  base::Time::now();
    base::Time step = base::Time::fromSeconds(1);
    stream_aligner::CircularArray<base::Time,1> buffer;

    BOOST_CHECK (buffer.empty() == true);
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.capacity()+buffer.capacity(); ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i >= buffer.capacity())
        {
            BOOST_CHECK (buffer.full() == true);
            BOOST_CHECK(buffer.size() == buffer.capacity());
        }
        else
        {
            BOOST_CHECK(buffer.size() == i);
        }
        buffer.push_back(time_stamp);

        time_stamp = time_stamp + step;
    }

    BOOST_CHECK (buffer.full() == true);
    BOOST_CHECK (buffer.front()  == time_start + base::Time::fromSeconds(buffer.capacity() * step.toSeconds()));
    BOOST_CHECK (buffer.back()  == time_start + base::Time::fromSeconds(2*buffer.capacity()-1 * step.toSeconds()));


    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = buffer.pop_front();
        if (time_stamp.isNull())
        {
            BOOST_CHECK (buffer.empty() == true);
        }
        else
        {
            BOOST_CHECK (!time_stamp.isNull() == true);
            BOOST_CHECK (time_stamp  == time_start + base::Time::fromSeconds(i+buffer.capacity() * step.toSeconds()));
        }
    }

    BOOST_CHECK (buffer.empty() == true);
    BOOST_CHECK(buffer.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_type_double)
{

    std::cout<<"\n*** CIRCULAR ARRAY [TEST 4] ***\n";

    base::Time time_stamp, time_start =  base::Time::now();
    base::Time step = base::Time::fromSeconds(1);
    stream_aligner::CircularArray<double> buffer;

    BOOST_CHECK (buffer.empty() == true);
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i >= buffer.capacity())
        {
            BOOST_CHECK (buffer.full() == true);
        }
        else
        {
            BOOST_CHECK(buffer.size() == i);
        }
        buffer.push_back(time_stamp.toSeconds());

        time_stamp = time_stamp + step;
    }

    BOOST_CHECK (buffer.full() == true);
    BOOST_CHECK (base::Time::fromSeconds(buffer.front())  == time_start + base::Time::fromSeconds(step.toSeconds()));
    BOOST_CHECK (base::Time::fromSeconds(buffer.back())  == time_start + base::Time::fromSeconds(buffer.capacity() * step.toSeconds()));

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        time_stamp = base::Time::fromSeconds(buffer.pop_front());
        if (time_stamp.toSeconds() < 0.00)
        {
            BOOST_CHECK (buffer.empty() == true);
        }
        else
        {
            BOOST_CHECK (!time_stamp.isNull() == true);
            BOOST_CHECK (time_stamp  == time_start + base::Time::fromSeconds(i+1 * step.toSeconds()));
        }
    }
    BOOST_CHECK(buffer.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_type_int64t)
{

    std::cout<<"\n*** CIRCULAR ARRAY [TEST 5] ***\n";

    base::Time time_stamp, time_start =  base::Time::now();
    base::Time step = base::Time::fromSeconds(1);
    stream_aligner::CircularArray<int64_t> buffer;

    BOOST_CHECK (buffer.empty());
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i >= buffer.capacity())
        {
            BOOST_CHECK (buffer.full() == true);
        }
        else
        {
            BOOST_CHECK(buffer.size() == i);
        }
        buffer.push_back(time_stamp.toMicroseconds());

        time_stamp = time_stamp + step;
    }

    BOOST_CHECK (buffer.full() == true);
    BOOST_CHECK (base::Time::fromMicroseconds(buffer.front())  == time_start + base::Time::fromSeconds(step.toSeconds()));
    BOOST_CHECK (base::Time::fromMicroseconds(buffer.back())  == time_start + base::Time::fromSeconds(buffer.capacity() * step.toSeconds()));

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        if (!buffer.empty())
        {
            time_stamp = base::Time::fromMicroseconds(buffer.pop_front());
            BOOST_CHECK (time_stamp  == time_start + base::Time::fromSeconds(i+1 * step.toSeconds()));
            //std::cout<<"["<< i <<"]Pop time_stamp "<<time_stamp.toString()<<std::endl;
        }
    }
    BOOST_CHECK (buffer.empty() == true);
    BOOST_CHECK(buffer.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_push_front)
{

    std::cout<<"\n*** CIRCULAR ARRAY [TEST 6] ***\n";

    base::Time time_stamp, time_start =  base::Time::now();
    base::Time step = base::Time::fromSeconds(1);
    stream_aligner::CircularArray<int64_t> buffer;

    BOOST_CHECK (buffer.empty() == true);
    time_stamp = time_start;

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        std::cout<<"["<< i <<"]Push time_stamp "<<time_stamp.toString()<<std::endl;

        if (i >= buffer.capacity())
        {
            BOOST_CHECK (buffer.full() == true);
        }
        else
        {
            BOOST_CHECK(buffer.size() == i);
        }
        buffer.push_front(time_stamp.toMicroseconds());

        time_stamp = time_stamp + step;
    }

    BOOST_CHECK (buffer.full() == true);
    BOOST_CHECK (base::Time::fromMicroseconds(buffer.back())  == time_start + base::Time::fromSeconds(step.toSeconds()));
    BOOST_CHECK (base::Time::fromMicroseconds(buffer.front())  == time_start + base::Time::fromSeconds(buffer.capacity() * step.toSeconds()));

    for (size_t i = 0; i<buffer.capacity()+1; ++i)
    {
        if (!buffer.empty())
        {
            time_stamp = base::Time::fromMicroseconds(buffer.pop_back());
            //BOOST_CHECK (time_stamp  == time_start + base::Time::fromSeconds(i+1 * step.toSeconds()));
            std::cout<<"["<< i <<"]Pop time_stamp "<<time_stamp.toString()<<std::endl;
        }
    }
    BOOST_CHECK (buffer.empty() == true);
    BOOST_CHECK(buffer.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_iterator)
{

    std::cout<<"\n*** CIRCULAR ARRAY [TEST 7] ***\n";

    stream_aligner::CircularArray<size_t> buffer;

    BOOST_CHECK (buffer.empty() == true);
    std::cout<<"address xbegin "<<buffer.xbegin()<<std::endl;
    std::cout<<"address xend "<<buffer.xend()<<std::endl;

    for (size_t i = 0; i<2*buffer.capacity()+1; ++i)
    {
        /** Iterator makes sense when using push_back **/
        buffer.push_back(i);
        /*std::cout<<"["<< i <<"]Push time_stamp "<<i<<std::endl;
        std::cout<<"["<< i <<"]address begin "<<buffer.begin()<<std::endl;
        std::cout<<"["<< i <<"]address end "<<buffer.end()<<std::endl;*/
    }

    BOOST_CHECK(buffer.front() == *buffer.begin());
    BOOST_CHECK (buffer.full() == true);

    /** begin == end if buffer full **/
    if (buffer.full())
    {
        BOOST_CHECK(buffer.end() == buffer.begin());
    }

    for (stream_aligner::cyclic_iterator<size_t> it(buffer); it.itx() != it.end(); ++it)
    {
        std::cout<<"time_stamp "<<*it<<" ["<< it.itx()<< "]"<<std::endl;
    }
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_pop_and_push)
{

    std::cout<<"\n*** CIRCULAR ARRAY [TEST 8] ***\n";

    stream_aligner::CircularArray<int, 3> buffer;

    /**  Insert three elements into the buffer. **/
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);

    BOOST_CHECK(buffer.front() == 1);
    BOOST_CHECK(buffer.back() == 3);
    BOOST_CHECK (buffer.full() == true);

    /** The buffer is full now, so pushing subsequent
    elements will overwrite the front-most elements. **/
    buffer.push_back(4);  // Overwrite 1 with 4.

    /** The buffer now contains 2, 3 and 4. **/
    BOOST_CHECK(buffer.front() == 2);
    BOOST_CHECK(buffer.back() == 4);

    buffer.push_back(5);  // Overwrite 2 with 5.

    /** The buffer now contains 3, 4 and 5. **/
    BOOST_CHECK(buffer.front() == 3);
    BOOST_CHECK(buffer.back() == 5);

    BOOST_CHECK(buffer.front() == *buffer.begin());
    BOOST_CHECK (buffer.full() == true);

    /** Elements can be popped from either the front or the back. **/
    buffer.pop_back();  // 5 is removed.
    buffer.pop_front(); // 3 is removed.

    // Leaving only one element with value = 4.
    BOOST_CHECK(buffer.front() == 4);
    BOOST_CHECK(buffer.back() == 4);
    BOOST_CHECK(buffer.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_array_reverse_iterator)
{

    std::cout<<"\n*** CIRCULAR ARRAY [TEST 9] ***\n";

    stream_aligner::CircularArray<size_t> buffer;

    BOOST_CHECK (buffer.empty() == true);
    std::cout<<"address xbegin "<<buffer.xbegin()<<std::endl;
    std::cout<<"address xend "<<buffer.xend()<<std::endl;

    for (size_t i = 0; i<2*buffer.capacity()+1; ++i)
    {
        /** Reverse iterator makes sense when using push_front **/
        buffer.push_front(i);
        /*std::cout<<"["<< i <<"]Push time_stamp "<<i<<std::endl;
        std::cout<<"["<< i <<"]address rbegin "<<buffer.rbegin()<<std::endl;
        std::cout<<"["<< i <<"]address rend "<<buffer.rend()<<std::endl;*/
    }

    BOOST_CHECK(buffer.front() == *buffer.begin());
    BOOST_CHECK (buffer.full() == true);

    /** begin == end if buffer full **/
    if (buffer.full())
    {
        BOOST_CHECK(buffer.rend() == buffer.rbegin());
    }

    for (stream_aligner::cyclic_reverse_iterator<size_t> it(buffer); it.itx() != it.rend(); ++it)
    {
        std::cout<<"time_stamp "<<*it<<" ["<< it.itx()<< "]"<<std::endl;
    }
}

BOOST_AUTO_TEST_CASE(test_perfect_circular_item_array_iterator)
{

    typedef std::pair<base::Time,std::string> item;
    std::cout<<"\n*** CIRCULAR ARRAY [TEST 10] ***\n";

    stream_aligner::CircularArray<item, 4> buffer;
    base::Time time_stamp, time_start =  base::Time::now();
    base::Time step = base::Time::fromSeconds(1);

    BOOST_CHECK (buffer.empty() == true);
    std::cout<<"address xbegin "<<buffer.xbegin()<<std::endl;
    std::cout<<"address xend "<<buffer.xend()<<std::endl;
    time_stamp = time_start;


    for (size_t i = 0; i<2*buffer.capacity()+1; ++i)
    {
        /** Iterator makes sense when using push_back **/
        buffer.push_back(item(time_stamp, std::to_string(i)));
        time_stamp = time_stamp + step;
    }

    BOOST_CHECK(buffer.front() == *buffer.begin());
    BOOST_CHECK (buffer.full() == true);

    /** begin == end if buffer full **/
    if (buffer.full())
    {
        BOOST_CHECK(buffer.end() == buffer.begin());
    }

    for (stream_aligner::cyclic_iterator<item, 4> it(buffer); it.itx() != it.end(); ++it)
    {
        item element = *it;
        std::cout<<"time["<<element.first.toString()<<"]: "<<element.second<<"\n";
    }

    while(!buffer.empty())
    {
        item my_it = buffer.pop_front();
        std::cout<<"*********************\n";
        std::cout<<"pop element: "<<my_it.second<<"\n";
        std::cout<<"*********************\n";

        for (stream_aligner::cyclic_iterator<item, 4> it(buffer); it.itx() != it.end(); ++it)
        {
            item element = *it;
            std::cout<<"time["<<element.first.toString()<<"]: "<<element.second<<"\n";
        }
    }
}

