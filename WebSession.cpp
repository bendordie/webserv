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
: FdHandler(fd, true), _request_received(false), _keep_alive(false),
_processed(false), _master(master), _request(0) {}

WebSession::~WebSession() {}

bool WebSession::wantBeWritten() const { return !_processed; }

int WebSession::countControlCharSequenceLen(const char *start, const char *end) {

    int i = 0;
    int range = end - start;

    if (*start == '\r') {
        for (; i < range && start[i] == '\r' && start[i + 1] == '\n'; i += 2) {}
    }

    return i;
}

// ABCD /* HTTP/1.

bool WebSession::isRequestLineValid(const char *request_begin) const {

    const char *end = strchr(request_begin, '\r');
    if (!end)
        return false;

    const char  *first_space = strchr(request_begin, ' ');
    if (!first_space || !(first_space + 1))
        return false;

    const char  *second_space = strchr(first_space + 1, ' ');
    if (!second_space || !(second_space + 1))
        return false;

    for (; *request_begin != ' '; ++request_begin) {
        if (!isupper(*request_begin))
            return false;
    }
    ++request_begin;
    if (*request_begin != '/')
        return false;

    size_t  len = end - second_space - 1;

    if (strncmp(second_space + 1, "HTTP/1.0", len) && strncmp(second_space + 1, "HTTP/1.1", len))
        return false;

    std::cout << "WebSession " << getFd() << ": Request line is valid" << std::endl;
    return true;
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
    std::cout << "WebSession " << getFd() << ": Defining header end..." << std::endl;

    const char    *header_end;

    header_end = strstr(begin, "\r\n\r\n");
    if (header_end) {
        std::cout << "WebSession " << getFd() << ": Header end exists" << std::endl;
        header_end += strlen("\r\n\r\n");
    }
    else
        std::cout << "WebSession " << getFd() << ": Header end not found" << std::endl;

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
    std::cout << "WebSession: Data from client has been received" << std::endl;
    return true;
}

void WebSession::handleRequestPipeline(const char *buffer, size_t bytes_read) {

}

//const char* WebSession::handleHeaderEnd(const char *begin, const char *end) {
//    std::cout << "WebSession " << getFd() << ": Trying to concatenate data..." << std::endl;
//
//    std::cout << "WebSession " << getFd() << ": Defining header end..." << std::endl;
//    const char  *header_end = defineHeaderEnd(begin);
//    if (!header_end) {
//        std::cout << "WebSession " << getFd() << ": Header end not found. Saving data received for next handling..." << std::endl;
//        if (_tmp_data.size() + end - begin > 4000) {
//            std::cout << "WebSession " << getFd() << ": Entity too large" << std::endl;
//            _master->removeSession(this);
//        }
//        std::cout << "WebSession " << getFd() << ": Saving data received for next handling..." << std::endl;
//        _tmp_data.insert(_tmp_data.end(), begin, end);
//        return 0;
//    }
//    if (_tmp_data.size() + header_end - begin > 4000) {
//        std::cout << "WebSession " << getFd() << ": Entity too large" << std::endl;
//        _master->removeSession(this);
//        return;
//    }
//}

HttpRequest* WebSession::retrieveRequestFromBuffer(const char *request_begin) {

}

size_t WebSession::addNewRequest(const string &type, const char *header_begin, const char *header_end, size_t buf_size) {

    std::cout << "WebSession " << getFd() << ": Trying create new request..." << std::endl;
    HttpRequest* new_request = HttpRequest::createRequest(type, header_begin,
                                             header_end, buf_size);
    if (!new_request) {
        std::cout << "WebSession " << getFd() << ": 400 BAD REQUEST" << std::endl;
        _master->removeSession(this);
    }
    if (new_request->getType() == "POST") {
        std::cout << "WebSession " << getFd() << ": Preparing to handle POST data..." << std::endl;
        handlePostData(new_request);
    }
    _request_list.push_back(new_request);
    std::cout << "WebSession " << getFd() << ": New request has been added to list" << std::endl;

    return new_request->getSize();
}

