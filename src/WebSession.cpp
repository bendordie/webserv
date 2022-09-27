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

WebSession::WebSession(WebServer* master, int fd)
    : FdHandler(fd, true), _beginTime(chrono::high_resolution_clock::now()),
    _isWritingPostDataOnDisk(false), _keepAlive(false), _processed(false), _master(master) {

    _id = _idGenerator++;
    _cookies.emplace_back("SESSIONID", to_string(_id) + "repgmreojhgieor312432");
    _mainBuffer = new char[global::TCP_BUFFER_SIZE];
}

WebSession::~WebSession() {
    showDebugMessage("WebSession " + to_string(getFd()) + ": Destructor called");

    delete [] _mainBuffer;
    showDebugMessage("WebSession " + to_string(getFd()) + ": Buffer cleared");

    queue<Request*>   empty;
    swap(_requestQueue, empty);

//    for (auto request = _requestQueue.front(); !_requestQueue.empty(); request = _requestQueue.front()) {
//        showDebugMessage("WebSession " + to_string(getFd()) + ": Request deleted");
//        delete request;
//        _requestQueue.pop();
//    }
}

bool WebSession::wantBeWritten() const { return !_requestQueue.empty(); }//!_processed; }

bool WebSession::receiveData(size_t& bytesRead) {

    bytesRead = recv(getFd(), _mainBuffer, global::TCP_BUFFER_SIZE, 0);
    _mainBuffer[bytesRead] = '\0';
    if (bytesRead < 0) {
        std::cerr << "WebSession " + to_string(getFd()) + ": Read socket: Error has been occurred" << std::endl;
        return false;
    }
    if (bytesRead == 0) {
        showDebugMessage("Client " + to_string(getFd()) + " has closed the connection");
        return false;
    }
    showDebugMessage("WebSession: Data from client has been received");

    return true;
}

void WebSession::handleRequest() {
    size_t   bytesRead;
    bool     receiveSuccess;

    receiveSuccess = receiveData(bytesRead);
    if (!receiveSuccess) {
        _master->removeSession(this);
        return;
    }

    {
        showDebugMessage("WebSession " + to_string(getFd()) + ": Data has been received");
        showDebugMessage("WebSession " + to_string(getFd()) + ": Bytes read: " + to_string(bytesRead));
        showDebugMessage("WebSession " + to_string(getFd()) + ": Data: ");
        showDebugMessage("--------------------------------------------------");
        showDebugMessage(_mainBuffer);
        showDebugMessage("--------------------------------------------------");
    } // src info output

    const char*   begin;
    const char*   end;

    if (_tempBuffer.empty()) {
        begin = _mainBuffer;
        end = _mainBuffer + bytesRead;
    } else { // request had not been created before, part of request had been stored in temp buffer, it needs to be concatenated
        _tempBuffer.insert(_tempBuffer.end(), _mainBuffer, _mainBuffer + bytesRead);
        begin = _tempBuffer.begin().base();
        end = _tempBuffer.end().base();
    }

    if (!_requestQueue.empty()) {

        auto   previousRequest = _requestQueue.back();
        if (previousRequest->getMethod() == "POST" && !previousRequest->isFullReceived()) {

            size_t        remainingBytes = previousRequest->getRemainingDataSize();
            const char*   dataEnd;

            showDebugMessage("WebSession " + to_string(getFd()) + ": Remaining POST bytes: " + to_string(remainingBytes));
            if (remainingBytes <= global::TCP_BUFFER_SIZE) {
                dataEnd = _mainBuffer + remainingBytes;
                begin = dataEnd;
                previousRequest->setBuffedDataSize(remainingBytes);
                previousRequest->setFullReceived(true);
                showDebugMessage("WebSession " + to_string(getFd()) + ": Full received flag set");
            } else {
                dataEnd = _mainBuffer + bytesRead;
                begin = end;
                previousRequest->setBuffedDataSize(bytesRead);
            }

            previousRequest->setDataBegin(_mainBuffer);
            previousRequest->setDataEnd(dataEnd);

            const Location*   location = previousRequest->getLocation();

            if (previousRequest->isChunked())
                decodeChunkedTransfer(_mainBuffer, dataEnd, previousRequest);

            if (previousRequest->isHandledByCgi())
                previousRequest->appendData(_mainBuffer, dataEnd);
            else {
                writeDataOnDisk(previousRequest, location);
                if (previousRequest->isFullReceived())
                    _isWritingPostDataOnDisk = false;
            }
        }
    }

    Request*   newRequest;
    if (begin < end) {

//        showDebugMessage("WebSession " + to_string(getFd()) + ": Buffer begin: " + string(begin, end));

        showDebugMessage("WebSession " + to_string(getFd()) + ": Checking if the message is complete");

        const char*   headerEnd = HttpMessage::findMessageEnd(begin);
        if (!headerEnd) {
            if (end - begin > global::MAX_CLIENT_MSG_SIZE) {
                newRequest = Request::createRequest(begin, end, global::response_status::ENTITY_TOO_LARGE);
                _requestQueue.push(newRequest);
                return;
                // 413 ENTITY TO LARGE
            }
            showDebugMessage("WebSession " + to_string(getFd()) + ": Message has no end, waiting for next handling");
            _tempBuffer.insert(_tempBuffer.end(), begin, end);
            return;
            // NEED NEXT HANDLING
        }

        newRequest = Request::createRequest(begin, end, _master);
        if (!newRequest) {
            showDebugMessage("WebSession " + to_string(getFd()) + ": Error: Request not created");
            _master->removeSession(this);
            return;
        }

        const Location*   location = newRequest->getLocation();

        _requestQueue.push(newRequest);

        showDebugMessage("WebSession " + to_string(getFd()) + ": New request has been added to list");

        if (newRequest->getMethod() == "POST") {
            if (newRequest->isChunked())
                decodeChunkedTransfer(newRequest->getBuffedDataBegin(), newRequest->getBuffedDataEnd(), newRequest);
            if (newRequest->isHandledByCgi())
                newRequest->setData(newRequest->getBuffedDataBegin(), newRequest->getBuffedDataEnd());
            else
                writeDataOnDisk(newRequest, location);
        }
    }
    _tempBuffer.clear();

    showDebugMessage("WebSession " + to_string(getFd()) + ": "
                                                + to_string(_requestQueue.size()) + " requests have been defined");
}

