#ifndef SERVICE_DESC_H_
#define SERVICE_DESC_H_

#include <string>


class ServiceDesc {
public:
    static ServiceDesc* Create(const std::string& filepath);
    static void Delete(ServiceDesc* info);

    const char* id(void) const;
    const char* device(void) const;
    const char* nickname(void) const;


private:
    ServiceDesc(const std::string& infopath
                , const std::string& id
                , const std::string& device
                , const std::string& nickname);
    ~ServiceDesc(void);


private:
    const std::string infopath_;
    const std::string id_;
    const std::string device_;
    std::string nickname_;
};


#endif  // SERVICE_DESC_H_