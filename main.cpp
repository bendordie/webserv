//
// Created by Carmel Shells on 12/16/21.
//

#include "Server.hpp"

int Client::count = 0;

int main() {
    Server  server;

    server.start();
    return 0;
}