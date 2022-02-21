/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FdHandler.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/01 18:44:36 by cshells           #+#    #+#             */
/*   Updated: 2022/02/01 18:44:38 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FdHandler.hpp"

FdHandler::FdHandler(int fd, bool own) : _fd(fd), _own_fd(own) {}

FdHandler::~FdHandler() {
    if (_own_fd)
        close(_fd);
}

FdHandler::FdHandler(const FdHandler &other) {}

FdHandler &FdHandler::operator=(const FdHandler &other) {}

const int &FdHandler::getFd() const { return _fd; }

bool FdHandler::wantBeRead() const { return true; }

bool FdHandler::wantBeWritten() const { return false; }
