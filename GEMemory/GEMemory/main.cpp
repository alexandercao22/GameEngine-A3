#include "TestCases.h"
#include "Interface.h"

#include "raylib.h"

#include <filesystem>

int main() {
	//Interface interface;

	//while (!WindowShouldClose()) {
	//	interface.Update();
	//}

	//CloseWindow();

	//PoolVSOS();
	//TestAll();


	// Filesystem testing

	std::string relative = "Hello.csv";
	std::filesystem::path relativePath(relative);
	std::filesystem::path p("/project/assets");

	std::filesystem::path fullPath = p / relativePath;
	std::cout << fullPath.string() << std::endl;
	

	return 0;
}