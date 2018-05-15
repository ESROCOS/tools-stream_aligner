#ifndef STREAM_ALIGNER_PULL_STREAM_ALIGNER_HPP
#define STREAM_ALIGNER_PULL_STREAM_ALIGNER_HPP

#include <stream_aligner/StreamAligner.hpp>

namespace stream_aligner
{
    /**
     * PullStreamBase
     *
     * @brief asbtract class for pull the streams
     *
     */
    class PullStreamBase
    {
    public:
        PullStreamBase() : has_data(false) {}
        virtual ~PullStreamBase() {}
        virtual void pull() = 0;
        virtual void push() = 0; 
        virtual void copyState( const PullStreamBase& other ) = 0; 

        base::Time lastTime() const { return last_ts; }
        bool hasData() const { return has_data; }

    protected:
        base::Time last_ts;
        bool has_data;
    };

    /**
     * PullStream
     *
     * @brief template class to pull the streams
     *
     */
    template <class T, size_t BUFFER_SIZE, size_t NUMBER_STREAMS>
    class PullStream : public PullStreamBase
    {
    public:
        typedef boost::function<bool (base::Time&, T&)> pull_callback_t;

    protected:
        int stream_idx;
        StreamAligner<NUMBER_STREAMS> *sa;

        pull_callback_t pull_callback;
        T last_data;

    public:
        PullStream( pull_callback_t pull_callback, StreamAligner<NUMBER_STREAMS>* sa, size_t stream_index )
        : stream_idx( stream_index ), sa( sa ), pull_callback( pull_callback ) {}

        void pull()
        {
            has_data = pull_callback( last_ts, last_data );
        }

        void push()
        {
        if( has_data )
            sa->template push<T, BUFFER_SIZE>( stream_idx, last_ts, last_data );

        has_data = false;	
        }

        void copyState( const PullStreamBase& other )
        {
            const PullStream<T, BUFFER_SIZE, NUMBER_STREAMS> &pull_stream(static_cast<const PullStream<T, BUFFER_SIZE, NUMBER_STREAMS>& >(other));
            operator=( pull_stream );
        }

    };

    /**
     * PullStreamAligner
     *
     * @brief template class to pull the aligned streams
     *
     */
    template <size_t NUMBER_STREAMS>
    class PullStreamAligner : public StreamAligner<NUMBER_STREAMS>
    {
    protected:

        typedef std::array<PullStreamBase*, NUMBER_STREAMS> PullStreamVector;
        PullStreamVector pull_streams;

        static bool comparePullStreams( const PullStreamBase* b1, const PullStreamBase* b2 )
        {
            if(!b1)
                return false;

            if(!b2)
                return true;

            const base::Time &ts1(b1->lastTime());
            const base::Time &ts2(b2->lastTime());

            return ts1 < ts2 || !b2->hasData(); 
        }

    public:
        PullStreamAligner(): StreamAligner<NUMBER_STREAMS>()
        {
            for(size_t i = 0; i < this->pull_streams.size(); i++)
            {
                this->pull_streams[i] = NULL;
            }
        }

        ~PullStreamAligner()
        {
            for(typename PullStreamVector::iterator it=this->pull_streams.begin();it != this->pull_streams.end(); it++)
            {
                delete *it;
            }
        }

        template <class T, size_t BUFFER_SIZE>
        int registerPullStream( typename PullStream<T, BUFFER_SIZE, NUMBER_STREAMS>::pull_callback_t pull_callback,
            typename Stream<T, BUFFER_SIZE>::callback_t callback, base::Time period, int priority  = -1 ) 
        {
            int idx = this-> template registerStream<T, BUFFER_SIZE>(callback, period, priority);
            this->pull_streams[idx] = new PullStream<T, BUFFER_SIZE, NUMBER_STREAMS>(pull_callback, this, idx);
            return idx;
        }

        bool pull()
        {
            for(typename PullStreamVector::iterator it=this->pull_streams.begin();it != this->pull_streams.end();it++)
            {
                if(*it != NULL)
                {
                    if (!(*it)->hasData())
                    {
                        (*it)->pull();
                    }
                }
            }

            std::sort(this->pull_streams.begin(), this->pull_streams.end(), &comparePullStreams);

            PullStreamBase *first = this->pull_streams.front();
            if (first != NULL)
            {
                if(first->hasData())
                {
                    first->push();
                    return true;
                }
            }
            return false;
        }

        void copyState(const PullStreamAligner<NUMBER_STREAMS>& other)
        {
            this-> template copyState(other);

            assert(this->pull_streams.size() == other.pull_streams.size() );
            for(size_t i=0;i<this->pull_streams.size();i++)
            {
                this->pull_streams[i]->copyState( *other.pull_streams[i] );
            }
        }
    };
}
#endif
