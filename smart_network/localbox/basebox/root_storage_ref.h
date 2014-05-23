#ifndef ROOT_STORAGE_REF_H_
#define ROOT_STORAGE_REF_H_

#include "base/reference.h"


class RootStorageRef : public Reference<RootStorage> {
public:
    explicit RootStorageRef(const std::string& rootpath);
};

#endif  // ROOT_STORAGE_REF_H_