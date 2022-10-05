/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventSelector.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/01 19:19:56 by cshells           #+#    #+#             */
/*   Updated: 2022/02/01 19:19:57 by cshells          ###   ########.fr       */
/*                                                                            */
/* *****************************************]********************************* */

#include "EventSelector.hpp"

EventSelector::EventSelector() : _fd_array(0), _quit_flag(false) {}

EventSelector::~EventSelector() {

    if (_fd_array)
        delete [] _fd_array;
}

void EventSelector::add(FdHandler *handler) {

    int   i;
    int   fd = handler->getFd();

    showDebugMessage("EventSelector: Prepare to add FD ");
    if (!_fd_array) {
        showDebugMessage("EventSelector: Creating new FD storage...");
        _fd_array_len = fd > 15 ? fd + 1 : 16;
        _fd_array = new FdHandler*[_fd_array_len];
        showDebugMessage("EventSelector: FD storage has been created. Storage size: " + std::to_string(_fd_array_len));
        for (i = 0; i < _fd_array_len; i++) {
            _fd_array[i] = 0;
        }
        _max_fd = -1;
    }
    if (_fd_array_len <= fd) {
        showDebugMessage("EventSelector: New FD (" + std::to_string(fd) + ") is to large (storage size: " +
                                 std::to_string(_fd_array_len) + "). Preparing new storage...");
        FdHandler**   tmp = new FdHandler*[fd + 1];
        for (i = 0; i <= fd; i++)
            tmp[i] = i < _fd_array_len ? _fd_array[i] : 0;
        _fd_array_len = fd + 1;
        delete [] _fd_array;
        _fd_array = tmp;
    }
    if (fd > _max_fd) {
        _max_fd = fd;
        showDebugMessage("EventSelector: Max FD has been redefined to ");
    }
    _fd_array[fd] = handler;
    showDebugMessage("EventSelector: New FD handler has been added to storage");
}

bool EventSelector::remove(FdHandler *handler) {

    int   fd = handler->getFd();

    if (fd >= _fd_array_len || _fd_array[fd] != handler)
        return false;
    _fd_array[fd] = 0;
    if (fd == _max_fd) {
        for (; _max_fd >= 0 && !_fd_array[_max_fd]; --_max_fd) {}
    }

    return true;
}

void EventSelector::breakLoop() { _quit_flag = true; }

void EventSelector::run() {

    showDebugMessage("EventSelector: Running...");

    _quit_flag = false;
    for (; !_quit_flag; ) {

        waitForDebug();
        showDebugMessage("***************************************************************************");
        showDebugMessage("EventSelector: Max FD: " + std::to_string(_max_fd));

        int      i;
        fd_set   read_set, write_set;

        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        _timeoutMutex.lock();
        for (i = 0; i <= _max_fd; ++i) {
            if (_fd_array[i]) {
                if (_fd_array[i]->wantBeRead()) {
                    showDebugMessage("EventSelector: FD " + std::to_string(i) + " marked as wanting to be read");
                    FD_SET(i, &read_set);
                }
                if (_fd_array[i]->wantBeWritten()) {
                    showDebugMessage("EventSelector: FD " + std::to_string(i) + " marked as wanting to be written");
                    FD_SET(i, &write_set);
                }
            }
        }

        waitForDebug();

        showDebugMessage("EventSelector: Selecting...");
        struct timeval timeout;
        bzero(&timeout, sizeof(timeout));
        timeout.tv_sec = 10;
        int result = select(_max_fd + 1, &read_set, &write_set, 0, &timeout);
        _timeoutMutex.unlock();
        if (result < 0) {
            if (errno == EINTR)
                continue;
            else
                break;
        }

        waitForDebug();

        if (result > 0) {
            showDebugMessage("EventSelector: Selecting is done");
            for (i = 0; i <= _max_fd; ++i) {
                _timeoutMutex.lock();
                if (!_fd_array[i]) {
                    _timeoutMutex.unlock();
                    continue;
                }
                bool   read = FD_ISSET(i, &read_set);
                bool   write = FD_ISSET(i, &write_set);
                if (read || write) {
                    showDebugMessage("EventSelector: FD " + std::to_string(i) + " has been selected for read: " +
                                             std::to_string(read) + " write: " + std::to_string(write));
                    _fd_array[i]->handle(read, write);
                }
                _timeoutMutex.unlock();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds (20));
    }
}

std::mutex &EventSelector::getTimeoutMutex() { return _timeoutMutex; }