//bool WebSession::checkThis(const char *header_end, size_t request_size1, size_t request_size2) {
//
//    if (!header_end) {
//        std::cout << "WebSession " << getFd() << ": Header end not found. Saving data received for next handling..." << std::endl;
//        if (_tmp_data.size() + bytes_read > 4000) {
//            std::cout << "WebSession " << getFd() << ": Entity too large" << std::endl;
//            _master->removeSession(this);
//            return;
//        }
//        std::cout << "WebSession " << getFd() << ": Saving data received for next handling..." << std::endl;
//        _tmp_data.insert(_tmp_data.end(), buffer, buffer + bytes_read);
//        return;
//    }
//    if (_tmp_data.size() + header_end - buffer > 4000) {
//        std::cout << "WebSession " << getFd() << ": Entity too large" << std::endl;
//        _master->removeSession(this);
//        return;
//    }
//}



void WebSession::handleRequest() {

    char            *buffer = new char[buffer_size + 1];
    size_t          bytes_read;
    string          msg = "";

    bool receiveFailed = !receiveData(buffer, bytes_read);
    if (receiveFailed) {
        _master->removeSession(this);
        return;
    }

    {
        std::cout << "WebSession " << getFd() << ": Data has been received" << std::endl;
        std::cout << "WebSession " << getFd() << ": Bytes read: " << bytes_read << std::endl;
        std::cout << "WebSession " << getFd() << ": Data: " << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << buffer << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
    } // system info output

    const char  *start;
    const char  *end;

    if (!_tmp_data.empty()) { // data from client had been received partially, other part had been stored in temp buffer, it needs to be concatenated

        std::cout << "WebSession " << getFd() << ": Trying to concatenate data..." << std::endl;

        const char  *header_end = defineHeaderEnd(buffer);
        if (!header_end) {
            if (_tmp_data.size() + bytes_read > 4000) {
                std::cout << "WebSession " << getFd() << ": Entity too large" << std::endl;
                _master->removeSession(this);
                return;
            }
            std::cout << "WebSession " << getFd() << ": Saving data received for next handling..." << std::endl;
            _tmp_data.insert(_tmp_data.end(), buffer, buffer + bytes_read);
            return;
        }
        if (_tmp_data.size() + header_end - buffer > 4000) {
            std::cout << "WebSession " << getFd() << ": Entity too large" << std::endl;
            _master->removeSession(this);
            return;
        }
        _tmp_data.insert(_tmp_data.end(), static_cast<const char*>(buffer), header_end);
        addNewRequest(_tmp_request_type, _tmp_data.begin().base(), header_end, _tmp_data.size());
        _tmp_data.clear();
        start = header_end;
        end = buffer + bytes_read;
    } else { // data from client is fully received

        std::cout << "WebSession " << getFd() << ": Temporary buffer is empty. Preparing to handle raw data..." << std::endl;

        start = buffer;
        end = buffer + bytes_read;

        for (list<HttpRequest*>::iterator it = _request_list.begin(); it != _request_list.end(); ++it) {
            if ((*it)->getType() == "POST" && !(*it)->getFullReceivedFlag()) {

                std::cout << "WebSession " << getFd() << ": Receiving rest POST data" << std::endl;

                size_t  data_remainder = (*it)->getRestDataSize();

                if (data_remainder > bytes_read) {
                    (*it)->setData(start, end);
                    handlePostData((*it));
                    return;
                }
                (*it)->setData(start, start + data_remainder);
                handlePostData((*it));
                (*it)->setFullReceivedFlag(true);

                start += data_remainder;
                break;
            }
        }
    }


    int         num_chars_to_skip;
    const char  *request_begin;
    string      request_type;
    const char  *header_end;

    for (; start < end ;) {
//        std::cout << "WebSession " << getFd() << ": Start address: " << (void*)start << std::endl;
//        std::cout << "WebSession " << getFd() << ": End address: " << (void*)end << std::endl;
        std::cout << "WebSession " << getFd() << ": Preparing to find request..." << std::endl;
        num_chars_to_skip = countControlCharSequenceLen(start, end);
        std::cout << "WebSession " << getFd() << ": Number of control chars to skip: " << num_chars_to_skip << std::endl;
        request_begin = start + num_chars_to_skip;

        {
            std::cout << "WebSession " << getFd() << ": Request begin: " << std::endl;
            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << request_begin << std::endl;
            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << "WebSession " << getFd() << ": Defining request type..." << std::endl;
        } // system info output


        if (!isRequestLineValid(request_begin)) {
            std::cout << "WebSession " << getFd() << ": Request line is invalid" << std::endl;
            std::cout << "WebSession " << getFd() << ": 400 BAD REQUEST" << std::endl;
            _master->removeSession(this);
            // create BAD REQUEST and add it to list
            return;
        }

        request_type = defineRequestType(request_begin);
        std::cout << "WebSession " << getFd() << ": Request type: " << request_type << std::endl;

        if (request_type.empty()) {
            std::cout << "WebSession " << getFd() << ": 400 BAD REQUEST" << std::endl;
            _master->removeSession(this);
            // create BAD REQUEST and add it to list
            return;
        }
        std::cout << "WebSession " << getFd() << ": Defining header end..." << std::endl;
        header_end = defineHeaderEnd(request_begin);
        if (!header_end) {
            if (end - request_begin > 4000) {
                std::cout << "WebSession " << getFd() << ": Entity too large" << std::endl;
                _master->removeSession(this);
                return;
            }
            std::cout << "WebSession " << getFd() << ": Saving data received for next handling..." << std::endl;
            _tmp_request_type = request_type;
            _tmp_data.assign(request_begin, end);
            return;
        }
//        std::cout << "WebSession " << getFd() << ": Header end - 5: " << header_end - 5 << std::endl;
        if (header_end - request_begin > 4000) {
            std::cout << "WebSession " << getFd() << ": Entity too large" << std::endl;
            _master->removeSession(this);
            return;
        }
        size_t request_size = addNewRequest(request_type, request_begin, header_end, bytes_read - num_chars_to_skip);
        start += request_size;
    }
}

