/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 19:40:20 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 19:40:21 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResposnse::HttpResposnse() {}

HttpResposnse::HttpResposnse(unsigned int code, const std::vector<std::string> &headerLines)
: HttpMessage(code, headerLines) {}

HttpResposnse::~HttpResposnse() {}

