/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrPage.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/14 19:24:18 by cshells           #+#    #+#             */
/*   Updated: 2022/01/14 19:24:19 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrPage.hpp"

ErrPage::ErrPage() {}

ErrPage::ErrPage(int code, std::string body) : _code(code), _body(body) {}

ErrPage::~ErrPage() {}

ErrPage::ErrPage(const ErrPage &other) : _code(other.getPageCode()), _body(other.getPageBody()) {}

ErrPage &ErrPage::operator=(const ErrPage &other) {
    if (this != &other) {
        _code = other.getPageCode();
        _body = other.getPageBody();
    }
    return *this;
}
