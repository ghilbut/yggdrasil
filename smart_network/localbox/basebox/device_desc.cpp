#include "device_desc.h"
#include "base/utility.h"
#include <boost/filesystem.hpp>
#include <json/reader.h>
#include <cstdio>


const DeviceDesc* DeviceDesc::Create(const std::string& basepath) {

    const std::string uiroot = (boost::filesystem::path(basepath) / "ui").string();
    if (!boost::filesystem::is_directory(uiroot)) {
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

    const DeviceDesc* info = new DeviceDesc(uiroot, id, name, model);
    return info;
}

void DeviceDesc::Delete(const DeviceDesc* info) {
    delete info;
}

const char* DeviceDesc::uiroot(void) const {
    return uipath_.c_str();
}

const char* DeviceDesc::id(void) const {
    return id_.c_str();
}

const char* DeviceDesc::name(void) const {
    return name_.c_str();
}

const char* DeviceDesc::model(void) const {
    return model_.c_str();
}

DeviceDesc::DeviceDesc(const std::string& uiroot
                       , const std::string& id
                       , const std::string& name
                       , const std::string& model)
    : uipath_(uiroot)
    , id_(id)
    , name_(name)
    , model_(model) {

}

DeviceDesc::~DeviceDesc(void) {

}
