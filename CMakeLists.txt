cmake_minimum_required(VERSION 3.20)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv main.cpp Server.hpp Server.cpp ServerConfig.hpp Location.hpp Location.cpp)
