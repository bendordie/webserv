//
// Created by Carmel Shells on 12/19/21.
//

#include "Utils.hpp"

std::vector<std::string> Utils::split(std::string str, char ch) {

    std::vector<std::string> result;

    if (str.length() > 0 && str.find(ch) == std::string::npos) {
        result.push_back(str);
        return (result);
    }
    for (size_t i = str.find_first_not_of(ch, 0), j = str.find(ch, i); j != std::string::npos;) {
        result.push_back(str.substr(i,j - i));
        for (; str[j] == ch; ++j) {}
        i = j;
        j = str.find(ch, i);
        if (i != str.length() && j == std::string::npos) {
            result.push_back(str.substr(i, str.length()));
        }
    }
    return result;
}

std::string Utils::getTime() {
    struct timeval  tv;
    time_t          nowtime;
    struct tm       *nowtm;
    char            tmbuf[64];

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%a, %d %b %Y %H:%M:%S GMT", nowtm);

    return tmbuf;
}

std::string Utils::getExtension(const std::string &file_name) {
    size_t pos = file_name.rfind('.');

    if (pos <= 0) return "";
    return file_name.substr(pos+1, std::string::npos);
}

bool Utils::searchFileInDir(const char* dir_path, const char* file_name) {
    size_t			len;
    struct dirent	*dp;
    DIR				*dirp;

    dirp = opendir(dir_path);
    if (dirp == NULL) {
        std::cout << "Can't open directory" << std::endl;
        return false;
    }
    len = strlen(file_name);
    dp = readdir(dirp);
    while (dp)
    {
        if (dp->d_namlen == len && strcmp(dp->d_name, file_name) == 0)
        {
            (void)closedir(dirp);
            return true;
        }
        dp = readdir(dirp);
    }
    (void)closedir(dirp);
    return false;
}

//bool Utils::searchDirInDir(const char *dir_path, const char *directory) {
//    size_t			len;
//    struct dirent	*dp;
//    DIR				*dirp;
//
//    dirp = opendir(dir_path);
//    if (dirp == NULL) {
//        std::cout << "Can't open directory" << std::endl;
//        return false;
//    }
//    len = strlen(file_name);
//    dp = readdir(dirp);
//    while (dp)
//    {
//        if (dp->d_namlen == len && strcmp(dp->d_name, file_name) == 0)
//        {
//            (void)closedir(dirp);
//            return true;
//        }
//        dp = readdir(dirp);
//    }
//    (void)closedir(dirp);
//    return false;
//}

bool Utils::isPathExist(const std::string &path) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
}

bool Utils::isPathAccessed(const std::string &path) {
    struct stat buffer;

    stat(path.c_str(), &buffer);
    return (EACCES);
}


bool Utils::getDirContent(const char* dir_path, std::list<struct dirent> &content) {
    size_t			len;
    struct dirent	*dp;
    DIR				*dirp;

    dirp = opendir(dir_path);
    if (dirp == NULL) {
        std::cout << "Can't open directory" << std::endl;
        return false;
    }
    dp = readdir(dirp);
//    int i = 0;
    while (dp)
    {
//        std::cout << i << std::endl;
        content.push_back(*dp);
        dp = readdir(dirp);
//        i++;
    }
    (void)closedir(dirp);
    return true;
}

std::string Utils::getFileLastModTime(const std::string& file_path) {
    struct stat     result;
    time_t          mod_time;
    struct tm       *nowtm;
    char            tmbuf[64];

    if(stat(file_path.c_str(), &result) == 0)
    {
        mod_time = result.st_mtime;
        nowtm = localtime(&mod_time);
        strftime(tmbuf, sizeof tmbuf, "%a, %d %b %Y %H:%M:%S GMT", nowtm);
    } else {
        return getTime();
    }
    return tmbuf;
}

std::pair<char*, size_t> Utils::readFile(std::string file_path) {
    std::ifstream   stream;
    char            *buff;

    stream.open(file_path, std::ifstream::binary);
    if (stream.is_open()) {
        try {
            struct stat fi;

            bzero(&fi, sizeof(fi));
            stat(file_path.c_str(), &fi);
            std::cout << "STAT FILE SIZE: " << fi.st_size << std::endl;
            buff = new char[fi.st_size];
            *buff = 0;
            stream.read(buff, fi.st_size);
            stream.close();
        }
        catch (std::exception &ex) {
            std::cout << ex.what() << std::endl;
        }
    } else {
        std::cout << "Can't open file" << std::endl;
        return std::make_pair((char*)NULL, 0);
    }
    return std::make_pair(buff, stream.gcount());;
}

Utils::t_find_key Utils::findKey(std::map<std::string, std::string>::const_iterator begin,
                          std::map<std::string, std::string>::const_iterator end, std::string value) {
    for (; begin != end; ++begin) {
        if (!strncmp(begin->second.c_str(), value.c_str(), begin->second.length()))
            return begin;
    }
    return end;
}



//template <class T1, class T2>
//const typename std::map<T1, T2>::const_iterator
//Utils::findKey(typename std::map<T1, T2>::const_iterator begin, typename std::map<T1, T2>::const_iterator end,
//               const T2 &value, T1) {
//    for (; begin != end; ++begin) {
//        if (begin->second == value)
//            return begin;
//    }
//    return end;
//}

//unsigned int Utils::getStrFileSize(const std::string &file_path) {
//    struct stat fi;
//
//}