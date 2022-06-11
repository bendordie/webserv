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

int WebSession::_idGenerator = 0;

WebSession::WebSession(WebServer *master, int fd)
    : FdHandler(fd, true), _beginTime(chrono::high_resolution_clock::now()), _keepAlive(false),
    _processed(false), _master(master) {

    _id = _idGenerator++;
}

WebSession::~WebSession() {}

bool WebSession::wantBeWritten() const { return !_requestQueue.empty(); }//!_processed; }

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

void WebSession::handleRequest() {

    char            *buffer = new char[buffer_size + 1];
    size_t          bytesRead;
    string          msg;

    bool receiveSuccess = receiveData(buffer, bytesRead);
    if (!receiveSuccess) {
        _master->removeSession(this);
        return;
    }

    {
        std::cout << "WebSession " << getFd() << ": Data has been received" << std::endl;
        std::cout << "WebSession " << getFd() << ": Bytes read: " << bytesRead << std::endl;
        std::cout << "WebSession " << getFd() << ": Data: " << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << buffer << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
    } // system info output

    const char  *begin;
    const char  *end;

    if (!_tempBuffer.empty()) { // data from client had been received partially, other part had been stored in temp buffer, it needs to be concatenated

        _tempBuffer.insert(_tempBuffer.end(), buffer, buffer + bytesRead);
        begin = _tempBuffer.begin().base();
        end = _tempBuffer.end().base();

    } else if (!_requestQueue.empty()) { // data from client is fully received OR it's the first entry

        std::cout << "WebSession " << getFd() << ": Temporary buffer is empty. Preparing to handle raw data..." << std::endl;

        begin = buffer;
        end = buffer + bytesRead;

        auto previousRequest = _requestQueue.back();
        if (previousRequest->getType() == "POST" && !previousRequest->isFullReceived()) {

            previousRequest->setDataBegin(buffer);
            const char *dataEnd = HttpMessage::findMessageEnd(buffer);
            if (dataEnd) {
                previousRequest->setDataEnd(dataEnd);
                previousRequest->setFullReceived(true);
                begin = dataEnd;
            }
            else {
                previousRequest->setDataEnd(buffer + bytesRead);
                begin = end;
            }
            handlePostRequest(previousRequest);
        }
    } else {
        begin = buffer;
        end = buffer + bytesRead;
    }

    for (; begin < end; ) {
        std::cout << "WebSession " << getFd() << ": Checking if the message is complete" << std::endl;
        const char *messageEnd = HttpMessage::findMessageEnd(begin);
        if (!messageEnd) {
            if (end - begin > MAX_ENTITY_SIZE) {
                _master->removeSession(this);
                // 413 ENTITY TO LARGE
            }
            std::cout << "WebSession " << getFd() << ": Message has no end, waiting for next handling" << std::endl;
            _tempBuffer.insert(_tempBuffer.end(), begin, end);
            return;
            // NEED NEXT HANDLING
        }

        HttpRequest *newRequest = HttpRequest::createRequest(begin, end, _master);
        if (!newRequest) {
            std::cout << "WebSession " << getFd() << ": 400 BAD REQUEST" << std::endl;
            _master->removeSession(this);
            return;
        }

        const string         &uri = newRequest->getUri();
        const string         &type = newRequest->getType();
        const string         hostname = newRequest->getHeaderEntryValue("Hostname");
        const ServerOptions  *urlOptions = _master->getLocationOptions(hostname, uri);

        if (!urlOptions->isMethodAllowed(type)) {
            std::cout << "WebSession " << getFd() << ": 405 METHOD NOT ALLOWED" << std::endl;
            _master->removeSession(this);
        }
        
        _requestQueue.push(newRequest);
        std::cout << "WebSession " << getFd() << ": New request has been added to list" << std::endl;

        if (newRequest->getType() == "POST") {
            handlePostRequest(newRequest);
        }
        begin += newRequest->getSize();
    }
    _tempBuffer.clear();
    std::cout << "WebSession " << getFd() << ": " << _requestQueue.size() << " requests have been defined" << std::endl;
}

void WebSession::handlePostRequest(HttpRequest *request) {

    if (request->isChunked())
        decodeChunkedTransfer(request);

    const string          &uri = request->getUri();
    const string          hostname = request->getHeaderEntryValue("Hostname");
    const ServerOptions   *urlOptions = _master->getLocationOptions(hostname, uri);
    std::cout << "WebSession " << getFd() << ": Preparing to handle POST data..." << std::endl;
    handlePostData(request, urlOptions);
}

