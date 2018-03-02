#ifndef STREAM_ALIGNER_CIRCULAR_ARRAY_HPP
#define STREAM_ALIGNER_CIRCULAR_ARRAY_HPP

#include <array>
#include <iostream>
#include <base/Float.hpp>
#include <base/Time.hpp>

namespace stream_aligner
{
    template< class I >
    class cyclic_iterator
    {
        I* it, beg, end;
    public:
        cyclic_iterator( I* f, I* l )
            : it( f ), beg( f ), end( l ) {}
        cyclic_iterator() : it(), beg(), end(){}

        cyclic_iterator &operator++() {
            ++ it;
            if ( it == end ) {
                it = beg;
            }
        } // etc for --, post-operations

        friend bool operator==
            ( cyclic_iterator const &lhs, cyclic_iterator const &rhs )
            { return lhs.it == rhs.it; }

        friend bool operator!=
            ( cyclic_iterator const &lhs, cyclic_iterator const &rhs )
            { return lhs.it != rhs.it; }

        I& operator*(){ return *it; }
    };

    template <class T = base::Time, size_t N = 10>
    class CircularArray
    {
    private:
        static const std::size_t capacity = N;
    protected:
        std::array<T, capacity> data;
        int front_idx, rear_idx;

    public:
       // typedef cyclic_iterator<T> cyclic_iterator;

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
        void push(const T &ts)
        {
            if(this->full())
            {
                this->pop();
            }

            if(rear_idx == -1)
            {
                rear_idx++;
                front_idx++;
            }
            else if(rear_idx==CircularArray::capacity-1)
                rear_idx=0;
            else
                rear_idx++;

            data[rear_idx] = ts;
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
        T pop()
        {
            T ts =  base::NaN<T>(); 

            if(this->empty())
            {
                return ts;
            }

            //std::cout<<data[front_idx]<<" deleted"<<std::endl;
            ts = data[front_idx];

            if(front_idx==rear_idx)
            {
                front_idx=-1;rear_idx=-1;
            }
            else if(front_idx==CircularArray::capacity-1)
                front_idx=0;
            else
                front_idx++;

            return ts;
        };

        /** @brief front
         *
         * It gives the first element without
         * removing it from the array
         *
         *  @param void.
         *  @return the first element
         */
        T front() const
        {
            if(!this->empty())
            {
                return data[front_idx];
            }
            return base::NaN<T>();
        }

        T* begin (){return std::__addressof(data[front_idx]);}

        /** @brief back
         *
         * It gives the last element without
         * removing it from the array
         *
         *  @param void.
         *  @return the last element
         */
        T back() const
        {
            if(!this->empty())
            {
                return data[rear_idx];
            }
            return base::NaN<T>();
        }

        T* end (){return std::__addressof(data[rear_idx]);}

        /** @brief array empty
         *
         *
         *  @param void.
         *  @return true if array is empty. false otherwise.
         */
        bool empty() const
        {
            if(front_idx==-1)
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
        bool full() const
        {
            if((rear_idx == CircularArray::capacity-1 && front_idx==0) || front_idx==rear_idx+1)
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
        size_t size() const
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
        size_t max_size() const
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
            front_idx=-1;rear_idx=-1;
        };

    };
}
#endif