bool WebSession::writeDataOnDisk(Request *request, const Location *location) {

    showDebugMessage("WebSession " + to_string(getFd()) + ": Prepare to write POST data on disk...");

    const char*     dataBegin = request->getBuffedDataBegin();
    const long      dataSize = request->getBuffedDataSize();
    const string&   dataPath = request->getDataPath();

    if (dataPath.empty()) {
        string                       absolutePath =  location->getRoot();

        if (*absolutePath.rbegin() != '/')
            absolutePath.append("/");

        const map<string, string>&   contentTypes = _master->getContentTypes();
        const string&                contentType = request->getContentType();
        auto                         result = Utils::findKey(contentTypes.begin(),
                                                             contentTypes.end(), contentType);
        string                       fileExtension = (result == contentTypes.end() ? "bin" : result->first);
        string                       url = absolutePath
                                           + "data_session_"
                                           + std::to_string(_id)
                                           + "r"
                                           + std::to_string(request->getId())
                                           + "."
                                           + fileExtension; // todo: needs to generate better filename

        request->setDataPath(url);
    }

    if (request->getHandledDataSize() + dataSize > location->getClientBodySize()) {
        request->setResponseCode(global::response_status::ENTITY_TOO_LARGE);
        return false;
    }

    showDebugMessage("WebSession " + to_string(getFd()) + ": POST Data path: " + dataPath);
    showDebugMessage("WebSession " + to_string(getFd()) + ": POST Data size: " + to_string(dataSize));

    std::ofstream   ofs;
    bool            isFileExists = Utils::isPathExist(dataPath);


    showDebugMessage("WebSession " + to_string(getFd()) + ": IsFileExists: " + to_string(isFileExists));
    showDebugMessage("WebSession " + to_string(getFd()) + ": IsWritingPostDataOnDisk: " + to_string(_isWritingPostDataOnDisk));

    if (!isFileExists ) {

        showDebugMessage("WebSession " + to_string(getFd()) + ": File does not exist, creating new file...");
        ofs.open(dataPath.c_str(), fstream::binary);
        _isWritingPostDataOnDisk = true;
    } else {

        if (!_isWritingPostDataOnDisk) {
            bool   isAccessGranted = access(dataPath.c_str(), W_OK) == 0;
            if (!isAccessGranted) {
                showDebugMessage("WebSession " + to_string(getFd()) + ": File access denied. 403 FORBIDDEN");
                request->setResponseCode(global::response_status::FORBIDDEN);
                return false;
                // 403 FORBIDDEN
            }
        }
        showDebugMessage("WebSession " + to_string(getFd()) + ": Appending data to existed file");
        ofs.open(dataPath.c_str(), fstream::binary | fstream::app);
    }

//    showDebugMessage("WebSession " + to_string(getFd()) + ": Data begin: " + dataBegin + "\n####################################################################");
//    showDebugMessage("WebSession " + to_string(getFd()) + ": Data size: " + to_string(dataSize));

    if (ofs.is_open()) {
        ofs.write(dataBegin, dataSize);
        if (ofs.fail()) {
            std::cerr << "WebSession " << getFd() << ": Error: Can't write data from Client " << getFd() << ": " << strerror(errno) << std::endl;
            request->setResponseCode(global::response_status::INTERNAL_ERROR);
            return false;
            // 500 INTERNAL ERROR
        }
    } else {
        std::cerr << "WebSession " << getFd() << ": Error: Can't open file to save data from Client " << getFd() << ": " << strerror(errno) << std::endl;
        request->setResponseCode(global::response_status::INTERNAL_ERROR);
        return false;
        // 500 INTERNAL ERROR
    }
    request->increaseHandledDataSize(dataSize);

    showDebugMessage("WebSession " + to_string(getFd()) + ": Data has been successfully written");

    return true;
}

