#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <json/json.h>
#include <boost/shared_ptr.hpp>
#include <string>


class Request;
class Response;
class Event;

class Message {
public:
    typedef enum {
        kConnect,
        kDisconnect,
        kRequest,
        kResponse,
        kEvent,
    } Type;

public:
    explicit Message(Type type);
    ~Message(void);

    virtual bool ToJson(Json::Value& root) const;
    virtual bool ToJson(std::string& json) const = 0;

    bool SetParam(const std::string& key, const std::string& value);
    bool SetParam(const std::string& key, bool value);
    bool SetParam(const std::string& key, int value);
    bool GetParam(const std::string& key, std::string& value) const;
    bool GetParam(const std::string& key, int& value) const;
    bool RemoveParam(const std::string& key);


protected:
    Message(Type type, const Message& msg);
protected:
    const Type type_;
    Json::Value params_;
};



class Request : public Message {
public:
    typedef boost::shared_ptr<Request> Ptr;
    static const Ptr FromJson(const std::string& json);


public:
    explicit Request(const std::string& query);
    ~Request(void);

    virtual bool ToJson(std::string& json) const;

    bool SetHeader(const std::string& key, const std::string& value);
    bool GetHeader(const std::string& key, std::string& value) const;
    bool RemoveHeader(const std::string& key);

    const std::string& query(void) const { return query_; }


protected:
    Request(Type type, const Request& req);
protected:
    std::string msg_id_;
    std::string query_;
    Json::Value header_;
};



class Response : public Request {
public:
    explicit Response(const Request& req);
    ~Response(void);
};



class Event : public Message {
public:
    explicit Event(const std::string& name, Message::Type type = Message::kEvent);
    ~Event(void);

    bool ToJson(std::string& json) const;


private:
    const std::string name_;
};

#endif  // MESSAGE_H_