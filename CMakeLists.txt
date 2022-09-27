cmake_minimum_required(VERSION 3.21)
project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv src/webserv.cpp src/EventSelector.hpp src/EventSelector.cpp src/WebServer.hpp src/WebServer.cpp src/WebSession.hpp src/WebSession.cpp src/HttpMessage.hpp src/HttpMessage.cpp src/Request.hpp src/Request.cpp src/Response.hpp src/Response.cpp src/Utils.hpp src/Utils.cpp src/FdHandler.hpp src/FdHandler.cpp src/Config.hpp src/global_variables.hpp)
