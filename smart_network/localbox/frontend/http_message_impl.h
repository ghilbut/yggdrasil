#ifndef HTTP_MESSAGE_IMPL_H_
#define HTTP_MESSAGE_IMPL_H_

#include "base/ref_implement.h"


namespace Http {


class Message::Impl : public RefImplement {
public:
    Impl(void);
    Impl(const char* data, size_t data_len);
    Impl(const Message& other);
    ~Impl(void);

    const char* data(void) const;
    size_t data_len(void) const;


private:
    const char* data_;
    const size_t data_len_;
};

}  // namespace Http

#endif  // HTTP_MESSAGE_H_