void WebSession::decodeChunkedTransfer(const char* begin, const char* end, Request *request) { // todo: если данные обрезаются на размере чанка?

    string           rawData(begin, end);
    vector<string>   rows = Utils::split(rawData, "\r\n");
    int              chunkSize;

    chunkSize = stoi(rows[0]);  // TODO: exception handler
    for (int i = 1; chunkSize > 0 && i + 1 < rows.size(); ++i) {
        request->appendData(rows[i].begin().base(), rows[i].end().base());
        chunkSize = stoi(rows[i + 1]);
        if (chunkSize == 0) {
            request->setFullReceived(true);
            break;
        }
    }
}

Response* WebSession::handleRequestByCgi(Request *request, const string& cgiPath) {
    pid_t   pid;
    char*   args[] = {
            (char*)cgiPath.c_str(),
            0
    };
    int    fd_1[2];
    int    fd_2[2];
    int    exitStatus;

    std::cout << args[0] << std::endl;

    pipe(fd_1);
    pipe(fd_2);
    pid = fork();
    if (!pid) {
        setenv("REQUEST_METHOD", request->getMethod().c_str(), 1);
        setenv("SERVER_PROTOCOL", request->getProtocol().c_str(), 1);
        setenv("PATH_INFO", cgiPath.c_str(), 1);

        dup2(fd_1[0], 0);
        dup2(fd_2[1], 1);

        write(fd_1[1], request->getData(), request->getDataSize());

        close(fd_1[1]);
        execv(args[0], args);
        exit(errno);
    }
    close(fd_1[1]);
    close(fd_2[1]);

    waitpid(pid, &exitStatus, WUNTRACED);
    if (exitStatus != 0) {
        std::cerr << "WebSession " << getFd() << ": CGI exit with error! Status: " << exitStatus << std::endl;
        return Response::createResponse(this, nullptr, _master);
    }

    showDebugMessage("WebSession " + to_string(getFd()) + ": CGI correctly. Status: " + to_string(exitStatus));

    const size_t    BUF_SIZE = 256;
    char            buf[BUF_SIZE];
    size_t          bytesRead;
    string          cgiResponse;

    while ((bytesRead = read(fd_2[0], buf, BUF_SIZE-1)) > 0) {
        buf[bytesRead] = '\0';
        cgiResponse.append(buf, bytesRead);
    }

    const char*   endOfCgiData = HttpMessage::findMessageEnd(cgiResponse.c_str());

    if (endOfCgiData)
        cgiResponse.append("\r\n\r\n");

    showDebugMessage("WebSession "
                                + to_string(getFd())
                                + ": cgiResponse:\n--------------------------------"
                                + cgiResponse + "\n--------------------------------");

    close(fd_1[0]);
    close(fd_1[1]);
    close(fd_2[0]);

    return Response::createResponse(this, request, _master,
                                    cgiResponse.c_str());
}

