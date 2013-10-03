#include "device_info.h"
#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <cstdio>


const DeviceInfo* DeviceInfo::Create(const std::string& basepath) {

    std::string uipath = (boost::filesystem::path(basepath) / "ui").string();
    if (!boost::filesystem::is_directory(uipath)) {
        return NULL;
    }

    std::string filepath = (boost::filesystem::path(basepath) / "info.txt").string();

    FILE* f = fopen(filepath.c_str(), "r");
    if (f == NULL) {
        return NULL;
    }

    std::string json;
    char buffer[1024];
    size_t read_bytes = 0;
    do {
        read_bytes = fread(buffer, 1, 1024, f);
        json.append(buffer, read_bytes);
    } while (read_bytes == 1024);
    fclose(f);

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(json, root, false)) {
        return NULL;
    }

    const std::string& id = root["id"].asString();
    const std::string& name = root["name"].asString();
    const std::string& model = root["model"].asString();

    // TODO(jh81.kim): check validation of data

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
