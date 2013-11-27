#include "http_request.h"
#include <mongoose/mongoose.h>
#include <json/json.h>


HttpRequest::HttpRequest(Channel& channel, mg_connection* conn)
    : channel_(channel), conn_(conn) {
    // nothing
}

HttpRequest::~HttpRequest(void) {
	
}

bool HttpRequest::DoRequest(const char* id, const char* method, const char* uri) {

    boost::mutex::scoped_lock lock(mutex_);

    Json::Value params(Json::objectValue);
    // TODO(jh81.kim):
    // get parameters and make JSON object.

    if (strcmp(uri, "change/mode") == 0) {

        char post_data[1024];
        const int post_data_len = mg_read(conn_, post_data, sizeof(post_data));

        char buffer[1024];
        mg_get_var(post_data, post_data_len, "mode", buffer, sizeof(buffer));
        params["mode"] = std::atoi(buffer);
    }

    Json::Value root(Json::objectValue);
    root["id"] = id;
    root["query"] = uri;
    root["params"] = params;

    Json::FastWriter writer;
    const std::string json = writer.write(root);

    chat_message msg;
    const size_t size = json.length();
    msg.body_length(size);
    strncpy(msg.body(), json.c_str(), size);
    msg.encode_header();
    msg.type(chat_message::kRequest);
    channel_.Deliver(msg);

    // TODO(jh81.kim): apply timeout
    cond_.wait(lock);

    return true;
}

void HttpRequest::DoResponse(const std::string& json) {

    mg_printf(conn_
        //, "HTTP/1.1 %d Error (%s)\r\n"
        , "HTTP/1.1 %d %s\r\n"
          "Content-Type: application/json\r\n"
          "Content-Length: %d\r\n"
          "\r\n%s"
        //, 500 /* This the error code you want to send back*/
        , 200, "OK"
        , json.length()
        , json.c_str());

    cond_.notify_one();
}