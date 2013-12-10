#include "fake_browser.h"

#include <sstream>


Request::Request(const std::string& method, const std::string& query) 
    : method_(method), query_(query) {
    // nothing
}

void Request::SetHeader(const std::string& key, const std::string& value) {
    headers_[key] = value;
}

void Request::SetParam(const std::string& key, const std::string& value) {
    params_[key] = value;
}

bool Request::GetRequest(std::string& request) const {

    if (method_ != "GET" && method_ != "POST") {
        return false;
    }

    if (host_.empty()) {
        return false;
    }

    std::string params;
    if (!params_.empty()) {
        std::map<std::string, std::string>::const_iterator itr = params_.begin();
        std::map<std::string, std::string>::const_iterator end = params_.end();
        params += (itr->first + "=" + itr->second);
        for (++itr; itr != end; ++itr) {
            params += ("&" + itr->first + "=" + itr->second);
        }
    }

    std::string query = (query_.empty() ? "/" : query_.c_str());
    if (method_ == "GET") {
        query += ("?" + params);
    }

    std::ostringstream ss;
    ss << method_ << " " << query << " " << "HTTP/1.1\r\n";
    ss << "Host: " << host_ << "\r\n";
    ss << "Connection: keep-alive\r\n";
    ss << "Accept: */*\r\n";
    ss << "User-Agent: FakeBrowser\r\n";
    if (!referer_.empty()) {
        ss << "Referer: " << referer_ << "\r\n";
    }
    if (method_ == "POST" && !params.empty()) {
        ss << "Content-Length: " << params.length() << "\r\n";
        ss << "\r\n";
        ss << params;
    } else {
        ss << "\r\n";
    }

    request = ss.str();
    return true;
}

const char* Request::mothod(void) const {
    return method_.c_str();
}

const char* Request::query(void) const {
    return query_.c_str();
}

const char* Request::host(void) const {
    return host_.c_str();
}

void Request::set_host(const std::string& host) {
    host_ = host;
}

const char* Request::referer(void) const {
    return referer_.c_str();
}

void Request::set_referer(const std::string& referer) {
    referer_ = referer;
}



FakeBrowser::FakeBrowser(void)
    : thread_(boost::bind(&IOService::run, &io_service_))
    , socket_(io_service_) {
}

FakeBrowser::~FakeBrowser(void) {
    io_service_.stop();
    thread_.join();
}

bool FakeBrowser::Connect(const char* host, const char* port) {
    TCP::resolver resolver(io_service_);
    TCP::resolver::query query(TCP::v4(), host, port);
    TCP::resolver::iterator iterator = resolver.resolve(query);
    boost::asio::connect(socket_, iterator);
    return socket_.is_open();
}

void FakeBrowser::Disconnect(void) {
    if (socket_.is_open()) {
        socket_.close();
    }
    io_service_.reset();
}

bool FakeBrowser::Send(const Request& request) {
    if (!socket_.is_open()) {
        return false;
    }

    std::string data;
    if (!request.GetRequest(data)) {
        return false;
    }

    boost::asio::write(socket_, boost::asio::buffer(data.c_str(), data.length()));

    //char response[1024];
    //const size_t read_bytes = boost::asio::read(socket_, boost::asio::buffer(response, 1024));
    boost::asio::streambuf response;
    const size_t read_bytes = boost::asio::read_until(socket_, response, "\0");

    return true;
}