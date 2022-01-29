/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrPage.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/14 19:24:12 by cshells           #+#    #+#             */
/*   Updated: 2022/01/14 19:24:15 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __ERRPAGE_HPP__
#define __ERRPAGE_HPP__

#include <map>
#include <string>

class ErrPage {

public:

    ErrPage(int code, std::string body);
    ~ErrPage();
    ErrPage(const ErrPage& other);
    ErrPage& operator=(const ErrPage& other);

    int                 getPageCode() const;
    const std::string&  getPageBody() const;

    void                setPageCode(int code);
    void                setPageBody(int code, std::string body);


private:

    ErrPage();
    int         _code;
    std::string _body;

};


#endif //WEBSERV_ERRPAGE_HPP
