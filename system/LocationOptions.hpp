/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationOptions.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/11 14:48:46 by cshells           #+#    #+#             */
/*   Updated: 2022/03/11 14:48:48 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef __LOCATIONTRAITS_HPP_
#define __LOCATIONTRAITS_HPP_

#include <iostream>
#include <string>
#include "ServerOptions.hpp"

using std::string;

class LocationOptions : public ServerOptions {

//    friend class WebServer;
    friend class VirtualServer;

public:

    LocationOptions(const ServerOptions &serv_traits, const string &url);
    ~LocationOptions();

    const string&   getUrl() const;
    const string&   getReturnValue() const;
    string          getAbsolutePath() const;

    void            setUrl(const string &url);

private:

    string   _url;
    string   _returnValue;

};

#endif //WEBSERV_LOCATIONTRAITS_HPP
