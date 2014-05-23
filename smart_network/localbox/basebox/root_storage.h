#ifndef ROOT_STORAGE_H_
#define ROOT_STORAGE_H_

#include "base/ref_object.h"
#include <string>


class RootStorage : public RefObject {
public:
    RootStorage(const std::string& root);
    ~RootStorage(void);

    const char* common(void) const;
    const char* main(void) const;
    const char* device_base(void) const;
    const char* service_base(void) const;

private:
    const std::string root_;
    const std::string common_;
    const std::string main_;
    const std::string devices_;
    const std::string services_;
};

#endif  // STORAGE_H_