esrocos_testsuite(circulararray-test test_circulararray.cpp
    DEPS_PKGCONFIG base-types)

esrocos_testsuite(timestamp-test test_timestamp.cpp
    DEPS stream_aligner
    DEPS_PKGCONFIG base-types)

esrocos_testsuite(streamaligner-test test_streamaligner.cpp
    PullStreamAligner.hpp
    DEPS stream_aligner
    DEPS_PKGCONFIG base-types)

