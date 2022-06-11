/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationOptions.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/11 14:48:51 by cshells           #+#    #+#             */
/*   Updated: 2022/03/11 14:48:52 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationOptions.hpp"

LocationOptions::LocationOptions(const ServerOptions &serv_traits, const std::string &url)
    : ServerOptions(serv_traits), _url(url) {

    initSingleTrait(_root, "location " + _url + ".root");
    initSingleTrait(_returnValue, "location " + _url + ".return");
    initSingleTrait(_client_body_size, "location " + _url + ".client_body_size", Utils::strToLongLong);
    initSingleTrait(_autoindex, "location " + _url + ".autoindex", Utils::strToBool);
    initMultipleTrait(_index_list, "location " + _url + ".index");
    initMultipleTrait(_methods_list, "location " + _url + ".methods");
};

LocationOptions::~LocationOptions() {}

const std::string &LocationOptions::getUrl() const { return _url; }

const string &LocationOptions::getReturnValue() const { return _returnValue; }

std::string LocationOptions::getAbsolutePath() const { return getRoot() + _url; }

void LocationOptions::setUrl(const std::string &url) { _url = url; }
