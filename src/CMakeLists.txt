set(headers CircularArray.hpp
            TimestampStatus.hpp
            TimestampEstimator.hpp
            StreamAligner.hpp
	    StreamAlignerStatus.hpp)

set(sources TimestampEstimator.cpp
	    StreamAlignerStatus.cpp)

rock_library(stream_aligner
                HEADERS ${headers}
                SOURCES ${sources}
                DEPS_PKGCONFIG base-types)
