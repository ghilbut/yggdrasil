#include <gmock/gmock.h>
#include "codebase/boost_lib_fwd.h"
#include "codebase/ssdp_scheduler.h"
#include "codebase/ssdp_listener.h"
#include <boost/thread.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

using ::testing::_;
using ::testing::StrEq;
using ::testing::AtLeast;
using ::testing::DoAll;


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
        : sender_(io_service_)
        , mock_listener_(io_service_)
        , listener_(mock_listener_) {
        // nothing
    }

    ~SsdpTest(void) {
        // nothing
    }

protected:
    virtual void SetUp(void) {
        io_service_.reset();
        thread_.swap(boost::thread(boost::bind(&IOService::run, &io_service_)));
    }

    virtual void TearDown(void) {
        io_service_.stop();
        thread_.join();
    }

public:
    boost::thread thread_;
    IOService io_service_;
    Ssdp::Scheduler sender_;
    MockListener mock_listener_;
    SsdpListener& listener_;
};



void DoSsdpCheck(const std::string& ssdp, boost::interprocess::interprocess_semaphore& done) {
    done.post();
}

ACTION_P2(DoSsdpCheck, target, sender) {
    sender->UnregistTarget(target);
}

ACTION_P(DoSsdpDone, done) {
    done->post();
}



TEST_F(SsdpTest, run) {
    ASSERT_FALSE(io_service_.stopped());
}

TEST_F(SsdpTest, search_and_not_found_single_target) {
    // DESC(jh81.kim): 
    // ssdp serach target every 5 seconds.
    // if there is not target on network,
    // there will be at least 2 ssdp packets in 11 seconds.
    const char target[] = "UUID";
    boost::interprocess::interprocess_semaphore done(0);

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).Times(AtLeast(2));

    sender_.RegistTarget(target);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_FALSE(done.timed_wait(until));
}

TEST_F(SsdpTest, search_and_found_single_target_1) {
    // DESC(jh81.kim): 
    // ssdp serach target every 5 seconds.
    // if there found target on network, stop search target.
    // there will be just single ssdp packet in 11 seconds.
    const char target[] = "UUID";
    boost::interprocess::interprocess_semaphore done(0);

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).Times(1).WillOnce(DoSsdpCheck(target, &sender_));

    sender_.RegistTarget(target);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_FALSE(done.timed_wait(until));
}

TEST_F(SsdpTest, DISABLED_search_and_found_single_target_2) {
    // DESC(jh81.kim): 
    // failed first search
    // found second search
    const char target[] = "UUID";
    boost::interprocess::interprocess_semaphore done(0);

    // TODO(ghilbut): how can I do expression ?
    //EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).Times(1);
    //EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).Times(1).WillOnce(DoAll(DoSsdpCheck(target, &sender_), DoSsdpDone(&done)));

    sender_.RegistTarget(target);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_TRUE(done.timed_wait(until));
}

TEST_F(SsdpTest, search_and_not_found_multi_target) {
    const char target1[] = "UUID1";
    const char target2[] = "UUID2";
    const char target3[] = "UUID3";
    boost::interprocess::interprocess_semaphore done1(0);
    boost::interprocess::interprocess_semaphore done2(0);
    boost::interprocess::interprocess_semaphore done3(0);

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target1), _)).Times(AtLeast(2));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target2), _)).Times(AtLeast(2));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target3), _)).Times(AtLeast(2));

    sender_.RegistTarget(target1);
    sender_.RegistTarget(target2);
    sender_.RegistTarget(target3);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_FALSE(done1.timed_wait(until));
    EXPECT_FALSE(done2.timed_wait(until));
    EXPECT_FALSE(done3.timed_wait(until));
}

TEST_F(SsdpTest, search_and_found_multi_target) {
    const char target1[] = "UUID1";
    const char target2[] = "UUID2";
    const char target3[] = "UUID3";
    boost::interprocess::interprocess_semaphore done1(0);
    boost::interprocess::interprocess_semaphore done2(0);
    boost::interprocess::interprocess_semaphore done3(0);

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target1), _)).Times(1).WillOnce(DoAll(DoSsdpCheck(target1, &sender_), DoSsdpDone(&done1)));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target2), _)).Times(1).WillOnce(DoAll(DoSsdpCheck(target2, &sender_), DoSsdpDone(&done2)));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target3), _)).Times(1).WillOnce(DoAll(DoSsdpCheck(target3, &sender_), DoSsdpDone(&done3)));

    sender_.RegistTarget(target1);
    sender_.RegistTarget(target2);
    sender_.RegistTarget(target3);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_TRUE(done1.timed_wait(until));
    EXPECT_TRUE(done2.timed_wait(until));
    EXPECT_TRUE(done3.timed_wait(until));
}