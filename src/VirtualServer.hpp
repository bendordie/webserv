#ifndef __VIRTUALSERVER_HPP__
#define __VIRTUALSERVER_HPP__

#include "Config.hpp"
#include "Location.hpp"

class VirtualServer {

public:

    VirtualServer(const Config &config, int serverIndexInConfig);
    ~VirtualServer();

    const Location*   getLocation(const string& url) const;

private:

    void              initLocations(const Config &config, int serverIndexInConfig);

    Location*         _defaultLocation;
    list<Location*>   _locations;
};

#endif //WEBSERV_VIRTUALSERVER_HPP
