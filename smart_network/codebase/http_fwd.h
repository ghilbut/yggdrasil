#ifndef HTTP_FWD_H_
#define HTTP_FWD_H_


// common static files
static const char* kCommonKeyword = "/common/";
static const size_t kCommonKeySize = strlen(kCommonKeyword);
// local static files
static const char* kStaticKeyword = "/static/";
static const size_t kStaticKeySize = strlen(kStaticKeyword);
// forward command over channel
static const char* kRequestKeyword = "/do/";
static const size_t kRequestKeySize = strlen(kRequestKeyword);
// execute internal script
static const char* kScriptKeyword = "/x/";
static const size_t kScriptKeySize = strlen(kScriptKeyword);


#endif  // HTTP_FWD_H_