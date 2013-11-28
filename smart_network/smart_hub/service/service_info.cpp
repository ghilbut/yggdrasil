#include "service_info.h"
#include "codebase/boost_lib_fwd.h"
#include <json/reader.h>
#include <cstdio>


ServiceInfo* ServiceInfo::Create(const std::string& filepath) {

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
    const std::string& device = root["device"].asString();
    const std::string& nickname = root["nickname"].asString();

    // TODO(jh81.kim): check validation of data

    ServiceInfo* info = new ServiceInfo(filepath, id, device, nickname);
    return info;
}

void ServiceInfo::Delete(ServiceInfo* info) {
    delete info;
}

const char* ServiceInfo::id(void) const {
    return id_.c_str();
}

const char* ServiceInfo::device(void) const {
    return device_.c_str();
}

const char* ServiceInfo::nickname(void) const {
    return nickname_.c_str();
}

ServiceInfo::ServiceInfo(const std::string& infopath
                       , const std::string& id
                       , const std::string& device
                       , const std::string& nickname)
    : infopath_(infopath)
    , id_(id)
    , device_(device)
    , nickname_(nickname) {

}

ServiceInfo::~ServiceInfo(void) {
    // open file to read
    FILE* f = fopen(infopath_.c_str(), "r");
    if (f == NULL) {
        // TODO(jh81.kim): error logging
        return;
    }

    // read json string
    std::string json;
    char buffer[1024];
    size_t read_bytes = 0;
    do {
        read_bytes = fread(buffer, 1, 1024, f);
        json.append(buffer, read_bytes);
    } while (read_bytes == 1024);
    fclose(f);

    // parse to json object
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(json, root, false)) {
        return;
    }

    // open file to write
    f = fopen(infopath_.c_str(), "w");
    if (f == NULL) {
        // TODO(jh81.kim): error logging
        return;
    }

    // update service information
    root["nickname"] = nickname_;

    // write json string
    fwrite(json.c_str(), 1, json.length(), f);
    fclose(f);
}
