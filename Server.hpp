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

#define BUFFSIZE    4096
#define MAX_BODY    100000000 // 100MB
#define GET_RQST    1
#define POST_RQST   2
#define DELETE_RQST 3
#define END_LEN     4
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

typedef struct  s_timeout_check {
    std::list<Client*>  *clients;
    fd_set              *curr_sock;
    t_mutex             *mutex;
}               t_timeout_check ;

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
//    void                        initErrPages();
    void                        handleRequest(int client_sock, fd_set &curr_sock);
    void                        sendResponse(int client_sock, fd_set &curr_sock, fd_set &write_sock);
    bool                        addNewClient(int client_sock);
    bool                        isKeepAlive(const std::vector<std::string> &data_lines);
    void                        removeClient(int client_sock, fd_set &curr_sock);
    void                        removeTimeoutClients(fd_set &curr_sock);
    void                        removeTimeoutClients2(fd_set &curr_sock);
    void                        addClientsToFdSet(fd_set &set);
    void                        error(std::string err_str);
    bool                        readFile(std::string file_path);
    Client*                     getClient(int client_sock);
    void                        makeResponse(const std::vector<std::string> &header);
    static void*                checkClientsTimeout(void *data);
//    void                        loadErrPage(int code);
//    std::string                 makeErrorPage(int code) const;
    void                        makeResponseHeader(const std::string &status, const std::string &content_type, const std::string &file_time);
    void                        makeGetResponse(Client *client);
    void                        makePostResponse(Client *client);
    void                        makeDeleteResponse(Client *client);
    void                        makeBadResponse();
    bool                        handlePostData(Client *client, const char *begin, const char *end);
    bool                        handlePostHeader(Client *client);
    bool                        handleDelete(Client *client);
    std::string                 makeAutoindexPage(std::string path);
    std::string                 makeAutoindexLine(std::list<struct dirent>::iterator file, const std::string &path);
    const std::string&          getIndex();
    std::vector<uint8_t>        ifPost(std::vector<std::string> msg_lines) const;
    std::vector<uint8_t>        ifDelete(std::vector<std::string> msg_lines) const;
    std::string                 badRequest(int code) const;
    std::string                 defineContentType(const std::string &file_path) const;

    int                                     _socket;
    int                                     _max_fd;
    int                                     _port;
    int                                     _backlog;
    std::string                             _autoindex_path;
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
    std::map<int, std::string>              _err_page;
    int                                     _max_body_size;

};


#endif
