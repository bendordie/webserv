/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 17:57:30 by cshells           #+#    #+#             */
/*   Updated: 2021/12/16 17:57:32 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _socket(-1) {
    count++;
    _id = count;
    _data_recieved.reserve(4096);
//    _data_recieved = "";
//    _ready_to_send = false;
    _keep_alive = false;
    _processed = false;
    _timeout = false;
    _request_type = 0;
    _request_header = "";
    _request_path = "";

    bzero(&_time_connected, sizeof(_time_connected));
    if (gettimeofday(&_time_connected, NULL) < 0)
        std::cout << "Client time error" << std::endl;
}

Client::Client(int socket) : _socket(socket) {
    count++;
    _id = count;
    _data_recieved.reserve(4096);
//    _data_recieved = "";
//    _ready_to_send = false;
    _keep_alive = false;
    _processed = false;
    _timeout = false;
    _request_type = 0;
    _request_header = "";
    _request_path = "";

    bzero(&_time_connected, sizeof(_time_connected));
    if (gettimeofday(&_time_connected, NULL) < 0)
        std::cout << "Client time error" << std::endl;
}

Client::~Client() {}

const int &Client::getId() const { return _id; }

const int &Client::getSocket() const { return _socket; }

const std::vector<uint8_t> &Client::getData() const { return _data_recieved; }

const struct timeval Client::getConnectionTime() const { return _time_connected; }

const int &Client::getRequestType() const { return _request_type; }

const std::string &Client::getRequestHeader() const { return _request_header; }

const std::string &Client::getRequestPath() const { return _request_path; }

const long long int &Client::getPostContentLen() const { return _post_content_len; }

const std::string &Client::getPostContentType() const { return _post_content_type; }

//bool Client::isReadySend() const { return _ready_to_send; }

bool Client::isKeepAlive() const { return _keep_alive; }

bool Client::isProcessed() const { return _processed; }

bool Client::isTimeout() const { return _timeout; }

const std::vector<uint8_t> &Client::getResponse() const { return _response; }

//void Client::setReadySend(bool value) { _ready_to_send = value; }

void Client::setKeepAlive(bool value) { _keep_alive = value; }

void Client::setProcessed(bool value) { _processed = value; }

void Client::setTimeout(bool value) { _timeout = value; }

void Client::setResponse(const char *data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        _response.push_back((uint8_t)data[i]);
    }
}

void Client::setRequestType(int value) { _request_type = value; }

void Client::setRequestHeader(const char *begin, const char* end) {
    _request_header.assign(begin, end);

}

void Client::setRequestPath(const char *request_line) {
    const char    *first_space_pos;
    const char    *second_space_pos;

    if (request_line) {
        first_space_pos = strchr(request_line, ' ');
        if (!first_space_pos || !(first_space_pos + 1))
            return;
        second_space_pos = strchr(first_space_pos + 1, ' ');
        if (!second_space_pos)
            return;
        _request_path.assign(first_space_pos + 1, second_space_pos);
    }
}

void Client::setPostContentLen(const long long int &length) { _post_content_len = length; }

void Client::setPostContentType(const std::string &type) { _post_content_type = type; }

void Client::writeData(const char *begin, const char *end) {
  for (; begin != end; ++begin) {
      _data_recieved.push_back(static_cast<uint8_t>(*begin));
  }
}

void Client::clearData() {
    _data_recieved.clear();
}