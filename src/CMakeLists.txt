set(HEADERS CircularArray.hpp
            TimestampEstimator.hpp)

set(SOURCES CircularArray.cpp)

add_library(libstream_aligner ${HEADERS} ${SOURCES})
