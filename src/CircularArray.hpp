#ifndef STREAM_ALIGNER_CIRCULAR_ARRAY_HPP
#define STREAM_ALIGNER_CIRCULAR_ARRAY_HPP

#include <array>
#include <base/Time.hpp>

namespace stream_aligner
{
    class CircularArray
    {
    private:
        static const std::size_t size = 10;
    protected:
        std::array<base::Time,size> data;
        int front,rear;
    public:
        /** @brief Constructor
         *
         *  @return void.
         */
        CircularArray()
        {
            front=-1;rear=-1;
        }

        /** @brief insert an element
         *
         *  This methods insert a new element
         *  in the CircularArray
         *
         *  @param ts the element.
         *  @return void.
         */
        bool push(const base::Time &ts);


        /** @brief remove an element
         *
         *  This methods remove an element
         *  from the CircularArray
         *
         *  @param void.
         *  @return void.
         */
        base::Time pop();

        /** @brief array empty
         *
         *
         *  @param void.
         *  @return true if array is empty. false otherwise.
         */
        bool empty();

        /** @brief array full
         *
         *
         *  @param void.
         *  @return true if array is full. false otherwise.
         */
        bool full();

        /** @brief size
         *
         *
         *  @param void.
         *  @return fixed size of the array.
         */
        size_t capacity();
    };
}
#endif