bool WebSession::handlePostData(HttpRequest *request) {

//    bool cgi_request = false;
//
//    if (cgi_request) {
//        _cgi->handleRequest(request);
//    }

    std::cout << "WebSession " << getFd() << ": Handling POST data..." << std::endl;

    const string                    &path = request->getPath();
    const WebServer::ServerTraits   *server_traits = _master->getLocationTraits(path);
    const WebServer::LocationTraits *location_traits = dynamic_cast<const WebServer::LocationTraits*>(server_traits);


    if (location_traits) {
        abs_path = location_traits->getRoot() + requested_path;
    } else {
        abs_path =  server_traits->getRoot() + requested_path;
    }

    const char      *data_begin = request->getData().begin().base();
    const long      &data_size = request->getDataSize();
    const string    &content_type = request->getContentType();
    string          uri = "." + path + "/data_from_client_fd_"
                          + std::to_string(getFd()) + "." + content_type;
    std::ofstream   ofs;

    request->setUri(uri);

    std::cout << "WebSession " << getFd() << ": POST Data path: " << uri << std::endl;
    std::cout << "WebSession " << getFd() << ": POST Data size: " << data_size << std::endl;
    ofs.open(uri.c_str(), std::fstream::binary);
    if (ofs.is_open()) {
        ofs.write(data_begin, data_size);
        if (ofs.fail()) {
            std::cout << "WebSession " << getFd() << ": Error: Can't write data from Client " << getFd() << std::endl;
            return false;
        }
    } else {
        std::cout << "WebSession " << getFd() << ": Error: Can't open file to save data from Client " << getFd() << std::endl;
        return false;
    }
    return true;
}

void WebSession::handleResponse() {

    HttpResponse                    *response;
    list<HttpRequest*>::iterator    it = _request_list.begin();

    if (it == _request_list.end())
        return;

    _keep_alive = (*it)->isKeepAlive();
    std::cout << "WebSession " << getFd() << ": Keep alive has been set to " << _keep_alive << std::endl;
    response = HttpResponse::createResponse(*it, _master);
    if (!response) {
        std::cout << "WebSession " << getFd() << ": No response" << std::endl;
        return;
    }
    sendResponse(response);
    _request_list.erase(it);
    if (_request_list.empty()) {
        std::cout << "WebSession " << getFd() << ": Has been processed" << std::endl;
        _processed = true;
    }
    if (!_keep_alive && _processed) {
        _master->removeSession(this);
    }
}

void WebSession::sendResponse(HttpResponse *response) {

    bool send_ok;

    send_ok = send(getFd(), response->getData().begin().base(), response->getData().size(), 0) >= 0;
    std::cout << "WebSession " << getFd() << ": Response has been sent" << std::endl;
    if (!send_ok)
        std::cout << "WebSession " << getFd() << ": Send error" << std::endl;
}

void WebSession::Handle(bool read, bool write) {

    if (read) {
        std::cout << "WebSession " << getFd() << ": Handling request..." << std::endl;
        handleRequest();
    } else if (write) {
        std::cout << "WebSession " << getFd() << ": Handling response..." << std::endl;
        handleResponse();
    } else
        std::cout << "WebSession " << getFd() << ": Nothing to do" << std::endl;
}
