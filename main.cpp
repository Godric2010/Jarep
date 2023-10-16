#include <iostream>
#include "core.hpp"

int main() {

	auto core = Core::CoreManager();
	core.Initialize();
	core.Run();
	core.Shutdown();
    return 0;
}
