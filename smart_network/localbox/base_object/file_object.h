#ifndef FILE_OBJECT_H_
#define FILE_OBJECT_H_

#include <v8.h>
#include <string>


class File {
public:
    File(const char* name, FILE* fp);
    ~File(void);

    const char* name(void) const;
    long size(void) const;

    size_t ReadAsText(char* buffer, size_t buffer_len) const;

    static void WeakCallback(const v8::WeakCallbackData<v8::Object, File>& data);
    void MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self);


private:
    const char* const name_;
    FILE* const fp_;
    v8::Persistent<v8::Object> self_;
};



class FileTemplate {
public:
    static v8::Local<v8::FunctionTemplate> New(v8::Isolate* isolate);

    

private:
    static void Exists(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetName(v8::Local<v8::String> property
        , const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetSize(v8::Local<v8::String> property
        , const v8::PropertyCallbackInfo<v8::Value>& info);
    
    static void ReadAsText(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    FileTemplate(void) {}
};

#endif  // FILE_OBJECT_H_