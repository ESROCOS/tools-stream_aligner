#ifndef STREAM_ALIGNER_CIRCULAR_ARRAY_HPP
#define STREAM_ALIGNER_CIRCULAR_ARRAY_HPP

#include <array>
#include <iostream>
#include <base/Float.hpp>
#include <base/Time.hpp>

namespace stream_aligner
{
    template <class T = base::Time, size_t N = 10>
    class CircularArray
    {
    private:
        static const std::size_t max_size = N;
    protected:
        std::array<T, max_size> data;
        int front_idx, rear_idx;

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
            else if(rear_idx==CircularArray::max_size-1)
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
            else if(front_idx==CircularArray::max_size-1)
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

        /** begin
         *
         * Pointer to the first element (front_idx)
         *
         * @return pointer to the last element
         */
        T* begin (){return std::__addressof(data[front_idx]);}

        /* xbegin
         *
         * Pointer to the first block in memory
         *
         * @return pointer to data[0]
         */
        T* xbegin (){return std::__addressof(data[0]);}

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

        /** end
         *
         * Pointer to the past-the-end element (next element to rear_idx)
         * The past-the-end element is the theoretical element that would follow
         * the last element in the array. It does not point to any element, and
         * thus shall not be dereferenced.
         *
         * @return pointer to the past-the-end element
         */
        T* end ()
        {
            if(rear_idx==CircularArray::max_size-1 || rear_idx == -1)
            {
                return std::__addressof(data[0]);
            }
            else
            {
                return std::__addressof(data[rear_idx+1]);
            }
        }


        /** xend
         *
         * Pointer to the last memory block in the array
         *
         * @return pointer to data[N-1]
         */
        T* xend (){return std::__addressof(data[CircularArray::max_size-1]);}

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
            if((rear_idx == CircularArray::max_size-1 && front_idx==0) || front_idx==rear_idx+1)
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
            if (empty())
            {
                return 0;
            }
            else
            {
                return 1 + (CircularArray::max_size - front_idx + rear_idx)%CircularArray::max_size;
            }
        };

        /** @brief capacity
         *
         *  maximum number of elements the array is able to hold
         *
         *  @param void.
         *  @return capacity
         */
        size_t capacity() const
        {
            return CircularArray::max_size;
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

    /** @brief cyclic_iterator
     *
     *  this is the circular array iterator
     *
     */

    template< typename I >
    class cyclic_iterator
    {
        I* it;
        I *xstart;
        I* last;
        I* xlast;
    public:
        /** constructor **/
        cyclic_iterator( CircularArray<I>& b )
            : it(b.begin()), xstart(b.xbegin()), last(b.end()), xlast(b.xend()) {}

        /** increment operator **/
        cyclic_iterator &operator++()
        {
            ++it;
            if ( it == xlast+1 )
            {
                it = xstart;
            }
        }

        /** equal bolean operator **/
        friend bool operator==
            ( cyclic_iterator const &lhs, cyclic_iterator const &rhs )
            { return lhs.it == rhs.it; }

        /** diferent boolena operator **/
        friend bool operator!=
            ( cyclic_iterator const &lhs, cyclic_iterator const &rhs )
            { return lhs.it != rhs.it; }

        /** contemt operator **/
        I& operator*(){ return *it; }

        /** itx
         *
         * get element
         *
         * @return pointer to the current element
         * */
        I* itx(){ return this->it; }

         /** end
         *
         * Pointer to the past-the-end element (next element to rear_idx)
         * The past-the-end element is the theoretical element that would follow
         * the last element in the array. It does not point to any element, and
         * thus shall not be dereferenced.
         *
         * @return pointer to the past-the-end element
         */
        I* end(){ return this->last; }
    };


}
#endif

