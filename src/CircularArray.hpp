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
        size_t elements_count;

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
         *  at the front of the CircularArray
         *
         *  @param ts the element.
         *  @return void.
         */
        void push_front(const T &ts)
        {
            if(this->full())
            {
                /** The buffer is full now, so pushing subsequent
                 elements will overwrite the back-most elements. **/
                if(rear_idx==CircularArray::max_size-1)
                    rear_idx=0;
                else
                    rear_idx++;

                this->elements_count--;
            }

            if(front_idx == -1)
            {
                front_idx++;
                rear_idx++;
            }
            else if(front_idx==CircularArray::max_size-1)
                front_idx=0;
            else
                front_idx++;

            data[front_idx] = ts;
            this->elements_count++;
            //std::cout<<"["<<front_idx<<"]"<<data[front_idx]<<" front inserted"<<std::endl;
            return;

        };

        /** @brief insert an element
         *
         *  This methods insert a new element
         *  at the rear of the CircularArray
         *
         *  @param ts the element.
         *  @return void.
         */
        void push_back(const T &ts)
        {
            if(this->full())
            {
                /** The buffer is full now, so pushing subsequent
                 elements will overwrite the front-most elements. **/
                if(front_idx==CircularArray::max_size-1)
                    front_idx=0;
                else
                    front_idx++;

                this->elements_count--;
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
            this->elements_count++;
            //std::cout<<"["<<rear_idx<<"]"<<data[rear_idx]<<" back inserted"<<std::endl;
            return;

        };

        /** @brief remove an element
         *
         *  This methods remove an element
         *  from the front of the CircularArray
         *
         *  @param void.
         *  @return void.
         */
        T pop_front()
        {
            T ts =  base::NaN<T>(); 

            if(this->empty())
            {
                return ts;
            }

            //std::cout<<"["<<front_idx<<"]"<<data[front_idx]<<" front deleted"<<std::endl;
            ts = data[front_idx];
            this->elements_count--;

            if(front_idx==CircularArray::max_size-1)
                front_idx=0;
            else
                front_idx++;

            return ts;
        };

        /** @brief remove an element
         *
         *  This methods remove an element
         *  from the rear of the CircularArray
         *
         *  @param void.
         *  @return void.
         */
        T pop_back()
        {
            T ts =  base::NaN<T>(); 

            if(this->empty())
            {
                return ts;
            }

            //std::cout<<"["<<rear_idx<<"]"<<data[rear_idx]<<" back deleted"<<std::endl;
            ts = data[rear_idx];
            this->elements_count--;

            if(rear_idx==0)
                rear_idx=CircularArray::max_size-1;
            else
                rear_idx--;

            return ts;
        };

        /** @brief front
         *
         * It gives the front element without
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

       /** rend
        *
        * The element preceding the first element in the array
        * (which is considered its reverse end).
        * */
        T* rend()
        {
            if(front_idx==CircularArray::max_size-1 || front_idx == -1)
            {
                return std::__addressof(data[0]);
            }
            else
            {
                return std::__addressof(data[front_idx+1]);
            }
        }

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

        /** rbegin
         *
         * pointing to the last element in the vector
         *
         * */
        T* rbegin()
        {
                return std::__addressof(data[rear_idx]);
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
            if(this->elements_count == 0)
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
            if (this->elements_count == this->CircularArray::max_size)
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
            return this->elements_count;
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
            for (size_t i = 0; i<data.size(); ++i)
            {
                data[i] = base::unset<T>();
            }

            this->front_idx=-1; this->rear_idx=-1;
            this->elements_count = 0;
        };
    };

    /** @brief cyclic_iterator
     *
     *  this is the circular array iterator
     *
     */

    template< typename I = base::Time, size_t N = 10>
    class cyclic_iterator
    {
        I* it;
        I *start;
        I *xstart;
        I* last;
        I* xlast;
        size_t counter;
    public:
        /** constructor **/
        cyclic_iterator( CircularArray<I, N>& b )
            : it(b.begin()), start(b.begin()), xstart(b.xbegin()), last(b.end()), xlast(b.xend()), counter(b.size()) {}

        /** increment operator **/
        cyclic_iterator &operator++()
        {
            ++it;
            if ( it == xlast+1 )
            {
                it = xstart;
            }
            counter--;
            return *this;
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
        I* end()
        {
            /** Work around to make the case when end() == begin() **/
            if (counter == 0)
            {
                return this->last;
            }
            else
                return NULL;
        }
    };

    /** @brief cyclic_reverse_iterator
     *
     *  this is the circular array reverse iterator
     *
     */

    template< typename I = base::Time, size_t N = 10>
    class cyclic_reverse_iterator
    {
        I* it;
        I *start;
        I *xstart;
        I* last;
        I* xlast;
        size_t counter;
    public:
        /** constructor **/
        cyclic_reverse_iterator( CircularArray<I, N>& b )
            : it(b.rbegin()), start(b.rbegin()), xstart(b.xbegin()), last(b.rend()), xlast(b.xend()), counter(b.size()) {}

        /** increment operator **/
        cyclic_reverse_iterator &operator++()
        {
            ++it;
            if ( it == xlast+1 )
            {
                it = xstart;
            }
            counter--;
            return *this;
        }

        /** equal bolean operator **/
        friend bool operator==
            ( cyclic_reverse_iterator const &lhs, cyclic_reverse_iterator const &rhs )
            { return lhs.it == rhs.it; }

        /** diferent boolena operator **/
        friend bool operator!=
            ( cyclic_reverse_iterator const &lhs, cyclic_reverse_iterator const &rhs )
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
        I* rend()
        {
            /** Work around to make the case when rend == rbegin() **/
            if (counter == 0)
            {
                return this->last;
            }
            else
                return NULL;
        }
    };



}
#endif

