#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_


namespace Http {

class Message {
public:
    Message(void);
    Message(const char* data, size_t data_len);
    Message(const Message& other);
    ~Message(void);

    Message& operator= (Message& other);

    const char* data(void) const;
    size_t data_len(void) const;


private:
    class Impl;
    Impl* pimpl_;
};

}  // namespace Http

#endif  // HTTP_MESSAGE_H_