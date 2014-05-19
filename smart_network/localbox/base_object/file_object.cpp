#include "file_object.h"

#include <boost/filesystem.hpp>


File::File(const char* name, FILE* fp)
    : name_(name)
    , fp_(fp) {
    // nothing
}

File::~File(void) {
    delete[] name_;
    fclose(fp_);
}

const char* File::name(void) const {
    return name_;
}

long File::size(void) const {
    const long cur = ftell(fp_);
    fseek(fp_, 0L, SEEK_END);
    const long size = ftell(fp_);
    fseek(fp_, cur, SEEK_SET);
    return size;
}

size_t File::ReadAsText(char* buffer, size_t buffer_len) const {
    return fread(buffer, sizeof(char), buffer_len, fp_);
}

void File::WeakCallback(const v8::WeakCallbackData<v8::Object, File>& data) {
    File* pThis = data.GetParameter();
    (pThis->self_).Reset();
    delete pThis;
}

void File::MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self) {
    v8::HandleScope handle_scope(isolate);
    self_.Reset(isolate, self);
    self->SetAlignedPointerInInternalField(0, this);
    //self_.MarkIndependent();
    self_.SetWeak(this, WeakCallback);
}



template<typename T>
static File* Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<ServiceBroker*>(ptr);
    return static_cast<File*>(object->GetAlignedPointerFromInternalField(0));
}

v8::Local<v8::FunctionTemplate> FileTemplate::New(v8::Isolate* isolate) {

    v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate, Constructor);
    ft->SetClassName(v8::String::NewFromUtf8(isolate, "File"));
    ft->Set(isolate, "exists", v8::FunctionTemplate::New(isolate, &FileTemplate::Exists));

    v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
    ot->SetInternalFieldCount(1);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "name")
        , &FileTemplate::GetName);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "size")
        , &FileTemplate::GetSize);
    ot->Set(isolate, "readAsText"
        , v8::FunctionTemplate::New(isolate, &FileTemplate::ReadAsText));

    return ft;
}


void FileTemplate::Exists(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::Local<v8::Object> arg0 = args[0]->ToObject();

    v8::Local<v8::String> str = arg0->ToString();
    char* path = new char[str->Utf8Length()+1];
    str->WriteUtf8(path);

    bool exists = boost::filesystem::exists(path);
    args.GetReturnValue().Set(exists);
    delete[] path;
}

void FileTemplate::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    if (!args.IsConstructCall()) {
        const char* msg = "Failed to construct 'File': Please use the 'new' operator, this DOM object constructor cannot be called as a function.";
        v8::Local<v8::Value> e = v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, msg));
        args.GetReturnValue().Set(isolate->ThrowException(e));
        return;
    }

    if (args.Length() < 1) {
        // TODO(ghilbut): error handling
        return;
    }

    v8::Local<v8::Value> arg0 = args[0];
    if (!arg0->IsString()) {
        // TODO(ghilbut): error handling
        return;
    }

    // TODO(ghilbut): hsould be related path
    // but, current is absolute path for test
    v8::Local<v8::String> str = arg0->ToString();
    const int size = str->Utf8Length();
    char* name = new char[size+1];
    name[str->WriteUtf8(name, size)] = '\0';
    FILE* fp = fopen(name, "r");
    if (fp == 0) {
        // TODO(ghilbut): error handling
        return;
    }

    v8::Local<v8::Object> object = args.Holder();
    File* file = new File(name, fp);
    file->MakeWeak(isolate, object);
    args.GetReturnValue().Set(object);
}

void FileTemplate::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    File* f = Unwrap(info);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), f->name()));
}

void FileTemplate::GetSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    File* f = Unwrap(info);
    info.GetReturnValue().Set(v8::Integer::New(info.GetIsolate(), f->size()));
}

void FileTemplate::ReadAsText(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    File* f = Unwrap(args);
    const long data_len = f->size();
    char* data = new char[data_len+1];
    data[f->ReadAsText(data, data_len)] = '\0';
    v8::Local<v8::String> text = v8::String::NewFromUtf8(isolate, data);
    delete[] data;

    args.GetReturnValue().Set(text);
}
