//
// Created by Carmel Shells on 12/19/21.
//

#include "Utils.hpp"

vector<string> Utils::split(const string &str, char ch) {

    vector<string> result;

    if (str.length() > 0 && str.find(ch) == string::npos) {
        result.push_back(str);
        return (result);
    }
    for (size_t i = str.find_first_not_of(ch, 0), j = str.find(ch, i); j != string::npos;) {
        result.push_back(str.substr(i,j - i));
        for (; str[j] == ch; ++j) {}
        i = j;
        j = str.find(ch, i);
        if (i != str.length() && j == string::npos) {
            result.push_back(str.substr(i, str.length()));
        }
    }
    return result;
}

vector<string> Utils::split(const string &str, const string &val) {
    vector<string> result;

    if (str.length() > 0 && str.find(val) == string::npos) {
        result.push_back(str);
        return (result);
    }
    size_t i = 0;
    for (; i != str.length() && strnstr(&str[i], val.c_str(), val.length()); i += val.length()) {}
    for (size_t j = str.find(val, i); j != string::npos;) {
        result.push_back(str.substr(i,j - i));
        for (; j != str.length() && strnstr(&str[j], val.c_str(), val.length()); j += val.length()) {}
        i = j;
        j = str.find(val, i);
        if (i != str.length() && j == string::npos) {
            result.push_back(str.substr(i, str.length()));
        }
    }
    return result;
}

string Utils::getTime() {
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

string Utils::getExtension(const string &file_name) {
    size_t pos = file_name.rfind('.');

    if (pos <= 0) return "";
    return file_name.substr(pos+1, string::npos);
}

bool Utils::searchFileInDir(const char* dir_path, const char* file_name) {
    size_t			len;
    struct dirent	*dp;
    DIR				*dirp;

    dirp = opendir(dir_path);
    if (dirp == NULL) {
        cout << "Can't open directory" << endl;
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
//        cout << "Can't open directory" << endl;
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

bool Utils::isPathExist(const string &path) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
}

bool Utils::isPathAccessed(const string &path) {
    struct stat buffer;

    stat(path.c_str(), &buffer);
    return (EACCES);
}


bool Utils::getDirContent(const char* dir_path, list<struct dirent> &content) {
    size_t			len;
    struct dirent	*dp;
    DIR				*dirp;

    dirp = opendir(dir_path);
    if (dirp == NULL) {
        cout << "Can't open directory" << endl;
        return false;
    }
    dp = readdir(dirp);
//    int i = 0;
    while (dp)
    {
//        cout << i << endl;
        content.push_back(*dp);
        dp = readdir(dirp);
//        i++;
    }
    (void)closedir(dirp);
    return true;
}

string Utils::getFileLastModTime(const string& file_path) {
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


Utils::t_file Utils::readFile(string file_path) {
    ifstream        stream;
    char            *buff;
    t_file          file;

    stream.open(file_path, ifstream::binary);
    if (stream.is_open()) {
        try {
            struct stat fi;

            bzero(&fi, sizeof(fi));
            stat(file_path.c_str(), &fi);
            buff = new char[fi.st_size];
            *buff = 0;
            stream.read(buff, fi.st_size);
            stream.close();
        }
        catch (exception &ex) {
            cout << ex.what() << endl;
        }
    } else {
        cout << "ReadFile: Can't open file" << endl;
        file.data = 0;
        file.size = 0;
        return file;
    }
    file.data = buff;
    file.size = stream.gcount();
    file.type = getExtension(file_path);

    return file;
}

map<string, string>::const_iterator
Utils::findKey(map<string, string>::const_iterator begin, map<string, string>::const_iterator end,
               const string &value) {
    for (; begin != end; ++begin) {
        if (value == begin->second)
            return begin;
    }
    return end;
}



//template <class T1, class T2>
//const typename map<T1, T2>::const_iterator
//Utils::findKey(typename map<T1, T2>::const_iterator begin, typename map<T1, T2>::const_iterator end,
//               const T2 &value, T1) {
//    for (; begin != end; ++begin) {
//        if (begin->second == value)
//            return begin;
//    }
//    return end;
//}

//unsigned int Utils::getStrFileSize(const string &file_path) {
//    struct stat fi;
//
//}