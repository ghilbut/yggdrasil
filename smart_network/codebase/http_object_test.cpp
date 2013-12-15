#include <gmock/gmock.h>
#include "fake_browser.h"
#include "codebase/http_object.h"
#include "codebase/boost_lib_fwd.h"
#include <boost/interprocess/sync/interprocess_semaphore.hpp>


extern std::string kStoragePath;

class HttpObjectMock : public Http::Object {
public:
    HttpObjectMock(const std::string& document_root, uint32_t port)
        : Http::Object(document_root, port) {
        // nothing
    }

    ~HttpObjectMock(void) {}

public:
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
        : port_(10000)
        , mock_object_("", port_)
        , storage_is_invalid_(!boost::filesystem::is_directory(kStoragePath)) {
        // nothing
    }

    ~HttpObjectTest(void) {}

    bool storage_is_invalid(void) const {
        return storage_is_invalid_;
    }

protected:
    virtual void SetUp(void) {
        mock_object_.Start();
        char sport[10];
        sprintf(sport, "%u", port_);
        browser_.Connect("127.0.0.1", sport);
    }

    virtual void TearDown(void) {
        browser_.Disconnect();
        mock_object_.Stop();
    }

public:
    uint32_t port_;
    FakeBrowser browser_;
    HttpObjectMock mock_object_;
    bool storage_is_invalid_;
};



TEST_F(HttpObjectTest, storage_exists) {
    ASSERT_FALSE(storage_is_invalid());
}

ACTION_P(DoRequestDone, done) {
    done->post();
}
using ::testing::_;
TEST_F(HttpObjectTest, DISABLED_request_timeout_of_mongoose) {
    boost::interprocess::interprocess_semaphore done(0);

    EXPECT_CALL(mock_object_, OnBeginRequest(_)).Times(1);
    EXPECT_CALL(mock_object_, OnEndRequest(_, _)).Times(1).WillOnce(DoRequestDone(&done));

    RequestGet r("/");
    r.set_host("localhost");
    EXPECT_TRUE(browser_.Send(r));

    const long sec = 120;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_FALSE(done.timed_wait(until));
}