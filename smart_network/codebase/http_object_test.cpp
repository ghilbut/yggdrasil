#include "codebase/boost_lib_fwd.h"
#include "codebase/http_object.h"
#include <gmock/gmock.h>


extern std::string kStoragePath;

class HttpObjectMock : public Http::Object {
public:
    HttpObjectMock(const std::string& document_root, uint32_t port)
        : Http::Object(document_root, port) {
        // nothing
    }

    ~HttpObjectMock(void) {}

protected:
    MOCK_METHOD1(OnBeginRequest, int(struct mg_connection* conn));
    MOCK_METHOD2(OnEndRequest, void(const struct mg_connection* conn, int reply_status_code));
    MOCK_METHOD1(OnWebsocketConnect, int(const struct mg_connection* conn));
    MOCK_METHOD1(OnWebsocketReady, void(struct mg_connection* conn));
    MOCK_METHOD4(OnWebsocketData, int(struct mg_connection* conn, int bits, char* data, size_t data_len));

private:
    GTEST_DISALLOW_COPY_AND_ASSIGN_(HttpObjectMock);
};

class TestBrowser {
};

class HttpObjectTest : public testing::Test {
public:
    HttpObjectTest(void)
        : mock_object_("", 10000)
        , storage_is_invalid_(!boost::filesystem::is_directory(kStoragePath)) {
    }

    ~HttpObjectTest(void) {}

    bool storage_is_invalid(void) const {
        return storage_is_invalid_;
    }

protected:
    virtual void SetUp(void) {
        mock_object_.Start();
    }

    virtual void TearDown(void) {
        mock_object_.Stop();
    }

private:
    HttpObjectMock mock_object_;
    bool storage_is_invalid_;
};



TEST_F(HttpObjectTest, storage_exists) {
    ASSERT_FALSE(storage_is_invalid());
}