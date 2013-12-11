#ifndef MONGOOSE_MOCK_H_
#define MONGOOSE_MOCK_H_

#include <gmock/gmock.h>
#include <stdint.h>


struct mg_context;
struct mg_connection;

class MongooseMock {
public:
    MongooseMock(const char* document_root, uint32_t port);
    ~MongooseMock(void);

    MOCK_METHOD1(OnBeginRequest, int(struct mg_connection* conn));
    MOCK_METHOD2(OnEndRequest, void(const struct mg_connection* conn, int reply_status_code));
    MOCK_METHOD1(OnWebsocketConnect, int(const struct mg_connection* conn));
    MOCK_METHOD1(OnWebsocketReady, void(struct mg_connection* conn));
    MOCK_METHOD4(OnWebsocketData, int(struct mg_connection* conn, int bits, char* data, size_t data_len));

private:
    mg_context* ctx_;
};

#endif  // MONGOOSE_MOCK_H_