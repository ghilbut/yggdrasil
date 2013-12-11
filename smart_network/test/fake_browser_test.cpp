#include "fake_browser.h"
#include "mongoose_mock.h"
#include <boost/interprocess/sync/interprocess_semaphore.hpp>


using ::testing::_;
using ::testing::WithArgs;
using ::testing::Invoke;

extern std::string kStoragePath;

class FakeBrowserTest : public testing::Test {
public:
    FakeBrowserTest(void)
        //: mock_(kStoragePath.c_str(), 80) {
        : mock_((boost::filesystem::path(kStoragePath) / "test_device_id/ui").string().c_str(), 80) {
        // nothing
    }

    ~FakeBrowserTest(void) {}

protected:
    virtual void SetUp(void) {
        browser_.Connect("127.0.0.1", "80");
    }

    virtual void TearDown(void) {
        browser_.Disconnect();
    }

public:
    FakeBrowser browser_;
    MongooseMock mock_;
};



ACTION(DoBeginRequest) {
    struct mg_connection* conn = arg0;
    return 0;
}

ACTION(DoEndRequest) {
    const struct mg_connection* conn = arg0;
    const int reply_status_code = arg1;
}

TEST_F(FakeBrowserTest, empty) {
    EXPECT_CALL(mock_, OnBeginRequest(_))
        .Times(1)
        .WillOnce(WithArgs<0>(DoBeginRequest()));
    EXPECT_CALL(mock_, OnEndRequest(_, _))
        .Times(1)
        .WillOnce(WithArgs<0, 1>(DoEndRequest()));

    Request r("GET", "/");
    r.set_host("localhost");
    EXPECT_TRUE(browser_.Send(r));
}