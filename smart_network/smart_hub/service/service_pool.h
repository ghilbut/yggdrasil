#ifndef SERVICE_POOL_H_
#define SERVICE_POOL_H_

#include <boost/shared_ptr.hpp>
#include <map>
#include <stdint.h>


class HttpServer;
class DevicePool;
class Service;

class ServicePool {
public:
    typedef std::map<std::string, Service*> ServiceMap;
    typedef ServiceMap::iterator Iterator;


    ServicePool(HttpServer& httpd
                , const DevicePool& devices
                , const std::string& rootpath);
    ~ServicePool(void);

    void Start(void);
    void Stop(void);

    Iterator Begin(void);
    Iterator End(void);

    Service* operator[] (const char* id) const;


private:
    HttpServer& httpd_;
    const DevicePool& devices_;

    ServiceMap services_;

    const std::string rootpath_;
    uint32_t next_port_;
};


#endif  // SERVICE_POOL_H_