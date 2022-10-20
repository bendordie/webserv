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
#include "global_variables.hpp"

using std::cout;
using std::cerr;
using std::endl;

void showDebugMessage(const string& message) {
#ifdef DEBUG
    std::cout << message << std::endl;
#else
    (void)message;
#endif
}

template <class Container>
WebServer* findServerByPort(int port, const Container& servers) {

    auto   isPortUsed = [port](WebServer* server){ return port == server->getPort(); };
    auto   result = std::find_if(servers.begin(), servers.end(), isPortUsed);

    return result != servers.end() ? *result : nullptr;
}

extern bool initServers(const Config &config, EventSelector *eventSelector) {

    list<WebServer*>   servers;
    string             listenField;
    bool               listenFieldExist;
    vector<string>     ports;
    int                port;
    WebServer*         newServer;

    for (size_t idx = 0; idx < /*1*/ config.size(); ++idx) { // TODO: hardcode
        showDebugMessage("Server idx " + to_string(idx));
        listenFieldExist = config.isOptionDefined(idx, "listen");
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
        for (auto & it : ports) {
            try {
                port = stoi(it);
            } catch (std::invalid_argument&) {
                cerr << "Error: Port is invalid for Server " << idx << endl;
                return false;
            }

            WebServer*   existingServer = findServerByPort(port, servers);
            if (!existingServer) {
                newServer = WebServer::init(eventSelector, port, config, idx);
                if (!newServer) {
                    perror("Server: ");
                    return false;
                }
                servers.push_back(newServer);
                showDebugMessage("main: WebServer id " + to_string(idx) + ", port " +
                                                                    to_string(port) + " has been created");
            } else {
                bool   serverAdded = existingServer->addNewVirtualServer(config, idx);
                if (!serverAdded)
                    return false;
            }
            showDebugMessage("*****************************************************************************");
        }
    }
    return true;
}

int main() {

    const Config*   config;

    try {
        config = new Config("./cfg/config_sample");
    } catch (std::exception) {
        cerr << "Error: Configuration file path is invalid" << endl;
        return 1;
    }

    showDebugMessage("main: Configuration file has been loaded");

    auto   eventSelector = new EventSelector();

    showDebugMessage("main: EventSelector has been created");

    bool   serverInitFailed = !initServers(*config, eventSelector);
    if (serverInitFailed)
        return 1;

    showDebugMessage("main: Begin listening");
    eventSelector->run();

    delete eventSelector;
    delete config;

    return 0;
}