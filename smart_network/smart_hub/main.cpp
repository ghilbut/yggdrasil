#include "ctrl_point.h"
#include "http/http_server.h"
#include "service/service_pool.h"
#include "codebase/device/device_pool.h"
#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include <cstdio>


int main(int argc, const char** argv)
{
    if (argc < 2) {
        printf("Usage: %s <storage>\n", argv[0]);
        return -1;
    }

    const char* basepath = argv[1];
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