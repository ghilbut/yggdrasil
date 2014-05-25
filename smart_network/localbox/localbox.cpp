#include "localbox.h"
#include "sample.h"
#include "basebox/device_ref.h"
#include "basebox/service.h"
#include "basebox/service_manager.h"
#include "base/io_service.h"
#include "backend/network_manager.h"
#include "backend/tcp_adapter.h"
#include <json/json.h>


static int http_request_handler(struct mg_connection* conn, enum mg_event ev) {
    LocalBox* s = static_cast<LocalBox*>(conn->server_param);
    return s->HttpRequest(conn, ev);
};

LocalBox::LocalBox(const char* rootpath) 
    : isolate_(v8::Isolate::GetCurrent())
    , handle_scope_(isolate_)
    , storage_(rootpath) {

    //isolate_->SetData(0, &io_service_);

    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate_);
    global->Set(v8::String::NewFromUtf8(isolate_, "print"), v8::FunctionTemplate::New(isolate_, Print));
    global->Set(v8::String::NewFromUtf8(isolate_, "read"), v8::FunctionTemplate::New(isolate_, Read));
    global->Set(v8::String::NewFromUtf8(isolate_, "load"), v8::FunctionTemplate::New(isolate_, Load));
    global->Set(v8::String::NewFromUtf8(isolate_, "quit"), v8::FunctionTemplate::New(isolate_, Quit));
    global->Set(v8::String::NewFromUtf8(isolate_, "version"), v8::FunctionTemplate::New(isolate_, Version));

    /*global->Set(
        v8::String::NewFromUtf8(isolate_, "Http")
        , Http::Template::New(isolate_)
        , Http::kAttribute);*/


    v8::Local<v8::Context> context = v8::Context::New(isolate_, NULL, global);

    
    
    context->Enter();



    BindSample(context);
    /*{
        v8::HandleScope handle_scope(context->GetIsolate());
        context->Global()->Set(
            v8::String::NewFromUtf8(isolate_, "http")
            , Http::NewInstance(isolate_)
            , Http::kAttribute);
    }*/

    context_ = context;




    
    




    DeviceRef device(io_service_, rootpath);

    service0_ = ServiceRef(device);
    service1_ = ServiceRef(device);
    service2_ = ServiceRef(device);

    servers_.Create(81, boost::bind(&Service::HttpRequest, service0_.Get(), _1, _2));
    servers_.Create(82, boost::bind(&Service::HttpRequest, service1_.Get(), _1, _2));
    servers_.Create(83, boost::bind(&Service::HttpRequest, service2_.Get(), _1, _2));





    




    service_manager_ = new ServiceManager(io_service_, storage_);

    net_manager_ = new NetworkManager(io_service_->IO());

    NetworkAdapter::Ptr eth_adapter(new TcpAdapter(io_service_->IO(), this, 8070));
    net_manager_->Register("ethernet", eth_adapter);
    net_manager_->Start();

    net_manager_->RegisterSsdpTarget("ethernet", "a");
    net_manager_->RegisterSsdpTarget("ethernet", "b");
    net_manager_->RegisterSsdpTarget("ethernet", "c");
}

LocalBox::~LocalBox(void) {

    context_->Exit();

    servers_.Destroy(83);
    servers_.Destroy(82);
    servers_.Destroy(81);

    if (service_manager_) {
        delete service_manager_;
    }
    if (net_manager_) {
        net_manager_->Stop();
        delete net_manager_;
    }
}

void LocalBox::RunShell(void) {
    //v8::HandleScope handle_scope(isolate_);
    //::RunShell(context_);
    service0_->RunShell();
    //service1_->RunShell();
}





void LocalBox::OnConnected(const std::string& json, Channel* channel) {

    Json::Value root;
    Json::Reader reader;
    reader.parse(json.c_str(), root);

    const std::string& id = root["id"].asString();
    printf(" id: %s\n device: %s\n nickname: %s\n protocol: %s\n\n"
        , id.c_str()
        , root["device"].asCString()
        , root["nickname"].asCString()
        , root["protocol"].asCString());

    /*ServiceBroker* s = service_factory_[id];
    // TODO(ghilbut):
    // I'm worryed about channel's life sycle
    {
        // TODO(ghilbut):
        // if service proxy created here bind handlers
        // or, it is not necessary.
        // s->BindCommonPathHandler(boost::bind(&CtrlPoint::handle_get_common_path, this, _1, _2));
        // s->BindDisconnectedHandler(boost::bind(&CtrlPoint::handle_disconnected, this, _1));
    }
    s->BindChannel(channel);

    connecting_list_.insert(id);
    net_manager_.UnregisterSsdpTarget("ethernet", id);

    {
        Json::Value root(Json::objectValue);
        root["event"] = "ServiceAdded";
        root["id"] = id;

        Json::FastWriter writer;
        const std::string json = writer.write(root);
        main_ui_service_.FireEvent(json);
    }*/
}

void LocalBox::OnResponse(const std::string& json) {
    BOOST_ASSERT(false);
}

void LocalBox::OnEvent(const std::string& text) {
    BOOST_ASSERT(false);
}

void LocalBox::OnDisconnected(void) {
    BOOST_ASSERT(false);
}
