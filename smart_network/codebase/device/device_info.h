#ifndef DEVICE_INFO_H_
#define DEVICE_INFO_H_

#include <string>


class DeviceInfo {
public:
    static const DeviceInfo* Create(const std::string& basepath);
    static void Delete(const DeviceInfo* info);

    const char* uipath(void) const;
    const char* id(void) const;
    const char* name(void) const;
    const char* model(void) const;


private:
    DeviceInfo(const std::string& uipath
               , const std::string& id
               , const std::string& name
               , const std::string& model);
    ~DeviceInfo(void);


private:
    const std::string uipath_;
    const std::string id_;
    const std::string name_;
    const std::string model_;
};


#endif  // DEVICE_INFO_H_