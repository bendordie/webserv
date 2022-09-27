#ifndef WEBSERV_GLOBAL_VARIABLES_HPP
#define WEBSERV_GLOBAL_VARIABLES_HPP

#include <iostream>
#include <map>
#include <chrono>
#include <list>

using std::string;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
using PairList = std::list<std::pair<string, string>>;

#define DEBUG

namespace global {

    static const std::string&   APP_NAME = "webserv";
    static const int            SESSION_TIMEOUT_SEC = 10;
    static const int            MAX_CLIENT_MSG_SIZE = 1000000;
    static const int            MAX_SERVER_BODY_SIZE = 1000000;
    static const int            FILE_READING_BUFFER_SIZE = 65535;
    static const int            TCP_BUFFER_SIZE = 16384;

#ifdef DEBUG
    static const int            DEBUG_DELAY_SEC = 1;
#elif
    static const int            DEBUG_DELAY_SEC = 0;
#endif

    namespace response_status {

        static const int   CONTINUE = 100;
        static const int   OK = 200;
        static const int   CREATED = 201;
        static const int   MOVED_PERMANENTLY = 301;
        static const int   BAD_REQUEST = 400;
        static const int   FORBIDDEN = 403;
        static const int   NOT_FOUND = 404;
        static const int   NOT_ALLOWED = 405;
        static const int   ENTITY_TOO_LARGE = 413;
        static const int   INTERNAL_ERROR = 500;
    }

    static const std::map<int, std::string>   responseStatuses = {
            std::make_pair(response_status::OK, "OK"),
            std::make_pair(response_status::CONTINUE, "CONTINUE"),
            std::make_pair(response_status::CREATED, "CREATED"),
            std::make_pair(response_status::MOVED_PERMANENTLY, "MOVED PERMANENTLY"),
            std::make_pair(response_status::BAD_REQUEST, "BAD REQUEST"),
            std::make_pair(response_status::FORBIDDEN, "FORBIDDEN"),
            std::make_pair(response_status::NOT_FOUND, "NOT FOUND"),
            std::make_pair(response_status::NOT_ALLOWED, "NOT ALLOWED"),
            std::make_pair(response_status::ENTITY_TOO_LARGE, "REQUEST ENTITY TOO LARGE"),
            std::make_pair(response_status::INTERNAL_ERROR, "INTERNAL SERVER ERROR")
    };
}

#endif //WEBSERV_GLOBAL_VARIABLES_HPP
