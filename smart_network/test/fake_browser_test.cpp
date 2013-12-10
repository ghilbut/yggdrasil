#include "fake_browser.h"
#include "mongoose_mock.h"
#include <boost/interprocess/sync/interprocess_semaphore.hpp>


using ::testing::_;

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



TEST_F(FakeBrowserTest, empty) {
    EXPECT_CALL(mock_, OnBeginRequest(_)).Times(1);
    EXPECT_CALL(mock_, OnEndRequest(_, _)).Times(1);

    Request r("GET", "/");
    r.set_host("localhost");
    EXPECT_TRUE(browser_.Send(r));
}