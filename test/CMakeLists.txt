rock_testsuite(circulararray-test test_circulararray.cpp
    DEPS_PKGCONFIG base-types)

rock_testsuite(timestamp-test test_timestamp.cpp
    DEPS stream_aligner
    DEPS_PKGCONFIG base-types)

rock_testsuite(streamaligner-test test_streamaligner.cpp
    PullStreamAligner.hpp
    DEPS stream_aligner
    DEPS_PKGCONFIG base-types)

rock_executable(example-usage test_example_usage.cpp
    DEPS stream_aligner
    DEPS_PKGCONFIG base-types)

