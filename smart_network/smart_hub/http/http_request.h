#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "channel.h"
#include <boost/thread.hpp>
#include <string>


struct mg_connection;

class HttpRequest {
public:
	HttpRequest(Channel& channel, mg_connection* conn);
	~HttpRequest(void);

    bool DoRequest(const char* id, const char* method, const char* uri);
    void DoResponse(const std::string& json);


private:
    Channel& channel_;
    mg_connection* conn_;
    boost::condition_variable cond_;
    boost::mutex mutex_;
};

#endif  // HTTP_REQUEST_H_