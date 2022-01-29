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

namespace Utils {

    typedef std::map<std::string, std::string>::const_iterator t_find_key;

    std::vector<std::string> split(std::string str, char ch);
    std::string              getTime();
    std::string              getExtension(const std::string& file_name);
    bool                     searchFileInDir(const char* dir_path, const char* file_name);
    bool                     isPathAccessed(const std::string &path);
    bool                     getDirContent(const char* dir_path, std::list<struct dirent> &content);
    std::string              getFileLastModTime(const std::string &file_path);
    bool                     isPathExist(const std::string &path);

    t_find_key
    findKey(std::map<std::string, std::string>::const_iterator begin, std::map<std::string,
            std::string>::const_iterator end, std::string value);

//    template<class T1, class T2>
//    const typename std::map<T1, T2>::const_iterator
//    findKey(typename std::map<T1, T2>::const_iterator begin, typename std::map<T1, T2>::const_iterator end,  const T2 &value, T1);

    //    static unsigned int             getStrFileSize(const std::string& file_path);
    //    static bool                     searchDirInDir(const char* dir_path, const char* directory);

};


#endif