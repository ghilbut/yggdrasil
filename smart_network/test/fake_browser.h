#ifndef FAKE_BROWSER_H_
#define FAKE_BROWSER_H_

#include "codebase/boost_lib_fwd.h"
#include <map>
#include <string>


class Request {
public:
    Request(const std::string& method, const std::string& query);
    virtual ~Request(void) {}

    void SetHeader(const std::string& key, const std::string& value);
    void SetParam(const std::string& key, const std::string& value);

    bool GetRequest(std::string& request) const;

    const char* mothod(void) const;
    const char* query(void) const;
    const char* host(void) const;
    void set_host(const std::string& host);
    const char* referer(void) const;
    void set_referer(const std::string& referer);

private:
    const std::string method_;
    const std::string query_;
    std::string host_;
    std::string referer_;
    std::map<std::string, std::string> headers_;
    std::map<std::string, std::string> params_;
};

class Response {
};


class FakeBrowser {
public:
    FakeBrowser(void);
    ~FakeBrowser(void);

    bool Connect(const char* host, const char* port);
    void Disconnect(void);
    bool Send(const Request& request);


private:
    IOService io_service_;
    boost::thread thread_;
    TCP::socket socket_;
    Response response_;
};

#endif  // FAKE_BROWSER_H_