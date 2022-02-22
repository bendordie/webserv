/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_new.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 21:22:29 by cshells           #+#    #+#             */
/*   Updated: 2022/02/02 21:22:31 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include "WebServer.hpp"

using namespace std;

int main() {

    EventSelector   *event_selector = new EventSelector;
    std::cout << "main: EventSelector has been created" << std::endl;
    WebServer       *server =   WebServer::start(event_selector, 80);
    std::cout << "main: WebServer has been created" << std::endl;

    if (!server) {
        perror("Server: ");
        return 1;
    }
    std::cout << "WebServer: Start listening" << std::endl;
    event_selector->run();

    return 0;
}

