#ifndef SERVICE_INFO_H_
#define SERVICE_INFO_H_

#include <string>


class ServiceInfo {
public:
    static ServiceInfo* Create(const std::string& filepath);
    static void Delete(ServiceInfo* info);

    const char* id(void) const;
    const char* device(void) const;
    const char* nickname(void) const;


private:
    ServiceInfo(const std::string& infopath
                , const std::string& id
                , const std::string& device
                , const std::string& nickname);
    ~ServiceInfo(void);


private:
    const std::string infopath_;
    const std::string id_;
    const std::string device_;
    std::string nickname_;
};


#endif  // SERVICE_INFO_H_