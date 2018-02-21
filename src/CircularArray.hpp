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
        CircularArray()
        {
            front=-1;rear=-1;
        }
        void insert(const base::Time &ts);
        void remove();
    };
}
#endif

