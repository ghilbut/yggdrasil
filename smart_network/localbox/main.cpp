//#include "http_object.h"
#include "service_broker.h"


int main(const int argc, const char* argv[]) {

    //Http::Object obj;
    //obj.Start();

    ServiceBroker b;
    b.Start();

    while (true)
    {
    }

    b.Stop();

    //obj.Stop();
    return 0;
}