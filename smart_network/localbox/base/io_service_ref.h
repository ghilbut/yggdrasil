#ifndef IO_SERVICE_REF_H_
#define IO_SERVICE_REF_H_

#include <boost/asio.hpp>
#include <boost/thread.hpp>


class IOService;

class IOServiceRef {
public:
    IOServiceRef(void);
    explicit IOServiceRef(const IOServiceRef& other);
    ~IOServiceRef(void);

    IOServiceRef& operator= (const IOServiceRef& other);
    bool operator== (const IOServiceRef& other) const;
    bool operator!= (const IOServiceRef& other) const;

    IOService* operator-> (void) const;


private:
    IOService* impl_;
};

#endif  // IO_SERVICE_REF_H_