#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "http_websocket.h"
#include <boost/function.hpp>
#include <map>
#include <string>
#include <stdint.h>


class HttpServer {
public:
	explicit HttpServer(const std::string& common_root);
	~HttpServer(void);

    bool Insert(HttpObject* http);
    void Remove(HttpObject* http);

    virtual int DoHandleCommon(mg_connection* conn
                               , const char* method
                               , const char* uri) const;


private:
    HttpMap http_map_;
    const std::string common_;

    HttpWebsocket websocket_;
};

#endif  // HTTP_SERVER_H_