//
// Created by Carmel Shells on 12/19/21.
//

#include "Utils.hpp"

vector<string> Utils::split(const string &str, char ch) {

    vector<string> result;

    if (str.empty())
        return result;
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

    if (str.empty())
        return result;
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

string Utils::getTimeInString() {
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
        return getTimeInString();
    }
    return tmbuf;
}


Utils::t_file Utils::readFile(string file_path, size_t max_buf_size, size_t bytes_already_read) {
    ifstream        stream;
    char            *buf;
    t_file          file {0, 0 ,"", 0};
    size_t          file_full_size;
    size_t          bytes_remaining;

    stream.open(file_path, ifstream::binary);

    if (!stream.is_open()) {
        cout << "ReadFile: Can't open file" << endl;
        return file;
    }
    try {

        stream.seekg(0, ios_base::end);
        file_full_size = stream.tellg();
        bytes_remaining = static_cast<size_t>(stream.tellg()) - bytes_already_read;
        stream.seekg(-static_cast<int>(bytes_remaining), ios_base::end);

        size_t buf_size = bytes_remaining < max_buf_size ? bytes_remaining : max_buf_size;

//            struct stat fi;

//            bzero(&fi, sizeof(fi));
//            stat(file_path.c_str(), &fi);

        buf = new char[buf_size];
        *buf = 0;
        stream.read(buf, buf_size);
        stream.close();
    }
    catch (exception &ex) {
        cout << ex.what() << endl;
    }
    file.data = buf;
    file.bytes_read = stream.gcount();
    file.type = getExtension(file_path);
    file.eof = (file_full_size == file.bytes_read + bytes_already_read) ? true : false;

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
//        const T2 &value) {
//    for (; begin != end; ++begin) {
//        if (begin->second == value)
//            return begin;
//    }
//    return end;
//}

bool Utils::strToBool(const string &str) {
    if (str == "on" || str == "1" || str == "true")
        return true;
    return false;
}

long long Utils::strToLongLong(const string &str) {
    return stoll(str);
}

string Utils::intToHexString(int value) {

    ostringstream oss;

    oss << std::hex << value;

    return oss.str();
}

//safsgjrekolafer
//        gola

const char *Utils::reverse_strstr(const char *source, const char *needle) {

    int sourceLastElem = strlen(source) - 1;
    int needleLastElem = strlen(needle) - 1;
    for (size_t i = sourceLastElem, j = needleLastElem; i >= 0 && i >= needleLastElem; --i) {
        if (source[i] == needle[needleLastElem]) {
            for (int k = 0; source[i - k] == needle[needleLastElem - k]; ++k) {
                if (k == needleLastElem) {
                    return &source[i - k];
                }
            }
        }
    }

    return nullptr;
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