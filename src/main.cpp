#include "raylib.h"
#include "app.h"

int main() {
	SetTraceLogLevel(LOG_ERROR);

	App app; 
	App* pApp = &app;
	pApp->Init();

	while(pApp->running ^ WindowShouldClose()) {
		pApp->Update();
	}

	pApp->Close();
	return 0;
}

