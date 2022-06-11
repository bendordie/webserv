#include "VirtualServer.hpp"

VirtualServer::VirtualServer(const Config &config, int index_in_config) {

    _mainOptions = new ServerOptions(config, index_in_config);
    initLocations(config, index_in_config);
}

VirtualServer::~VirtualServer() {}

void VirtualServer::initLocations(const Config &config, int index_in_config) {

    const set<string> &locations = config.getServerLocations(index_in_config);

    for (auto location_url = locations.cbegin(); location_url != locations.cend(); ++location_url) {
        auto *new_location = new LocationOptions(*_mainOptions, *location_url);
        _locationsOptions.push_front(new_location);
    }
}

const ServerOptions *VirtualServer::getLocationOptions(const string &url) const {

    size_t   last_slash_pos = url.length() - 1;

    for (; last_slash_pos != string::npos; last_slash_pos = url.rfind('/', last_slash_pos - 1)) {
        auto it = _locationsOptions.begin();
        for (; it != _locationsOptions.end(); ++it) {
            if ((*it)->_url == url.substr(0, last_slash_pos + 1)) {
                return *it;
            }
        }
    }

    return _mainOptions;
}