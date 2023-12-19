#include <iostream>
#include "core.hpp"

int main(int argc, char* argv[]) {

	auto core = Core::CoreManager();
	core.Initialize();
	core.Run();
	core.Shutdown();
    return 0;
}
