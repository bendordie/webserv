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
#include "Config.hpp"

using namespace std;

int main() {

    Config          config("./config_sample");
    std::cout << "main: Configuration file has been loaded" << std::endl;
    EventSelector   *event_selector = new EventSelector;
    std::cout << "main: EventSelector has been created" << std::endl;
    WebServer       *server =   WebServer::start(event_selector, 80, config);
    std::cout << "main: WebServer has been created" << std::endl;

    if (!server) {
        perror("Server: ");
        return 1;
    }
    std::cout << "WebServer: Start listening" << std::endl;
    event_selector->run();

    return 0;
}

