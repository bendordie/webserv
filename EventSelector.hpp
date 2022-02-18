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

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "FdHandler.hpp"

class EventSelector {


public:

    EventSelector();
    ~EventSelector();
    EventSelector(const EventSelector &other);
    EventSelector& operator=(const EventSelector &other);

    void        add(FdHandler *handler);
    bool        remove(FdHandler *handler);
    void        breakLoop();
    void        run();



private:

    FdHandler   **_fd_array;
    int         _fd_array_len;
    int         _max_fd;
    bool        _quit_flag;




};

#endif //WEBSERV_EVENTSELECTOR_HPP
