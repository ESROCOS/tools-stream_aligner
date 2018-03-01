#ifndef STREAM_ALIGNER_CIRCULAR_ARRAY_HPP
#define STREAM_ALIGNER_CIRCULAR_ARRAY_HPP

#include <array>
#include <base/Time.hpp>

namespace stream_aligner
{
    template <size_t N = 10>
    class CircularArray
    {
    private:
        static const std::size_t capacity = N;
    protected:
        std::array<base::Time, capacity> data;
        int front,rear;
    public:
        /** @brief Constructor
         *
         *  @return void.
         */
        CircularArray()
        {
            this->clear();
        }

        /** @brief insert an element
         *
         *  This methods insert a new element
         *  in the CircularArray
         *
         *  @param ts the element.
         *  @return void.
         */
        void push(const base::Time &ts)
        {
            if(this->full())
            {
                this->pop();
            }

            if(rear == -1)
            {
                rear++;
                front++;
            }
            else if(rear==CircularArray::capacity-1)
                rear=0;
            else
                rear++;

            data[rear] = ts;
            return;

        };


        /** @brief remove an element
         *
         *  This methods remove an element
         *  from the CircularArray
         *
         *  @param void.
         *  @return void.
         */
        base::Time pop()
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
            else if(front==CircularArray::capacity-1)
                front=0;
            else
                front++;

            return ts;
        };

        /** @brief array empty
         *
         *
         *  @param void.
         *  @return true if array is empty. false otherwise.
         */
        bool empty()
        {
            if(front==-1)
            {
                //std::cout<<"\n Circular Queue is empty";
                return true;
            }
            return false;
        };

        /** @brief array full
         *
         *
         *  @param void.
         *  @return true if array is full. false otherwise.
         */
        bool full()
        {
            if((rear == CircularArray::capacity-1 && front==0) || front==rear+1)
            {
                //std::cout<<"\nCircular queue is full";
                return true;
            }
            return false;
        };

        /** @brief size
         *
         *
         *  @param void.
         *  @return number of elements in the array
         */
        size_t size()
        {
            return data.size();
        };

        /** @brief max_size
         *
         *  maximum number of elements the array is able to hold
         *
         *  @param void.
         *  @return capacity
         */
        size_t max_size()
        {
            return CircularArray::capacity;
        };

        /** @brief clear
         *
         *
         *  @param void.
         *  @return void.
         */
        void clear()
        {
            front=-1;rear=-1;
        };

    };
}
#endif

