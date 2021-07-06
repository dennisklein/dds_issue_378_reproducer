cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
cmake_policy(VERSION 3.16..3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(dds_issue_378_reproducer VERSION 1 LANGUAGES CXX)

find_package(DDS REQUIRED QUIET)
message(STATUS "Using DDS ${DDS_VERSION}")

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED QUIET COMPONENTS program_options system filesystem log log_setup thread regex)
message(STATUS "Using Boost ${Boost_VERSION}")

set(tgt reproducer)
add_executable(${tgt} reproducer.cpp)
target_compile_features(${tgt} PRIVATE cxx_std_17)
target_link_libraries(${tgt} PRIVATE DDS::dds_intercom_lib DDS::dds_tools_lib)

set(tgt di378task)
add_executable(${tgt} di378task.cpp)
target_compile_features(${tgt} PRIVATE cxx_std_17)
target_link_libraries(${tgt} PRIVATE DDS::dds_intercom_lib)
