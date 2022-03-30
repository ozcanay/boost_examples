cmake_minimum_required(VERSION 3.21)
project(boost_examples)

find_package(Boost 1.76.0 REQUIRED system)

set(CMAKE_CXX_STANDARD 23)

add_executable(tcp_echo_server tcp_echo_server.cpp)
add_executable(blocking_tcp_echo_server blocking_tcp_echo_server.cpp)
add_executable(blocking_tcp_echo_client blocking_tcp_echo_client.cpp)
add_executable(chat_client chat_client.cpp)
add_executable(chat_server chat_server.cpp)

target_link_libraries(tcp_echo_server Boost::system)
target_link_libraries(blocking_tcp_echo_server Boost::system)
target_link_libraries(blocking_tcp_echo_client Boost::system)
target_link_libraries(chat_client Boost::system)
target_link_libraries(chat_server Boost::system)
