cmake_minimum_required(VERSION 2.6)
set(ROCK_USE_CXX11 TRUE)
find_package (rock)
set(ROCK_TEST_ENABLED ON)

option(COVERAGE "Enable code coverage. run 'make test && make coverage' to generate the coverage report. The report will be in ${CMAKE_BINARY_DIR}/cov" OFF)

if(COVERAGE)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-fprofile-arcs -ftest-coverage)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

        add_custom_target(coverage
          COMMAND lcov --directory test --capture --output-file cov.info
          COMMAND lcov --remove cov.info 'tests/*' '/usr/*' '*/install/include/*' --output-file cov.info.cleaned
          COMMAND genhtml -o ./cov cov.info.cleaned
          COMMAND cmake -E remove cov.info cov.info.cleaned
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    else()
        message(FATAL_ERROR "Code coverage only works in Debug versions" )
    endif()
endif()

rock_init(stream_aligner 0.1)
rock_standard_layout()
