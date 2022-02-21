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

    std::cout << "EventSelector: Prepare to add FD " << fd << std::endl;
    if (!_fd_array) {
        std::cout << "EventSelector: Creating new FD storage..." << std::endl;
        _fd_array_len = fd > 15 ? fd + 1 : 16;
        _fd_array = new FdHandler*[_fd_array_len];
        std::cout << "EventSelector: FD storage has been created. Storage size: " << _fd_array_len << std::endl;
        for (i = 0; i < _fd_array_len; i++) {
            _fd_array[i] = 0;
        }
        _max_fd = -1;
    }
    if (_fd_array_len <= fd) {
        std::cout << "EventSelector: New FD (" << fd << ") is to large (storage size: " << _fd_array_len << "). Preparing new storage..." << std::endl;
        FdHandler   **tmp = new FdHandler*[fd + 1];
        for (i = 0; i <= fd; i++)
            tmp[i] = i < _fd_array_len ? _fd_array[i] : 0;
        _fd_array_len = fd + 1;
        delete [] _fd_array;
        _fd_array = tmp;
    }
    if (fd > _max_fd) {
        _max_fd = fd;
        std::cout << "EventSelector: Max FD has been redefined to "  << fd<< std::endl;
    }
    _fd_array[fd] = handler;
    std::cout << "EventSelector: New FD handler has been added to storage" << std::endl;
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

    std::cout << "EventSelector: Running..." << std::endl;
    _quit_flag = false;
    for (; !_quit_flag; ) {
        sleep(DEBUG_DELAY_SEC);
        std::cout << "***************************************************************************" << std::endl;
        std::cout << "EventSelector: Max FD: " << _max_fd << std::endl;
        int     i;
        fd_set  read_set, write_set;

        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        for (i = 0; i <= _max_fd; ++i) {
            if (_fd_array[i]) {
                if (_fd_array[i]->wantBeRead()) {
                    std::cout << "EventSelector: FD " << i << " marked as wanting to be read" << std::endl;
                    FD_SET(i, &read_set);
                }
                if (_fd_array[i]->wantBeWritten()) {
                    std::cout << "EventSelector: FD " << i << " marked as wanting to be written" << std::endl;
                    FD_SET(i, &write_set);
                }
            }
        }
        sleep(DEBUG_DELAY_SEC);
        std::cout << "EventSelector: Selecting..." << std::endl;
        int result = select(_max_fd + 1, &read_set, &write_set, 0, 0);
        if (result < 0) {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        sleep(DEBUG_DELAY_SEC);
        if (result > 0) {
            std::cout << "EventSelector: Selecting is done" << std::endl;
            for (i = 0; i <= _max_fd; ++i) {
                if (!_fd_array[i])
                    continue;
                bool read = FD_ISSET(i, &read_set);
                bool write = FD_ISSET(i, &write_set);
                if (read || write) {
                    std::cout << "EventSelector: FD " << i << " has been selected for read: " << read << " write: " << write << std::endl;
                    _fd_array[i]->Handle(read, write);
                }
            }
        }
    }
}
