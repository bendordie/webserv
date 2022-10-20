#include "VirtualServer.hpp"

VirtualServer::VirtualServer(const Config& config, int serverIndexInConfig) : _defaultLocation(nullptr) {

    initLocations(config, serverIndexInConfig);
    if (!_defaultLocation)
        throw std::exception(); // todo: сделать норм исключение
}

VirtualServer::~VirtualServer() {
//    delete _defaultLocation;
//
//    for (auto it = _locations.begin(); it != _locations.end(); ++it) {
//        _locations.remove(*it);
//        delete *it;
//    }
//    _locations.clear();
}

void VirtualServer::initLocations(const Config& config, int serverIndexInConfig) {

    const set<string> &locations = config.getServerLocations(serverIndexInConfig);

    for (auto locationURL = locations.cbegin(); locationURL != locations.cend(); ++locationURL) {

        auto   newLocation = new Location(config, serverIndexInConfig, *locationURL);

        if (!_defaultLocation && *locationURL == "/") {
            _defaultLocation = newLocation;
        }
        _locations.push_front(newLocation);
    }
}

// /directory

const Location *VirtualServer::getLocation(const string& url) const {

    string   urlCopy = url;

    if (urlCopy.length() > 1 && *urlCopy.rbegin() == '/')
        urlCopy.pop_back();

    size_t   lastSlashPos = urlCopy.length();

    for (; lastSlashPos != string::npos; lastSlashPos = urlCopy.rfind('/', lastSlashPos - 1)) {
        auto location = _locations.begin();
        for (; location != _locations.end(); ++location) {

            const string&   temp = lastSlashPos != 0 ?
                    urlCopy.substr(0, lastSlashPos) : urlCopy.substr(0, lastSlashPos + 1);

            if ((*location)->_url == temp) {
                return *location;
            }
        }
    }

    return _defaultLocation;
}