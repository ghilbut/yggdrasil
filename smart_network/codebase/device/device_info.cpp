#include "device_info.h"
#include "utility.h"
#include "boost_lib_fwd.h"
#include <json/reader.h>
#include <cstdio>


const DeviceInfo* DeviceInfo::Create(const std::string& basepath) {

    const std::string uipath = (boost::filesystem::path(basepath) / "ui").string();
    if (!boost::filesystem::is_directory(uipath)) {
        return 0;
    }

    const std::string filepath = (boost::filesystem::path(basepath) / "info.txt").string();

    std::string json;
    if (!::ReadText(filepath.c_str(), json)) {
        // TODO(ghilbut): error message
        return 0;
    }

    Json::Value root;
    if (!::ReadJson(json, root)) {
        // TODO(ghilbut): error message
        return 0;
    }

    const std::string& id = root["id"].asString();
    const std::string& name = root["name"].asString();
    const std::string& model = root["model"].asString();

    // TODO(ghilbut): check validation of data

    const DeviceInfo* info = new DeviceInfo(uipath, id, name, model);
    return info;
}

void DeviceInfo::Delete(const DeviceInfo* info) {
    delete info;
}

const char* DeviceInfo::uipath(void) const {
    return uipath_.c_str();
}

const char* DeviceInfo::id(void) const {
    return id_.c_str();
}

const char* DeviceInfo::name(void) const {
    return name_.c_str();
}

const char* DeviceInfo::model(void) const {
    return model_.c_str();
}

DeviceInfo::DeviceInfo(const std::string& uipath
                       , const std::string& id
                       , const std::string& name
                       , const std::string& model)
    : uipath_(uipath)
    , id_(id)
    , name_(name)
    , model_(model) {

}

DeviceInfo::~DeviceInfo(void) {

}
