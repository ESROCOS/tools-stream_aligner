#include "CircularArray.hpp"

using namespace stream_aligner;

bool CircularArray::push(const base::Time &ts)
{
    if(this->full())
    {
        return false;
    }
    else if(rear == -1)
    {
        rear++;
        front++;
    }
    else if(rear==CircularArray::size-1)
        rear=0;
    else
        rear++;

    data[rear] = ts;
    return true;
}

base::Time CircularArray::pop()
{
    base::Time ts =  base::Time::fromMicroseconds(0);

    if(this->empty())
    {
        return ts;
    }

    //std::cout<<data[front]<<" deleted"<<std::endl;
    ts = data[front];

    if(front==rear)
    {
        front=-1;rear=-1;
    }
    else if(front==CircularArray::size-1)
        front=0;
    else
        front++;

    return ts;
}

bool CircularArray::empty()
{
    if(front==-1)
    {
        //std::cout<<"\n Circular Queue is empty";
        return true;
    }
    return false;
}

bool CircularArray::full()
{
    if((rear == CircularArray::size-1 && front==0) || front==rear+1)
    {
        //std::cout<<"\nCircular queue is full";
        return true;
    }
    return false;
}

size_t CircularArray::capacity()
{
    return CircularArray::size;
}
