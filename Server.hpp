/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 17:50:39 by cshells           #+#    #+#             */
/*   Updated: 2021/12/16 17:50:40 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <list>
#include <fcntl.h>
#include <fstream>
#include <pthread.h>

#include "Utils.hpp"
#include "Client.hpp"
#include "ErrPage.hpp"
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#define BUFFSIZE        4096
#define MAX_BODY        100000000 // 100MB
#define UNKNOWN_RQST    0
#define GET_RQST        1
#define POST_RQST       2
#define DELETE_RQST     3
#define END_LEN         4
//#define HTTP_200    "HTTP/1.1 200 OK\n"
#define HTTP_200    200
#define HTTP_404    404
#define IS_DIRECTORY(x) (x == 4 ? 1 : 0)
#define IS_FILE(x) (x == 8 ? 1 : 0)

#define TEXT_HTML   0
#define ERR_PAGE_MAXSIZE   1024
#define MAX_FILESIZE        50000000  // 50MB
#define CHECKER_TIMEOUT_DELAY_USEC  5000
#define CLIENT_TIMEOUT_DELAY_SEC    40
#define DEBUG_DELAY_SEC             1

typedef pthread_mutex_t t_mutex;

typedef struct  s_timeout_data {
    std::list<Client*>  *clients;
    fd_set              *curr_sock;
    t_mutex             *mutex;
}               t_timeout_data;

class Server {

public:

    Server(int port = 80, int backlog = 16);
    ~Server();

    bool            start();
    const int&      getServSocket() const;


private:

    int                         setupServer();
    int                         acceptNewConnection();
    void                        initContentTypes();
    void                        initIndexPages();
    void                        initTimeoutThread(pthread_t **timeout_thread, t_timeout_data *data, fd_set *curr_sock);
    void                        handleRequest(int clientSocket, fd_set &curr_sock);
    void                        sendResponse(int client_sock, fd_set &curr_sock, fd_set &write_sock);
    bool                        addNewClient(int client_sock);
    bool                        isKeepAlive(const std::vector<std::string> &data_lines);
    bool                        receiveDataFromClient(char *buffer, int clientSocket, size_t &bytesRead);
    void                        removeClient(int client_sock, fd_set &curr_sock);
    std::string                 extractRequestPath(const char *buffer);
    int                         countEmptyHeaderLines(const char *buffer, unsigned request_type);
    unsigned                    defineRequestType(const char *buffer);
//    bool                        ifHeaderFullyReceived();
    void                        defineHeaderEdge(char *buffer, const size_t &bytesRead, char **headerEnd, size_t &headerSize);
    void                        addClientsToWriteSet(fd_set &write_sock);
    int                         countReadySockets(fd_set *read_sock, fd_set *write_sock);
    void                        error(std::string err_str);
    Client*                     getClient(int client_sock);
    static void*                checkClientsTimeout(void *data);
    void                        makeResponseHeader(const std::string &status, const std::string &content_type, const std::string &file_time);
    void                        makeGetResponse(Client *client);
    void                        makePostResponse(Client *client);
    void                        makeDeleteResponse(Client *client);
    bool                        handlePostData(Client *client, const char *begin, const char *end);
    bool                        handlePostHeader(Client *client);
    std::string                 makeAutoindexPage(std::string path);
    std::string                 makeAutoindexLine(std::list<struct dirent>::iterator file, const std::string &path);
    std::string                 defineContentType(const std::string &file_path) const;



    int                                     _socket;
    int                                     _max_fd;
    int                                     _port;
    int                                     _backlog;
    char*                                   _response_data;
    int                                     _response_data_size;
    int                                     _response_full_size;
    bool                                    _autoindex;
    uint8_t*                                _log_data;
    std::list<std::string>                  _index;
    std::string                             _response_header;
    std::string                             _name;
    std::list<Client*>                      _clients;
    std::map<std::string, std::string>      _content_type;
    long                                    _max_body_size;

};


#endif
