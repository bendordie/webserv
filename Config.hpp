#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

class Config {
	vector<map<string, string> > servers;
	Config() {};
public:
	~Config() {};

	explicit Config(const string& path) {
		ifstream file;
		vector<string> properties;
		map<string, string> tmpMap;
		string line;
		string currentProperty;
		string currentKey;
		bool isQuotes = false;
		int idx;

		file.open (path, ifstream::in);

		while (getline(file, line)) {
			idx = 0;
			line = line.substr(0, line.find("#")); // remove comments

			while (line[idx] != '\0') {
				if (line[idx] == ',') {
					currentProperty += line[idx++];
					while (line[idx] == ' ')
						idx++;
					continue;
				}
				if (line[idx] == '"')
					isQuotes = !isQuotes;
				else if ((line[idx] == ' ' || line[idx] == '\t') && !isQuotes) {
					if (!currentProperty.empty()) {

						properties.push_back(currentProperty);
						currentProperty.erase();
					}
					idx++;
					continue;
				} else if (line[idx] == ';' && currentProperty.empty()) {
					properties[properties.size() - 1] += ";";
					idx++;
					continue;
				}

				currentProperty += line[idx];
				idx++;
			}
			if (!currentProperty.empty()) {
				properties.push_back(currentProperty);
				currentProperty.erase();
			}
		}



		for (vector<string>::iterator it = properties.begin(); it != properties.end(); it++) {
			if (*it == "return" && (it + 1)->find(";") == string::npos) {
				tmpMap[currentKey + "." + *it] = *(it + 1) + " " + (it + 2)->substr(0, (it + 2)->size() - 1);
				it++;
			}
			else if (*it == "}") {
				if (currentKey.rfind(".") != string::npos) {
					currentKey = currentKey.substr(0, currentKey.rfind("."));
				}
				else {
					servers.push_back(tmpMap);
					tmpMap.clear();
					currentKey.erase();
				}
			} else if ((it + 1)->rfind(";") != string::npos) {
				tmpMap[currentKey + "." + *it] = (it + 1)->substr(0, (it + 1)->size() - 1);
				it++;
			}
			else if (*(it + 1) == "{") {
				if (!currentKey.empty())
					currentKey += ".";
				currentKey += *it;
				it++;
			}
			else if (*(it + 2) == "{") {
				if (!currentKey.empty())
					currentKey += ".";
				currentKey += *it + " " + *(it + 1);
				it += 2;
			}

		}
	};

    set<string> getServerLocations(int idx) const {
        set<string> tmp;

        for (map<string, string>::const_iterator it = servers[idx].begin(); it != servers[idx].end(); it++) {

            if (it->first.find("server.location /") == 0) {
                int start_pos = it->first.find("/");
                tmp.insert(it->first.substr(start_pos, it->first.find(".", start_pos) - start_pos));
            }
        }

        return tmp;
    }

    bool isTraitDefined(int idx, const string& trait_name) const {

        for (map<string, string>::const_iterator it = servers[idx].begin(); it != servers[idx].end(); it++) {
            int find_idx = it->first.find(trait_name);
            if (find_idx == strlen("server.")) {
                return true;
            }
        }

        return false;
    }

//    bool hasLocationTrait(int idx, const string& location, const string& trait_name) const {
//
//        for (map<string, string>::const_iterator it = servers[idx].begin(); it != servers[idx].end(); it++) {
//
//            int find_idx = it->first.find(location + "." + trait_name);
//            if (find_idx == strlen("server.location") + location.length()) {
//                return true;
//            }
//        }
//
//        return false;
//
//    }

	size_t size() {
		return servers.size();
	}

	string operator [](string const& name) {

		int idx = atoi(name.c_str());
		return servers[idx].find(name.substr(name.find('.') + 1))->second;
	}

	string const operator [](string const& name) const {
		int idx = atoi(name.c_str());
		return servers[idx].find(name.substr(name.find('.') + 1))->second;
	}


private:
	string to_lower(string &s) {
		for (int i = 0; s[i] != '\0'; ++i)
			s[i] = tolower(s[i]);
		return s;
	}
};

#endif