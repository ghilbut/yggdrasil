#include "storage_path.h"
#include <boost_lib_fwd.h>


StoragePath::StoragePath(const std::string& basepath)
    : device_root_((boost::filesystem::canonical(basepath).normalize() / "devices").string())
    , common_root_((boost::filesystem::canonical(basepath).normalize() / "common").string())
    , main_ui_root_((boost::filesystem::canonical(basepath).normalize() / "main").string())
    , service_root_((boost::filesystem::canonical(basepath).normalize() / "services").string()) {
    // nothing
}

StoragePath::~StoragePath(void) {
}

const std::string& StoragePath::device_root(void) const {
    return device_root_;
}

const std::string& StoragePath::common_root(void) const {
    return common_root_;
}

const std::string& StoragePath::main_ui_root(void) const {
    return main_ui_root_;
}

const std::string& StoragePath::service_root(void) const {
    return service_root_;
}