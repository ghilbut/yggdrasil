#include "root_storage.h"
#include "root_storage_ref.h"
#include <boost/filesystem.hpp>


RootStorage::RootStorage(const std::string& root)
    : root_(boost::filesystem::canonical(root).generic_string())
    , common_(boost::filesystem::canonical("common", root).generic_string())
    , main_(boost::filesystem::canonical("main", root).generic_string())
    , devices_(boost::filesystem::canonical("devices", root).generic_string())
    , services_(boost::filesystem::canonical("services", root).generic_string()) {
    // nothing
}

RootStorage::~RootStorage(void) {
    // nothing
}

const char* RootStorage::common(void) const {
    return common_.c_str();
}

const char* RootStorage::main(void) const {
    return main_.c_str();
}

const char* RootStorage::device_base(void) const {
    return devices_.c_str();
}

const char* RootStorage::service_base(void) const {
    return services_.c_str();
}



RootStorageRef::RootStorageRef(const std::string& rootpath) {
    Reset(new RootStorage(rootpath));
}
