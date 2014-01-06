#ifndef SSDP_SENDER_H_
#define SSDP_SENDER_H_
// NOTE(jh81.kim): SSDP - Simple Service Discovery Protocol
#include <string>


namespace Ssdp {

class Sender {
public:
    virtual void RegistTarget(const std::string& target) = 0;
    virtual void UnregistTarget(const std::string& target) = 0;
    virtual void Send(void) const = 0;

protected:
    virtual ~Sender(void) {}
};

}  // namespace Ssdp

#endif  // SSDP_SENDER_H_