void WebSession::handleResponse() {

    if (_requestQueue.empty())
        return;

    Response*   response;
    Request*    request = _requestQueue.front();

    _keepAlive = request->isKeepAlive();
    showDebugMessage("WebSession " + to_string(getFd()) + ": Keep alive has been set to " + to_string(_keepAlive));

    const Location*   location = request->getLocation();

    if (request->isHandledByCgi()) {
        showDebugMessage("WebSession " + to_string(getFd()) + ": CGI handling needed");
        response = handleRequestByCgi(request, location->getCgiPath());
    } else {
        showDebugMessage("WebSession " + to_string(getFd()) + ": Not CGI");
        response = Response::createResponse(this, request, _master);

        size_t   bytesHandled = response->getHandledDataSize();
        request->increaseHandledDataSize(bytesHandled);
    }

    if (!response) {
        showDebugMessage("WebSession " + to_string(getFd()) + ": No response");
        return;
    }

    sendResponse(response);

    if (response->isFullProcessed() || request->isHandledByCgi()) {
        delete _requestQueue.front();
        _requestQueue.pop();
    }
    if (_requestQueue.empty()) {
        showDebugMessage("WebSession " + to_string(getFd()) + ": Has been processed");
        _processed = true;
    }

    delete response;

    if ((!_keepAlive && _processed) || request->getResponseCode()) {
        _master->removeSession(this);
        return;
    }
}

void WebSession::sendResponse(Response *response) { // todo: return result of sending

    bool send_ok;

    send_ok = send(getFd(), response->getData(), response->getDataSize(), 0) >= 0;

    {
        showDebugMessage("WebSession " + to_string(getFd()) + ": Response:");
        showDebugMessage("--------------------------------------------------");
        showDebugMessage(string(response->getData(), response->getDataSize()));
        showDebugMessage("--------------------------------------------------");
        showDebugMessage("WebSession " + to_string(getFd()) + ": Response has been sent");
    }

    if (!send_ok)
        std::cerr << "WebSession " << getFd() << ": Send error" << std::endl;
}

void WebSession::handle(bool read, bool write) {

    bool       needsImmediateResponse = false;

    if (!_requestQueue.empty()) {
        auto   request = _requestQueue.front();
        needsImmediateResponse = (request->getResponseCode() != 0);
    }

    if (read && !needsImmediateResponse) {
        _beginTime = chrono::high_resolution_clock::now();
        showDebugMessage("WebSession " + to_string(getFd()) + ": Handling request...");
        handleRequest();
    } else if (write) {
        _beginTime = chrono::high_resolution_clock::now();
        showDebugMessage("WebSession " + to_string(getFd()) + ": Handling response...");
        handleResponse();
    } else if (needsImmediateResponse) {
        showDebugMessage("WebSession " + to_string(getFd()) + ": Can't handle request, removing session...");
        _master->removeSession(this);
    } else
        showDebugMessage("WebSession " + to_string(getFd()) + ": Nothing to do");
}

const chrono::time_point<chrono::steady_clock>& WebSession::getStartTime() const { return _beginTime; }

const PairList &WebSession::getCookiesList() const { return _cookies; }


// 0 [GET header CRLF]       OK
// 1 [GET hea]  [der CRLF]   OK
// 2 [GET]  [header CRLF]    OK

// 3 [POST header CRLF data CRLF]       OK
// 4 [POST header CRLF d]  [ata CRLF]   OK
// 5 [POST hea]  [der CRLF data CRLF]   OK

// 6 [GET header CRLF POST header CRLF data CRLF]         OK
// 7 [GET hea]  [der CRLF POST header CRLF data CRLF]     OK
// 8 [POST header CRLF d] [ata CRLF GET head] [er CRLF]
