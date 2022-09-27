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

FdHandler::FdHandler(int fd, bool own) : _fd(fd), _ownFd(own) {}

FdHandler::~FdHandler() {
    if (_ownFd)
        close(_fd);
}

int FdHandler::getFd() const { return _fd; }

bool FdHandler::wantBeRead() const { return true; }

bool FdHandler::wantBeWritten() const { return false; }
