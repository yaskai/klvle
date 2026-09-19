#include "raylib.h"
#include "app.h"

int main() {
	App app; 
	App* pApp = &app;
	pApp->Init();

	while(pApp->running ^ WindowShouldClose()) {
		pApp->Update();
	}

	pApp->Close();
	return 0;
}

