/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 00:12:37 by cshells           #+#    #+#             */
/*   Updated: 2021/12/19 00:12:38 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>
#include <list>
#include <map>
#include <vector>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;
using std::exception;
using std::ios_base;
using std::ifstream;
using std::string;
using std::vector;
using std::list;
using std::map;
using std::ostringstream;

namespace Utils {

    typedef struct  s_file {

        char*    data = nullptr;
        size_t   bytes_read = 0;
        string   type;
        bool     eof;
    }               t_file;

    vector<string>                         split(const string& str, char ch);
    vector<string>                         split(const string& str, const string& val);
    string                                 getTimeInString();
    string                                 getExtension(const string& file_name);
    bool                                   searchFileInDir(const char* dir_path, const char* file_name);
    bool                                   isPathAccessed(const string& path);
    bool                                   getDirContent(const char* dir_path, list<struct dirent>& content);
    string                                 getFileLastModTime(const string& file_path);
    bool                                   isPathExist(const string& path);
    t_file                                 readFile(string filePath, size_t maxBufSize, size_t bytesAlreadyRead = 0);
    bool                                   strToBool(const string& str);
    long long                              strToLongLong(const string &str);
    string                                 intToHexString(int value);
    const char                             *reverse_strstr(const char* source, const char* needle);
    map<string, string>::const_iterator    findKey(map<string, string>::const_iterator begin, map<string,
                                                   string>::const_iterator end, const string& value);

};

void showDebugMessage(const string& message);

#endif