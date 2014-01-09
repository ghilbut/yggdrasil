#include "codebase/storage_path.h"
#include "ctrl_point.h"
#include <string>
#include <iostream>
#include <cstdio>


int main(int argc, const char** argv)
{
    if (argc < 2) {
        printf("Usage: %s <storage>\n", argv[0]);
        return -1;
    }

    const char* storage_root = argv[1];
    StoragePath storage(storage_root);
    CtrlPoint cp(storage);
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