#include "GlfwOccView.h"

int main(int, char**)
{
	GlfwOccView app;
	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
    return 0;
}
