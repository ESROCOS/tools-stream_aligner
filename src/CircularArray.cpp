#include "CircularArray.hpp"
#include <iostream>

using namespace stream_aligner;

void CircularArray::insert(const base::Time &ts)
{
    if((rear == size-1 && front==0) || front==rear+1)
    {
        std::cout<<"\nCircular queue is full";
        return;
    }
    else if(rear == -1)
    {
        rear++;
        front++;
    }
    else if(rear==size-1)
        rear=0;
    else
        rear++;

    std::cout<<"Enter Data : ";
    data[rear] = ts;
}

void CircularArray::remove()
{
    if(front==-1)
    {
        std::cout<<"\n Circular Queue is empty";
        return;
    }

    std::cout<<data[front]<<" deleted"<<std::endl;

    if(front==rear)
    {
        front=-1;rear=-1;
    }
    else if(front==size-1)
        front=0;
    else
        front++;
}

