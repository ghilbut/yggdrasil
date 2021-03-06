#ifndef UTILITY_H_
#define UTILITY_H_

#include <json/json.h>

bool ReadText(const char* filepath, std::string& text);
bool WriteText(const char* filepath, const std::string& text);

bool ReadJson(const std::string& json, Json::Value& root);
bool WriteJson(const Json::Value& root, std::string& json, bool pretty = false);

#endif  // UTILITY_H_