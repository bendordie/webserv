/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebSession.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 15:26:57 by cshells           #+#    #+#             */
/*   Updated: 2022/02/02 15:26:58 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebSession.hpp"

WebSession::WebSession(WebServer *master, int fd)
: FdHandler(fd, true), _request_received(false), _processed(false), _master(master), _request(0) {}

WebSession::~WebSession() {}

bool WebSession::wantWrite() { return !_processed; }

void WebSession::sendMessage(const char *msg) {
    send(getFd(), msg, strlen(msg), 0);
}

int WebSession::countControlCharSequenceLen(const char *start, const char *end) {

    int i = 0;
    int range = end - start;

    if (*start == '\r') {
        for (; i < range && start[i] == '\r' && start[i + 1] == '\n'; i += 2) {}
    }

    return i;
}

std::string WebSession::defineRequestType(const char *begin) {
    if (!strncmp(begin, "GET", strlen("GET")))
        return "GET";
    else if (!strncmp(begin, "POST", strlen("POST")))
        return "POST";
    else if (!strncmp(begin, "DELETE", strlen("DELETE")))
        return "DELETE";

    return "";
}

const char* WebSession::defineHeaderEnd(const char *begin) {

    const char    *header_end;

    header_end = strstr(begin, "\r\n\r\n");
    if (header_end) {
        std::cout << "END DETECTED" << std::endl;
        header_end += strlen("\r\n\r\n");
    }
    else
        std::cout << "NO END" << std::endl;

    return header_end;
}

bool WebSession::receiveData(char *buffer, size_t &bytesRead) {

    bytesRead = recv(getFd(), buffer, buffer_size, 0);
    buffer[bytesRead] = '\0';
    if (bytesRead < 0) {
        std::cout << "Read socket: Error has been occurred" << std::endl;
        return false;
    }
    if (bytesRead == 0) {
        std::cout << "Client " << getFd() << " has closed the connection" << std::endl;
        return false;
    }
    return true;
}

bool WebSession::handlePostData(HttpRequest *request) {

//    bool cgi_request = false;
//
//    if (cgi_request) {
//        _cgi->handleRequest(request);
//    }

    const char      *data_begin = request->getData().begin().base();
    const long      &data_size = request->getDataSize();
    const string    &path = request->getPath();
    const string    &content_type = request->getContentType();
    string          uri = "." + path + "/data_from_client_fd_"
                                + std::to_string(getFd()) + "." + content_type;
    std::ofstream   ofs;

    request->setUri(uri);

    std::cout << "Data path: " << uri << std::endl;
    std::cout << "Data size: " << data_size << std::endl;
    ofs.open(uri.c_str(), std::fstream::binary);
    if (ofs.is_open()) {
        ofs.write(data_begin, data_size);
        if (ofs.fail()) {
            std::cout << "Error: Can't write data from Client " << getFd() << std::endl;
            return false;
        }
    } else {
        std::cout << "Error: Can't open file to save data from Client " << getFd() << std::endl;
        return false;
    }
    return true;
}

void WebSession::handleRequest() {

    char            *buffer = new char[buffer_size + 1];
    size_t          bytes_read;
    string          msg = "";

    bool receiveFailed = !receiveData(buffer, bytes_read);
    if (receiveFailed) {
        _master->removeSession(this);
        return;
    }

    const char  *start;
    const char  *end;

    if (!_tmp_data.empty()) {
        size_t  extended_buf_size = _tmp_data.size() + bytes_read;
        char    *extended_buf = new char[extended_buf_size];

        std::memcpy(extended_buf, _tmp_data.begin().base(), _tmp_data.size());
        std::memcpy(extended_buf, buffer, bytes_read);

        start = extended_buf;
        end = extended_buf + extended_buf_size;
    } else {
        start = buffer;
        end = buffer + bytes_read;

        for (list<HttpRequest*>::iterator it = _request_list.begin(); it != _request_list.end(); ++it) {
            if ((*it)->getType() == "POST" && !(*it)->getFullReceivedFlag()) {
                size_t  remainder = (*it)->getRestDataSize();

                if (remainder > bytes_read) {
                    (*it)->setData(start, end);
                    handlePostData((*it));
                    return;
                }
                (*it)->setData(start, start + remainder);
                handlePostData((*it));
                (*it)->setFullReceivedFlag(true);

                start += remainder;
                break;
            }
        }
    }

    int         num_chars_to_skip;
    const char  *request_begin;
    string      request_type;
    const char  *header_end;
    HttpRequest *new_request;

    for (;;) {
        num_chars_to_skip = countControlCharSequenceLen(start, end);
        request_begin = start + num_chars_to_skip;
        request_type = defineRequestType(request_begin);

        if (request_type.empty()) {
            std::cout << "BAD REQUEST" << std::endl;
            _master->removeSession(this);
            // create BAD REQUEST and add it to list
            return;
        }

        header_end = defineHeaderEnd(request_begin);
        if (!header_end) {
            _tmp_data.assign(request_begin, end);
            return;
        }
        new_request = HttpRequest::createRequest(request_type, request_begin,
                                                 header_end, bytes_read - num_chars_to_skip);
        if (!new_request) {
            std::cout << "BAD REQUEST" << std::endl;
            _master->removeSession(this);
            return;
        }

        if (new_request->getType() == "POST") {
            handlePostData(new_request);
        }

        _request_list.push_back(new_request);
        start += new_request->getSize();
    }
    _tmp_data.clear();
}

void WebSession::sendResponse(HttpResponse *response) {

    bool send_ok;

    send_ok = send(getFd(), response->getData().begin().base(), response->getData().size(), 0) >= 0;
    if (!send_ok)
        std::cout << "Send error" << std::endl;
}

void WebSession::handleResponse() {

    HttpResponse                    *response;
    list<HttpRequest*>::iterator    it = _request_list.begin();

    if (it == _request_list.end())
        return;

    response = HttpResponse::createResponse(*it, _master);
    if (!response) {
        std::cout << "No response" << std::endl;
        return;
    }
    sendResponse(response);
    if (!(*it)->isKeepAlive()) {
        _master->removeSession(this);
    }
}

void WebSession::Handle(bool read, bool write) {

    if (read) {
        handleRequest();
    } else if (write) {
        handleResponse();
    }
}

void WebSession::ReadAndIgnore() {}

void WebSession::ReadAndCheck() {}

void WebSession::CheckLines() {}

void WebSession::ProcessLine(const char *str) {}

