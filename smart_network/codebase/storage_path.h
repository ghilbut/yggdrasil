#ifndef STORAGE_PATH_H_
#define STORAGE_PATH_H_

#include <string>


class StoragePath {
public:
    StoragePath(const std::string& basepath);
    ~StoragePath(void);

    const std::string& device_root(void) const;
    const std::string& common_root(void) const;
    const std::string& main_ui_root(void) const;
    const std::string& service_root(void) const;

private:
    const std::string device_root_;
    const std::string common_root_;
    const std::string main_ui_root_;
    const std::string service_root_;
};

#endif  // STORAGE_PATH_H_