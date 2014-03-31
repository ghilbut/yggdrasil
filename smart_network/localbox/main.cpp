#include "storage.h"
#include "service_broker.h"

#include <v8.h>


int main(const int argc, const char* argv[]) {

    if (argc < 2) {
        return -1;
    }

    v8::V8::InitializeICU();
    int c = 2;
    char* v[] = {"", "--expose_gc"};
    v8::V8::SetFlagsFromCommandLine(&c, v, true);

    Storage s(argv[1]);

    {
        ServiceBroker service(s);
        service.RunShell();
    }
    v8::V8::Dispose();

    return 0;
}