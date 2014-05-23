#include "localbox.h"
#include <v8.h>

#include <boost/filesystem.hpp>


int main(const int argc, const char* argv[]) {

    if (argc < 2) {
        return -1;
    }

    v8::V8::InitializeICU();
    int c = 2;
    char* v[] = {"", "--expose_gc"};
    v8::V8::SetFlagsFromCommandLine(&c, v, true);

    LocalBox box(argv[1]);
    box.RunShell();

    v8::V8::Dispose();

    return 0;
}