bool WebSession::handlePostData(HttpRequest *request, const ServerOptions *urlOptions) {

    std::cout << "WebSession " << getFd() << ": Handling POST data..." << std::endl;

    const char     *dataBegin = request->getData();
    const long     &dataSize = request->getBuffedDataSize();
    const string   &contentType = request->getContentType();
    const string   &requestedUri = request->getUri();
    string         absolutePath =  urlOptions->getRoot() + requestedUri;
    string         uri = absolutePath + "/data_session_" +
            std::to_string(_id) + "r" + std::to_string(request->getId()) + "." + contentType;

    if (request->getHandledDataSize() + dataSize > urlOptions->getClientBodySize()) {
        // 413 ENTITY TOO LARGE
        _master->removeSession(this);
    }
    request->setDataPath(uri);

    std::cout << "WebSession " << getFd() << ": POST Data path: " << uri << std::endl;
    std::cout << "WebSession " << getFd() << ": POST Data size: " << dataSize << std::endl;

    std::ofstream   ofs;

    ofs.open(uri.c_str(), fstream::binary | fstream::app);
    if (ofs.is_open()) {
        ofs.write(dataBegin, dataSize);
        if (ofs.fail()) {
            std::cout << "WebSession " << getFd() << ": Error: Can't write data from Client " << getFd() << std::endl;
            return false;
        }
    } else {
        std::cout << "WebSession " << getFd() << ": Error: Can't open file to save data from Client " << getFd() << std::endl;
        return false;
    }
    request->increaseHandledDataSize(dataSize);

//    size_t   handledDataSize = request->getHandledDataSize();
//    if (!request->isChunked()) {
//        long long   content_length = request->getContentLength();
//
//        if (content_length > handledDataSize)
//            request->setFullReceived(false);
//        else
//            request->setFullReceived(true);
//    }

    return true;
}

void WebSession::decodeChunkedTransfer(HttpRequest *request) {

    const char      *dataBegin = request->getDataBegin();
    const char      *data_end = request->getDataEnd();
    string          raw_data(dataBegin, data_end);
    vector<string>  rows = Utils::split(raw_data, "\r\n");
    int             chunk_size = stoi(rows[0]);  // TODO: exception handler

    for (int i = 1; chunk_size > 0 && i + 1 < rows.size(); ++i) {
        request->appendData(rows[i].begin().base(), rows[i].end().base());
        chunk_size = stoi(rows[i + 1]);
        if (chunk_size == 0) {
            request->setFullReceived(true);
            break;
        }
    }
}

void WebSession::handleResponse() {

    if (_requestQueue.empty())
        return;

    HttpResponse  *response;
    HttpRequest   *request = _requestQueue.front();

    _keepAlive = request->isKeepAlive();
    std::cout << "WebSession " << getFd() << ": Keep alive has been set to " << _keepAlive << std::endl;
    response = HttpResponse::createResponse(request, _master);
    if (!response) {
        std::cout << "WebSession " << getFd() << ": No response" << std::endl;
        return;
    }
    sendResponse(response);
    if (response->isFullProcessed())
        _requestQueue.pop();
    if (_requestQueue.empty()) {
        std::cout << "WebSession " << getFd() << ": Has been processed" << std::endl;
        _processed = true;
    }
    if (!_keepAlive && _processed) {
        _master->removeSession(this);
    }
}

void WebSession::sendResponse(HttpResponse *response) {

    bool send_ok;

    send_ok = send(getFd(), response->getData(), response->getDataSize(), 0) >= 0;
    std::cout << "WebSession " << getFd() << ": Response:" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << string(response->getData(), response->getData() + response->getDataSize()) << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "WebSession " << getFd() << ": Response has been sent" << std::endl;
    if (!send_ok)
        std::cout << "WebSession " << getFd() << ": Send error" << std::endl;
}

void WebSession::handle(bool read, bool write) {

    if (read) {
        std::cout << "WebSession " << getFd() << ": Handling request..." << std::endl;
        handleRequest();
    } else if (write) {
        std::cout << "WebSession " << getFd() << ": Handling response..." << std::endl;
        handleResponse();
    } else
        std::cout << "WebSession " << getFd() << ": Nothing to do" << std::endl;
}

const chrono::time_point<chrono::steady_clock>& WebSession::getStartTime() const { return _beginTime; }


// 0 [GET header CRLF]       OK
// 1 [GET hea]  [der CRLF]   OK
// 2 [GET]  [header CRLF]    OK

// 3 [POST header CRLF data CRLF]       OK
// 4 [POST header CRLF d]  [ata CRLF]   OK
// 5 [POST hea]  [der CRLF data CRLF]   OK

// 6 [GET header CRLF POST header CRLF data CRLF]         OK
// 7 [GET hea]  [der CRLF POST header CRLF data CRLF]     OK
// 8 [POST header CRLF d] [ata CRLF GET head] [er CRLF]