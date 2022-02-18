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
/* ************************************************************************** */

#include "EventSelector.hpp"

EventSelector::EventSelector() : _fd_array(0), _quit_flag(false) {}

EventSelector::~EventSelector() {

    if (_fd_array)
        delete [] _fd_array;
}

EventSelector::EventSelector(const EventSelector &other) {}

EventSelector &EventSelector::operator=(const EventSelector &other) {}

void EventSelector::add(FdHandler *handler) {

    int i;
    int fd = handler->getFd();

    if (!_fd_array) {
        _fd_array_len = fd > 15 ? fd + 1 : 16;
        _fd_array = new FdHandler*[_fd_array_len];
        for (i = 0; i < _fd_array_len; i++) {
            _fd_array = 0;
        }
        _max_fd = -1;
    }
    if (_fd_array_len <= fd) {
        FdHandler   **tmp = new FdHandler*[fd + 1];
        for (i = 0; i <= fd; i++)
            tmp[i] = i < _fd_array_len ? _fd_array[i] : 0;
        _fd_array_len = fd + 1;
        delete [] _fd_array;
        _fd_array = tmp;
    }
    if (fd > _max_fd)
        _max_fd = fd;
    _fd_array[fd] = handler;
}

bool EventSelector::remove(FdHandler *handler) {

    int fd = handler->getFd();

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

    _quit_flag = false;
    for (; !_quit_flag; ) {
        int i;
        fd_set  read_set, write_set;

        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        for (i = 0; i <= _max_fd; ++i) {
            if (_fd_array[i]) {
                if (_fd_array[i]->wantRead())
                    FD_SET(i, &read_set);
                if (_fd_array[i]->wantWrite())
                    FD_SET(i, &write_set);
            }
        }
        int result = select(_max_fd + 1, &read_set, &write_set, 0, 0);
        if (result < 0) {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        if (result > 0) {
            for (i = 0; i <= _max_fd; ++i) {
                if (!_fd_array[i])
                    continue;
                bool read = FD_ISSET(i, &read_set);
                bool write = FD_ISSET(i, &write_set);
                if (read || write)
                    _fd_array[i]->Handle(read, write);
            }
        }
    }
}
