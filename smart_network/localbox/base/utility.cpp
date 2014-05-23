#include "utility.h"
#include <cassert>
#include <cstdio>

bool ReadText(const char* filepath, std::string& text) {

    FILE* f = fopen(filepath, "r");
    if (!f) {
        printf("[ERROR] couldn't open below file.\n  - %s\n", filepath);
        return false;
    }

    size_t size;
    char buf[1024];
    text.clear();
    do {
        size = fread(buf, 1, 1024, f);
        text.append(buf, buf+size);
    } while (size == 1024);
    fclose(f);

    return true;
}

bool WriteText(const char* filepath, const std::string& text) {

    const char* data = &text[0];
    const size_t data_length = text.length();

    assert(data_length > 0);
    if (data_length < 1) {
        printf("[ERROR] there is no json data to write in below file.\n  - %s\n", filepath);
        return false;
    }

    FILE* f = fopen(filepath, "w");
    if (!f) {
        printf("[ERROR] couldn't open below file.\n  - %s\n", filepath);
        return false;
    }

    const size_t written_bytes = fwrite(data, 1, data_length, f);
    fclose(f);

    assert(written_bytes == data_length);
    if (written_bytes != data_length) {
        printf("[ERROR] couldn't write json data correctly in below file.\n  - %s\n", filepath);
        return false;
    }

    return true;
}

bool ReadJson(const std::string& json, Json::Value& root) {

    Json::Reader reader;
    if (!reader.parse(json, root, false)) {
        //printf("[ERROR] couldn't read json data of below file.\n  - %s\n", filepath);
        printf("[ERROR] couldn't read json data.\n");
        return false;
    }

    return true;
}

bool WriteJson(const Json::Value& root, std::string& json, bool pretty) {
    assert(root.isObject());
    if (!root.isObject()) {
        // printf("[ERROR] it is not object type which json data to read below file.\n  - %s\n", filepath);
        printf("[ERROR] it is not object type which json data to read.\n");
        return false;
    }

    static Json::FastWriter fwriter;
    static Json::StyledWriter swriter;
    Json::Writer& writer = (pretty ? static_cast<Json::Writer&>(swriter) : static_cast<Json::Writer&>(fwriter));
    json = writer.write(root);

    return true;
}