/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:18 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:19 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest(string type, string path)
: HttpMessage(), _type(type), _path(path) {

    addHeaderParam("Request-Type", type);
    addHeaderParam("Request-Path", path);
}

HttpRequest::HttpRequest(std::string type, std::string path, bool keep_alive, std::string content_type,
                         long long content_length, size_t header_size, size_t size)
                         : HttpMessage(), _type(type), _path(path), _keep_alive(keep_alive), _content_type(content_type),
                         _content_length(content_length), _header_size(header_size), _size(size),
                         _full_received(false) {}

HttpRequest::~HttpRequest() {}

HttpRequest *HttpRequest::createRequest(string request_type, const char *header_begin, const char *header_end, size_t buf_size) {

    size_t      header_size = header_end - header_begin;
    std::string request_path = defineRequestPath(header_begin);

    if (request_path.empty())
        return 0;

    HttpRequest     *request = new HttpRequest(request_type, request_path);
    vector<string>  header_lines = Utils::split(string(strchr(header_begin, '\n') + 1, header_end), '\n');

    pair<string, string>    header_entry;
    for (vector<string>::iterator it = header_lines.begin(); it != header_lines.end(); ++it) {

        header_entry = extractParam2((*it));
        if (header_entry.first.empty())
            return 0;
    }

    long long       content_length = 0;
    std::string     content_type = "";
    if (request_type == "POST") {
        content_length = request->getContentLength();
        content_type = request->getType();
    }
    size_t      request_size = header_size + strlen("\r\n") + content_length;

    bool        fully_received;
    const char  *data_begin = header_end + strlen("\r\n");
    const char  *data_end;

    if (data_begin + content_length > header_begin + buf_size) {
        data_end = header_begin + buf_size;
        fully_received = false;
    } else {
        data_end = data_begin + content_length;
        fully_received = true;
    }
    request->setData(data_begin, data_end);
    request->setFullReceivedFlag(fully_received);

    return request;
}

pair<string, string> HttpRequest::extractParam2(const string &str) {

    size_t  separator_pos = str.find(": ", 0);

    if (separator_pos == string::npos)
        return make_pair("", "");

    string  key = str.substr(0, separator_pos);
    string  value = str.substr(separator_pos + strlen(": "));

    return make_pair(key, value);
}

string HttpRequest::defineRequestPath(const char *begin) {
    string          result_path;
    const char      *first_space_pos;
    const char      *second_space_pos;

    if (begin) {
        first_space_pos = strchr(begin, ' ');
        if (!first_space_pos || !(first_space_pos + 1))
            return "";
        second_space_pos = strchr(first_space_pos + 1, ' ');
        if (!second_space_pos)
            return "";
        result_path.assign(first_space_pos + 1, second_space_pos);
    }
    return result_path;
}

std::string HttpRequest::extractParam(const char *header_begin, const char *param_name) {

    const char      *from;
    const char      *to;

    if (header_begin) {
        from = strstr(header_begin, param_name);
        if (!from || !(from + strlen(param_name)))
            return "";
        to = strstr(from + strlen(param_name), "\r\n");
        if (!to)
            return "";
    }
    return std::string(from, to);
}

void HttpRequest::setPath(const std::string &path) { _path = path; }

void HttpRequest::setContentType(const std::string &contentType) { _content_type = contentType; }

void HttpRequest::setContentLength(const long long &contentLength) { _content_length = contentLength; }

void HttpRequest::setUri(const string &uri) { _uri = uri; }

void HttpRequest::setFullReceivedFlag(bool value) { _full_received = value; }

const std::string &HttpRequest::getType() const { return _type; }

const std::string &HttpRequest::getPath() const { return _path; }

const std::string &HttpRequest::getContentType() const { return _content_type; }

const long long &HttpRequest::getContentLength() const {
    const string&   value = getHeaderParamValue("Content-Length");

    if (value.empty())
        return -1;

    long long   content_length = std::atol(value.c_str()); // TODO: try carch
    //        content_length = strtoll(header_begin, )
    //        content_length = std::wcstol(extractParam(header_begin, "Content-Length: "), 0, 10); // try catch nedded

    return content_length;
}

const size_t &HttpRequest::getHeaderSize() const { return _header_size; }

const size_t &HttpRequest::getSize() const { return _size; }

const size_t &HttpRequest::getDataSize() const { return _data.size(); }

const bool &HttpRequest::getFullReceivedFlag() const { return _full_received; }

const size_t HttpRequest::getRestDataSize() const { return _content_length - _data.size(); }

const string &HttpRequest::getUri() const { return _uri; }

bool HttpRequest::isKeepAlive() const {

    if (getHeaderParamValue("Connection") == "keep-alive")
        return true;

    return false;
}