#ifndef __VIRTUALSERVER_HPP__
#define __VIRTUALSERVER_HPP__

#include "Config.hpp"
#include "ServerOptions.hpp"
#include "LocationOptions.hpp"

class VirtualServer {

public:

    VirtualServer(const Config &config, int index_in_config);
    ~VirtualServer();

    const ServerOptions*       getLocationOptions(const string& url) const;

private:

    void                      initLocations(const Config &config, int index_in_config);

    ServerOptions            *_mainOptions;
    list<LocationOptions*>   _locationsOptions;
};

#endif //WEBSERV_VIRTUALSERVER_HPP
