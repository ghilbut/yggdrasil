#include "message.h"
#include "codebase/utility.h"
#include <boost/assert.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>


Message::Message(Type type) 
    : type_(type), params_(Json::objectValue) {
    // nothing
}

Message::~Message(void) {
    // nothing
}

bool Message::ToJson(Json::Value& root) const {
    BOOST_ASSERT(root.isObject());
    if (!root.isObject()) {
        return false;
    }

    root["type"] = type_;

    if (!params_.empty()) {
        Json::Value params(Json::objectValue);
        Json::ValueConstIterator itr = params_.begin();
        Json::ValueConstIterator end = params_.end();
        for (; itr != end; ++itr) {
            params[itr.key().asString()] = *itr;
        }
        root["params"] = params;
    }

    return true;
}

bool Message::SetParam(const std::string& key, const std::string& value) {
    if (params_.isMember(key)) {
        return false;
    }
    params_[key] = value;
    return true;
}

bool Message::SetParam(const std::string& key, bool value) {
    if (params_.isMember(key)) {
        return false;
    }
    params_[key] = value;
    return true;
}

bool Message::SetParam(const std::string& key, int value) {
    if (params_.isMember(key)) {
        return false;
    }
    params_[key] = value;
    return true;
}

bool Message::GetParam(const std::string& key, std::string& value) const {
    if (!params_.isMember(key)) {
        return false;
    }
    const Json::Value& param = params_[key];
    if (!param.isString()) {
        return false;
    }
    value = params_[key].asString();
    return true;
}

bool Message::GetParam(const std::string& key, int& value) const {
    if (!params_.isMember(key)) {
        return false;
    }
    const Json::Value& param = params_[key];
    if (!param.isInt()) {
        return false;
    }
    value = params_[key].asInt();
    return true;
}

bool Message::RemoveParam(const std::string& key) {
    if (!params_.isMember(key)) {
        return false;
    }
    params_.removeMember(key);
    return true;
}

Message::Message(Type type, const Message& msg) 
    : type_(type), params_(msg.params_) {
    // nothing
}



const Request::Ptr Request::FromJson(const std::string& json) {

    Json::Value root(Json::objectValue);
    if (!::ReadJson(json, root)) {
        return 0;
    }
    if (!root["query"].isString()) {
        return 0;
    }
    if (!root["id"].isString()) {
        return 0;
    }

    const Json::Value object(Json::objectValue);
    const Json::Value& header = root["header"];
    const Json::Value& params = root["params"];

    Ptr req(new Request(root["query"].asString()));
    req->msg_id_ = root["id"].asString();
    req->header_ = (header.isObject() ? header : object);
    req->params_ = (params.isObject() ? params : object);
    return req;
}

Request::Request(const std::string& query)
    : Message(Message::kRequest)
    , msg_id_(to_string(boost::uuids::random_generator()()))
    , query_(query)
    , header_(Json::objectValue) {
    // nothing
}

Request::~Request(void) {
    // nothing
}

bool Request::ToJson(std::string& json) const {

    Json::Value root(Json::objectValue);
    if (!Message::ToJson(root)) {
        return false;
    }

    root["id"] = msg_id_;
    root["query"] = query_;

    if (!header_.empty()) {
        Json::Value header(Json::objectValue);
        Json::ValueConstIterator itr = header_.begin();
        Json::ValueConstIterator end = header_.end();
        for (; itr != end; ++itr) {
            header[itr.key().asString()] = *itr;
        }
        root["header"] = header;
    }
    
    return ::WriteJson(root, json, false);
}

bool Request::SetHeader(const std::string& key, const std::string& value) {
    if (header_.isMember(key)) {
        return false;
    }
    header_[key] = value;
    return true;
}

bool Request::GetHeader(const std::string& key, std::string& value) const {
    if (!header_.isMember(key)) {
        return false;
    }
    const Json::Value& param = params_[key];
    if (!header_.isString()) {
        return false;
    }
    value = header_[key].asString();
    return true;
}

bool Request::RemoveHeader(const std::string& key) {
    if (!header_.isMember(key)) {
        return false;
    }
    header_.removeMember(key);
    return true;
}

Request::Request(Type type, const Request& req)
    : Message(type, req)
    , msg_id_(req.msg_id_)
    , query_(req.query_)
    , header_(req.header_) {
    // nothing
}



Response::Response(const Request& req)
    : Request(Message::kResponse, req) {
    // nothing
}

Response::~Response(void) {
    // nothing
}



Event::Event(const std::string& name, Message::Type type)
    : Message(type), name_(name) {
    // nothing
}

Event::~Event(void) {
    // nothing
}

bool Event::ToJson(std::string& json) const {

    Json::Value root(Json::objectValue);
    if (!Message::ToJson(root)) {
        return false;
    }

    root["name"] = name_;

    return ::WriteJson(root, json, false);
}