cmake_minimum_required(VERSION 3.20)
project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv webserv.cpp EventSelector.hpp EventSelector.cpp WebServer.hpp WebServer.cpp WebSession.hpp WebSession.cpp HttpMessage.hpp HttpMessage.cpp HttpRequest.hpp HttpRequest.cpp HttpResponse.hpp HttpResponse.cpp Utils.hpp Utils.cpp ServerTraits.hpp ServerTraits.cpp FdHandler.hpp FdHandler.cpp Config.hpp)
