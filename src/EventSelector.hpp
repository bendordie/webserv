/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/01 19:19:51 by cshells           #+#    #+#             */
/*   Updated: 2022/02/01 19:19:52 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __EVENTSELECTOR_HPP__
#define __EVENTSELECTOR_HPP__

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <mutex>

#include "FdHandler.hpp"
#include "global_variables.hpp"


class EventSelector {


public:

    EventSelector();
    ~EventSelector();

    void          add(FdHandler *handler);
    bool          remove(FdHandler *handler);
    void          breakLoop();
    void          run();
    std::mutex&   getTimeoutMutex();



private:

    FdHandler**   _fd_array;
    int           _fd_array_len;
    int           _max_fd;
    bool          _quit_flag;
    std::mutex    _timeoutMutex;

};

void showDebugMessage(const string& message);

#endif //WEBSERV_EVENTSELECTOR_HPP
