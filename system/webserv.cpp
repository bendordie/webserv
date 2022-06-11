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

#include <thread>
#include <sstream>
#include "WebServer.hpp"
#include "Config.hpp"
#include "Utils.hpp"

using namespace std;

template <class Container>
WebServer* findServerByPort(int port, const Container &servers) {

    auto isPortUsed = [port](WebServer* server){ return port == server->getPort(); };
    auto result = std::find_if(servers.begin(), servers.end(), isPortUsed);

    return result != servers.end() ? *result : nullptr;
}

bool    initServers(const Config &config, EventSelector *event_selector) {

    list<WebServer*> servers;
    string           listenField;
    bool             listenFieldExist;
    vector<string>   ports;
    int              port;
    WebServer        *newServer;

    for (int idx = 0; idx < 1 /*config.size()*/; ++idx) { // TODO: hardcode
        std::cout << "Server idx " << idx << std::endl;
        listenFieldExist = config.isTraitDefined(idx, "listen");
        if (!listenFieldExist) {
            cerr << "Error: Listen field has been missed for Server idx " << idx << endl;
            return false;
        }
        listenField = config[to_string(idx) + ".server.listen"];
        if (listenField.empty()) {
            cerr << "Error: Listen field is empty for Server idx " << idx << endl;
            return false;
        }
        ports = Utils::split(listenField, ',');
        for (int it = 0; it < ports.size(); ++it) {
            try {
                port = stoi(ports[it]);
            } catch (std::invalid_argument&) {
                cerr << "Error: Port is invalid for Server " << idx << endl;
                return false;
            }

            WebServer *existingServer = findServerByPort(port, servers);
            if (!existingServer) {
                newServer = WebServer::init(event_selector, port, config, idx);
                if (!newServer) {
                    perror("Server: ");
                    return false;
                }
                servers.push_back(newServer);
                cout << "main: WebServer id " << idx << ", port " << port << " has been created" << endl;
            } else {
                bool server_added = existingServer->addNewVirtualServer(config, idx);
                if (!server_added)
                    return false;
            }

            std::cout << "*****************************************************************************" << std::endl;
        }
    }
    return true;
}

int main() {

    Config           config("./config_sample");
    cout << "main: Configuration file has been loaded" << endl;

//    cout << config.isTraitDefined(1, "location /ttt.return") << endl;
//
//    std::cout << config["1.server.location /ttt.return"].empty() << std::endl;

    EventSelector    *eventSelector = new EventSelector;
    cout << "main: EventSelector has been created" << endl;
    bool serverInitFailed = !initServers(config, eventSelector);
    if (serverInitFailed)
        return 1;
    cout << "WebServer: begin listening" << endl;
    eventSelector->run();

    return 0;
}

