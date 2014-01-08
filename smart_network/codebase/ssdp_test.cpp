#include <gmock/gmock.h>
#include "codebase/boost_lib_fwd.h"
#include "codebase/ssdp_scheduler.h"
#include "codebase/ssdp_ethernet_sender.h"
#include "codebase/ssdp_listener.h"
#include <boost/thread.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

using ::testing::_;
using ::testing::StrEq;
using ::testing::AtLeast;
using ::testing::DoAll;


class MockTrigger {
public:
    MockTrigger(void) {}
    ~MockTrigger(void) {}

    MOCK_METHOD0(SendSsdp, void());

private:
    GTEST_DISALLOW_COPY_AND_ASSIGN_(MockTrigger);
};

class MockListener : public SsdpListener {
public:
    MockListener(IOService& io_service)
        : SsdpListener(io_service) {
            // nothing
    }

    MOCK_METHOD2(OnSsdp, void(const std::string&, const std::string&));

private:
    GTEST_DISALLOW_COPY_AND_ASSIGN_(MockListener);
};

class SsdpTest : public testing::Test {
public:
    SsdpTest(void)
        : scheduler_(io_service_)
        , mock_listener_(io_service_)
        , listener_(mock_listener_)
        , eth_sender_(io_service_) {
        // nothing
    }

    ~SsdpTest(void) {
        // nothing
    }

protected:
    virtual void SetUp(void) {
        io_service_.reset();
        thread_.swap(boost::thread(boost::bind(&IOService::run, &io_service_)));
        scheduler_.Start();
    }

    virtual void TearDown(void) {
        scheduler_.Stop();
        io_service_.stop();
        thread_.join();
    }

public:
    boost::thread thread_;
    IOService io_service_;
    Ssdp::Scheduler scheduler_;
    MockListener mock_listener_;
    SsdpListener& listener_;



    MockTrigger mock_trigger_;

    Ssdp::EthernetSender eth_sender_;
};



ACTION_P(DoSsdpDone, done) {
    done->post();
}

ACTION_P2(DoSsdpCheck, target, sender) {
    //sender->UnregistTarget(target);
}



TEST_F(SsdpTest, run) {
    ASSERT_FALSE(io_service_.stopped());
}

TEST_F(SsdpTest, triggered_by_scheduler) {
    boost::interprocess::interprocess_semaphore done(0);

    EXPECT_CALL(mock_trigger_, SendSsdp()).Times(AtLeast(2));

    scheduler_.BindTrigger(boost::bind(&MockTrigger::SendSsdp, &mock_trigger_));

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_FALSE(done.timed_wait(until));
}

TEST_F(SsdpTest, ethernet_send_ssdp) {
    const char target[] = "UUID";
    boost::interprocess::interprocess_semaphore done(0);

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).Times(1).WillOnce(DoSsdpDone(&done));

    eth_sender_.RegistTarget(target);
    eth_sender_.Send();

    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(1);
    EXPECT_TRUE(done.timed_wait(until));

    eth_sender_.UnregistTarget(target);
}