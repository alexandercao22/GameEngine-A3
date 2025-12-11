#include "Scene.h"

int main() 
{
	Scene scene;
	scene.Init();

	while (!WindowShouldClose()) {
		//interface.Update();

		scene.Update();
		scene.RenderUpdate();
	}

	CloseWindow();

	return 0;
}