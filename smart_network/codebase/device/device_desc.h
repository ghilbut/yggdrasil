#ifndef DEVICE_DESCRIPTION_H_
#define DEVICE_DESCRIPTION_H_

#include <string>


class DeviceDesc {
public:
    static const DeviceDesc* Create(const std::string& basepath);
    static void Delete(const DeviceDesc* info);

    const char* uiroot(void) const;
    const char* id(void) const;
    const char* name(void) const;
    const char* model(void) const;


private:
    DeviceDesc(const std::string& uiroot
               , const std::string& id
               , const std::string& name
               , const std::string& model);
    ~DeviceDesc(void);


private:
    const std::string uipath_;
    const std::string id_;
    const std::string name_;
    const std::string model_;
};


#endif  // DEVICE_DESCRIPTION_H_