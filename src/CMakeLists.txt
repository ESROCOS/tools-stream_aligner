set(headers CircularArray.hpp
            TimestampEstimator.hpp)

set(sources TimestampEstimator.cpp)

esrocos_library(stream_aligner
                HEADERS ${headers}
                SOURCES ${sources}
                DEPS_PKGCONFIG base-types)
