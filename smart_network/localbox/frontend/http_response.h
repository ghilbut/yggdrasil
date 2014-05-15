#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include <v8.h>
#include <map>
#include <string>
#include <vector>


struct mg_connection;

namespace Http {

class Request;

class Response {
public:
    class Impl;
    Response(void);
    Response(int status_code, const char* data, int data_len);
    Response(const Response& other);
    ~Response(void);

    void Reset(Impl* pimpl);
    Response& operator= (const Response& other);

    int status_code(void) const;
    void set_status_code(int status_code);

    const char* GetHeader(const char* name) const;
    void SetHeader(const char* name, const char* value);
    void RemoveHeader(const char* name);

    const std::string& data(void) const;
    void set_data(const char* data, int data_len);

    void Send(struct mg_connection* conn) const;


private:
    Impl* pimpl_;
};

}  // namespace Http

#endif  // HTTP_RESPONSE_H_