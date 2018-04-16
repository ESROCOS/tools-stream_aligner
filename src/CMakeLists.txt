set(headers CircularArray.hpp
            TimestampConfig.hpp
            TimestampStatus.hpp
            TimestampEstimator.hpp
            StreamAligner.hpp)

set(sources TimestampEstimator.cpp)

esrocos_library(stream_aligner
                HEADERS ${headers}
                SOURCES ${sources}
                DEPS_PKGCONFIG base-types)
