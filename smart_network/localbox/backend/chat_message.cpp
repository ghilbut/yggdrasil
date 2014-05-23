#include "chat_message.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>


chat_message::chat_message()
    : body_length_(0)
{
}

const char* chat_message::data() const
{
    return data_;
}

char* chat_message::data()
{
    return data_;
}

size_t chat_message::length() const
{
    return header_length + type_length + body_length_;
}

const char* chat_message::body() const
{
    return data_ + header_length + type_length;
}

char* chat_message::body()
{
    return data_ + header_length + type_length;
}

size_t chat_message::body_length() const
{
    return body_length_;
}

void chat_message::body_length(size_t new_length)
{
    body_length_ = new_length;
    if (body_length_ > max_body_length)
        body_length_ = max_body_length;
}

bool chat_message::decode_header()
{
    using namespace std; // For strncat and atoi.
    char header[header_length + 1] = "";
    strncat(header, data_, header_length);
    body_length_ = atoi(header);
    if (body_length_ > max_body_length)
    {
        body_length_ = 0;
        return false;
    }
    return true;
}

void chat_message::encode_header()
{
    using namespace std; // For sprintf and memcpy.
    char header[header_length + 1] = "";
    sprintf(header, "%4d", body_length_);
    memcpy(data_, header, header_length);
}

chat_message::Type chat_message::type(void) const {
    return static_cast<Type>(data_[header_length]);
}

void chat_message::type(const Type type) {
    data_[header_length] = type;
}