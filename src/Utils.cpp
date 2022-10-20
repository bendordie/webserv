//
// Created by Carmel Shells on 12/19/21.
//

#include "Utils.hpp"

vector<string> Utils::split(const string& str, char ch) {

    vector<string>   result;

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

vector<string> Utils::split(const string& str, const string& val) {
    vector<string>   result;

    if (str.empty())
        return result;
    if (str.length() > 0 && str.find(val) == string::npos) {
        result.push_back(str);
        return (result);
    }
    size_t   i = 0;
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
    struct timeval   tv;
    time_t           nowtime;
    struct tm*       nowtm;
    char             tmbuf[64];

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%a, %d %b %Y %H:%M:%S GMT", nowtm);

    return tmbuf;
}

string Utils::getExtension(const string& file_name) {
    size_t   pos = file_name.rfind('.');

    if (pos <= 0) return "";
    return file_name.substr(pos+1, string::npos);
}

bool Utils::searchFileInDir(const char* dir_path, const char* file_name) {
    size_t			 len;
    struct dirent*   dp;
    DIR*             dirp;

    dirp = opendir(dir_path);
    if (dirp == NULL) {
        std::cerr << "searchFileInDir(): Can't open directory" << endl;
        return false;
    }
    len = strlen(file_name);
    dp = readdir(dirp);
    while (dp) {
        if (dp->d_namlen == len && strcmp(dp->d_name, file_name) == 0) {
            (void)closedir(dirp);
            return true;
        }
        dp = readdir(dirp);
    }
    (void)closedir(dirp);

    return false;
}

bool Utils::isPathExist(const string& path) {
        struct stat   buffer;

        return (stat(path.c_str(), &buffer) == 0);
}

bool Utils::isPathAccessed(const string& path) {
    struct stat   buffer;

    stat(path.c_str(), &buffer);
    return (EACCES);
}


bool Utils::getDirContent(const char* dir_path, list<struct dirent>& content) {
    struct dirent*   dp;
    DIR*             dirp;

    dirp = opendir(dir_path);
    if (dirp == NULL) {
        cout << "Can't open directory" << endl;
        return false;
    }
    dp = readdir(dirp);
    while (dp) {
        content.push_back(*dp);
        dp = readdir(dirp);
    }
    (void)closedir(dirp);

    return true;
}

string Utils::getFileLastModTime(const string& file_path) {
    struct stat     result;
    time_t          mod_time;
    struct tm*      nowtm;
    char            tmbuf[64];

    if (stat(file_path.c_str(), &result) == 0) {
        mod_time = result.st_mtime;
        nowtm = localtime(&mod_time);
        strftime(tmbuf, sizeof tmbuf, "%a, %d %b %Y %H:%M:%S GMT", nowtm);
    } else {
        return getTimeInString();
    }

    return tmbuf;
}


Utils::t_file Utils::readFile(string filePath, size_t maxBufSize, size_t bytesAlreadyRead) {
    ifstream   stream;
    char*      buf;
    t_file     file;
    size_t     fileFullSize;
    size_t     bytesRemaining;

    stream.open(filePath, ifstream::binary);

    if (!stream.is_open()) {
        std::cerr << "Utils: ReadFile: Can't open file" << std::endl;
        return file;
    }
    try {

        stream.seekg(0, ios_base::end);
        fileFullSize = stream.tellg();

        showDebugMessage("Utils: Reading file: File full size: " + std::to_string(fileFullSize));

        bytesRemaining = static_cast<size_t>(stream.tellg()) - bytesAlreadyRead;

        showDebugMessage("Utils: Reading file: Bytes already read: " + std::to_string(bytesAlreadyRead));
        showDebugMessage("Utils: Reading file: Bytes remaining: " + std::to_string(bytesRemaining));

        stream.seekg(-static_cast<int>(bytesRemaining), ios_base::end);

        const size_t   bufSize = bytesRemaining < maxBufSize ? bytesRemaining : maxBufSize;

        buf = new char[bufSize];
        *buf = 0;
        stream.read(buf, bufSize);
        stream.close();
    }
    catch (exception &ex) {
        std::cerr << ex.what() << endl;
    }
    file.data = buf;
    file.bytes_read = stream.gcount();
    file.type = getExtension(filePath);
    file.eof = (fileFullSize == file.bytes_read + bytesAlreadyRead) ? true : false;

    return file;
}

map<string, string>::const_iterator
Utils::findKey(map<string, string>::const_iterator begin, map<string, string>::const_iterator end,
               const string& value) {

    for (; begin != end; ++begin) {
        if (value == begin->second)
            return begin;
    }
    return end;
}

bool Utils::strToBool(const string& str) {
    if (str == "on" || str == "1" || str == "true")
        return true;
    return false;
}

long long Utils::strToLongLong(const string& str) {
    return stoll(str);
}

string Utils::intToHexString(int value) {

    ostringstream   oss;

    oss << std::hex << value;

    return oss.str();
}

const char *Utils::reverse_strstr(const char* source, const char* needle) {

    size_t   sourceLastElem = strlen(source) - 1;
    size_t   needleLastElem = strlen(needle) - 1;
    for (size_t i = sourceLastElem; i >= 0 && i >= needleLastElem; --i) {
        if (source[i] == needle[needleLastElem]) {
            for (size_t k = 0; source[i - k] == needle[needleLastElem - k]; ++k) {
                if (k == needleLastElem) {
                    return &source[i - k];
                }
            }
        }
    }

    return nullptr;
}