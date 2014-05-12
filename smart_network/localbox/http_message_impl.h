#ifndef HTTP_MESSAGE_IMPL_H_
#define HTTP_MESSAGE_IMPL_H_

#include <boost/atomic.hpp>


namespace Http {


class Message::Impl {
public:
    Impl(void);
    Impl(const char* data, size_t data_len);
    Impl(const Message& other);
    ~Impl(void);

    void AddRef(void) const;
    void Release(void) const;

    const char* data(void) const;
    size_t data_len(void) const;


private:
    mutable boost::atomic_int ref_count_;
    const char* data_;
    const size_t data_len_;
};

}  // namespace Http

#endif  // HTTP_MESSAGE_H_