#include <gmock/gmock.h>
#include "codebase/boost_lib_fwd.h"
#include "codebase/ssdp_scheduler.h"
#include "codebase/network_manager.h"
#include "codebase/tcp_adapter.h"
#include "codebase/ssdp_listener.h"
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

using ::testing::_;
using ::testing::StrEq;
using ::testing::AtLeast;
using ::testing::DoAll;



class MockEthernetListener : public SsdpListener {
public:
    MockEthernetListener(IOService& io_service)
        : SsdpListener(io_service) {
            // nothing
    }

    MOCK_METHOD2(OnSsdp, void(const std::string&, const std::string&));

private:
    GTEST_DISALLOW_COPY_AND_ASSIGN_(MockEthernetListener);
};

class SsdpEthernetTest : public testing::Test, public ChannelDelegate {
public:
    SsdpEthernetTest(void)
        : mock_listener_(io_service_)
        , net_manager_(io_service_) {
        // nothing
    }

    ~SsdpEthernetTest(void) {
        // nothing
    }

    // ChannelDelegate
    virtual void OnConnected(const std::string& json, Channel* channel) {
    }

    virtual void OnResponse(const std::string& json) {
    }

    virtual void OnEvent(const std::string& text) {
    }
    
    virtual void OnDisconnected(void) {
    }

protected:
    virtual void SetUp(void) {
        NetworkAdapter::Ptr eth_adapter(new TcpAdapter(io_service_, this));
        net_manager_.Register("ethernet", eth_adapter);
        net_manager_.Start();

        io_service_.reset();
        thread_.swap(boost::thread(boost::bind(&IOService::run, &io_service_)));
    }

    virtual void TearDown(void) {
        net_manager_.Stop();

        io_service_.stop();
        thread_.join();
    }

public:
    boost::thread thread_;
    IOService io_service_;
    MockEthernetListener mock_listener_;


    NetworkManager net_manager_;
};



ACTION_P(DoSsdpDone, done) {
    done->post();
}

ACTION_P2(DoSsdpRegister, manager, target) {
    manager->RegisterSsdpTarget("ethernet", target);
}

ACTION_P2(DoSsdpUnregister, manager, target) {
    manager->UnregisterSsdpTarget("ethernet", target);
}





TEST_F(SsdpEthernetTest, search_and_not_found_single_target) {
    // NOTE(ghilbut): 
    // ssdp serach target every 5 seconds.
    // if there is not target on network,
    // there will be at least 2 ssdp packets in 11 seconds.
    const char target[] = "UUID";

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).Times(AtLeast(2));
    net_manager_.RegisterSsdpTarget("ethernet", target);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    boost::interprocess::interprocess_semaphore done(0);
    EXPECT_FALSE(done.timed_wait(until));

    net_manager_.UnregisterSsdpTarget("ethernet", target);
}

TEST_F(SsdpEthernetTest, search_and_found_single_target_1) {
    // NOTE(ghilbut): 
    // ssdp serach target every 5 seconds.
    // if there found target on network, stop search target.
    // there will be just single ssdp packet in 11 seconds.
    const char target[] = "UUID";
    boost::interprocess::interprocess_semaphore done(0);

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).Times(1).WillOnce(DoAll(DoSsdpUnregister(&net_manager_, target), DoSsdpDone(&done)));
    net_manager_.RegisterSsdpTarget("ethernet", target);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_TRUE(done.timed_wait(until));

    net_manager_.UnregisterSsdpTarget("ethernet", target);
}

TEST_F(SsdpEthernetTest, search_and_found_single_target_2) {
    // NOTE(ghilbut): 
    // ssdp serach target every 5 seconds.
    // if there found target on network, stop search target.
    // there will be just single ssdp packet in 11 seconds.
    const char target[] = "UUID";

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).Times(1).WillOnce(DoSsdpUnregister(&net_manager_, target));
    net_manager_.RegisterSsdpTarget("ethernet", target);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    boost::interprocess::interprocess_semaphore done(0);
    EXPECT_FALSE(done.timed_wait(until));

    net_manager_.UnregisterSsdpTarget("ethernet", target);
}

