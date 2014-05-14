#ifndef ENVIRON_H_
#define ENVIRON_H_

#include "storage.h"
#include "template_factory.h"
#include <v8.h>
#include <boost/asio.hpp>


class Environ {
public:
    Environ(boost::asio::io_service& io_service
        , const char* basepath
        , int port);
    ~Environ(void);

    boost::asio::io_service& io_service(void) const;
    v8::Isolate* isolate(void) const;
    TemplateFactory& template_factory(void) const;
    Storage& storage(void) const;
    int port(void) const;

    template <typename F>
    inline void Post(const F& handler) {
        strand_.post(handler);
    }

private:
    boost::asio::io_service& io_service_;
    boost::asio::strand strand_;

    v8::Isolate* isolate_;
    v8::Isolate::Scope isolate_scope_;
    v8::HandleScope handle_scope_;
    mutable TemplateFactory template_factory_;
    mutable Storage storage_;
    const int port_;
};

#endif  // ENVIRON_H_