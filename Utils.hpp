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

using namespace std;

namespace Utils {

    typedef struct  s_file {
        char    *data;
        size_t  size;
        string  type;
    }               t_file;

//    typedef map<string, string>::const_iterator t_map_const_iter;

    vector<string>              split(const string &str, char ch);
    vector<string>              split(const string &str, const string &val);
    string                      getTime();
    string                      getExtension(const string& file_name);
    bool                        searchFileInDir(const char* dir_path, const char* file_name);
    bool                        isPathAccessed(const string &path);
    bool                        getDirContent(const char* dir_path, list<struct dirent> &content);
    string                      getFileLastModTime(const string &file_path);
    bool                        isPathExist(const string &path);
    t_file                      readFile(string file_path);
    map<string, string>::const_iterator            findKey(map<string, string>::const_iterator begin, map<string, string>::const_iterator end, const string &value);

//    template<class T1, class T2>
//    const typename std::map<T1, T2>::const_iterator
//    findKey(typename std::map<T1, T2>::const_iterator begin, typename std::map<T1, T2>::const_iterator end,  const T2 &value, T1);

    //    static unsigned int             getStrFileSize(const std::string& file_path);
    //    static bool                     searchDirInDir(const char* dir_path, const char* directory);

};


#endif