TEST_F(SsdpEthernetTest, DISABLED_search_and_found_single_target_3) {
    // NOTE(ghilbut): 
    // failed first search
    // found second search
    const char target[] = "UUID";
    boost::interprocess::interprocess_semaphore done(0);

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).WillOnce(DoSsdpRegister(&net_manager_, target));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target), _)).WillOnce(DoAll(DoSsdpUnregister(&net_manager_, target), DoSsdpDone(&done)));

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_TRUE(done.timed_wait(until));

    net_manager_.UnregisterSsdpTarget("ethernet", target);
}

TEST_F(SsdpEthernetTest, search_and_not_found_multi_target) {
    const char target1[] = "UUID1";
    const char target2[] = "UUID2";
    const char target3[] = "UUID3";

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target1), _)).Times(AtLeast(2));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target2), _)).Times(AtLeast(2));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target3), _)).Times(AtLeast(2));
    net_manager_.RegisterSsdpTarget("ethernet", target1);
    net_manager_.RegisterSsdpTarget("ethernet", target2);
    net_manager_.RegisterSsdpTarget("ethernet", target3);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    boost::interprocess::interprocess_semaphore done1(0);
    boost::interprocess::interprocess_semaphore done2(0);
    boost::interprocess::interprocess_semaphore done3(0);
    EXPECT_FALSE(done1.timed_wait(until));
    EXPECT_FALSE(done2.timed_wait(until));
    EXPECT_FALSE(done3.timed_wait(until));

    net_manager_.UnregisterSsdpTarget("ethernet", target1);
    net_manager_.UnregisterSsdpTarget("ethernet", target2);
    net_manager_.UnregisterSsdpTarget("ethernet", target3);
}

TEST_F(SsdpEthernetTest, search_and_found_multi_target_1) {
    const char target1[] = "UUID1";
    const char target2[] = "UUID2";
    const char target3[] = "UUID3";
    boost::interprocess::interprocess_semaphore done1(0);
    boost::interprocess::interprocess_semaphore done2(0);
    boost::interprocess::interprocess_semaphore done3(0);

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target1), _)).Times(1).WillOnce(DoSsdpDone(&done1));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target2), _)).Times(1).WillOnce(DoSsdpDone(&done2));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target3), _)).Times(1).WillOnce(DoSsdpDone(&done3));
    net_manager_.RegisterSsdpTarget("ethernet", target1);
    net_manager_.RegisterSsdpTarget("ethernet", target2);
    net_manager_.RegisterSsdpTarget("ethernet", target3);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    EXPECT_TRUE(done1.timed_wait(until));
    EXPECT_TRUE(done2.timed_wait(until));
    EXPECT_TRUE(done3.timed_wait(until));

    net_manager_.UnregisterSsdpTarget("ethernet", target1);
    net_manager_.UnregisterSsdpTarget("ethernet", target2);
    net_manager_.UnregisterSsdpTarget("ethernet", target3);
}

TEST_F(SsdpEthernetTest, search_and_found_multi_target_2) {
    const char target1[] = "UUID1";
    const char target2[] = "UUID2";
    const char target3[] = "UUID3";

    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target1), _)).Times(1).WillOnce(DoSsdpUnregister(&net_manager_, target1));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target2), _)).Times(1).WillOnce(DoSsdpUnregister(&net_manager_, target2));
    EXPECT_CALL(mock_listener_, OnSsdp(StrEq(target3), _)).Times(1).WillOnce(DoSsdpUnregister(&net_manager_, target3));
    net_manager_.RegisterSsdpTarget("ethernet", target1);
    net_manager_.RegisterSsdpTarget("ethernet", target2);
    net_manager_.RegisterSsdpTarget("ethernet", target3);

    const long sec = Ssdp::Scheduler::kSendIntervalSec * 2 + 1;
    boost::posix_time::ptime until = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(sec);
    boost::interprocess::interprocess_semaphore done1(0);
    boost::interprocess::interprocess_semaphore done2(0);
    boost::interprocess::interprocess_semaphore done3(0);
    EXPECT_FALSE(done1.timed_wait(until));
    EXPECT_FALSE(done2.timed_wait(until));
    EXPECT_FALSE(done3.timed_wait(until));

    net_manager_.UnregisterSsdpTarget("ethernet", target1);
    net_manager_.UnregisterSsdpTarget("ethernet", target2);
    net_manager_.UnregisterSsdpTarget("ethernet", target3);
}