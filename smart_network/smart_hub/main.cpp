#include <stdio.h>
#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include <json/reader.h>
#include "http/http_server.h"
#include "device/device_pool.h"
#include "service/service_pool.h"
#include "ctrl_point.h"


int main(int argc, const char** argv)
{
	//if (argc < 2) {
	//	return -1;
	//}

    //const char* basepath = argv[1];
    //const char* basepath = "E:\\workbench\\project\\yggdrasil\\smart_network\\sample_storage";
    const char* basepath = "C:\\work\\project\\yggdrasil\\smart_network\\sample_storage";
    boost::filesystem::path root(basepath);

    const std::string common = (root / "common").string();
    HttpServer httpd(common);

    DevicePool devices((root / "devices").string());
    ServicePool services(httpd, devices, (root / "services").string());

    const std::string home = (root / "main").string();
    CtrlPoint cp(home, httpd, devices, services);
    if (!cp.Start()) {
        printf("[MAIN] ctrl-point start failed.\n");
        return -1;
    }

	bool stop = false;
	while (!stop) {
		std::string cmd;
		std::cin >> cmd;
		if (cmd == "q" || cmd == "Q") {
            stop = true;
        } else {
            std::cout << cmd << std::endl;

        }
	}
    cp.Stop();

	return 0;
}