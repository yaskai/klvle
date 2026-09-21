#include <stdlib.h>
#include "cam.h"
#include "raylib.h"
#include "raymath.h"
#include "map.h"
#include "app.h"
#include "ui.h"
#include "brush.h"
#include "rlgl.h"

#define CLIP_NEAR 	2.0
#define CLIP_FAR 	40000.0

void App::Init() {
	pMap = NULL;

	ww = 1920;
	wh = 1080;

	accumulator = 0.0f;
	running = true;

	InitWindow(ww, wh, "klvle");
	SetWindowState(FLAG_VSYNC_HINT);

	rlSetClipPlanes(CLIP_NEAR, CLIP_FAR);
	//rlDisableBackfaceCulling();
	rlSetLineWidth(4.0f);

	font = LoadFont("resources/fonts/zed_mono.ttf");
	ui_Init(font);

	camera = (Camera3D) {
		.position = (Vector3) { 0, 0, 0 },
		.target = (Vector3) { 1, 0, 0 },
		.up = (Vector3) { 0, 0, 1 },
		.fovy = 90.0f,
		.projection = CAMERA_PERSPECTIVE
	};

	cc = cc_Init(&camera);

	LoadBrushGraphics();
}

void App::Close() {
	running = false;
	if(pMap) MapClose(pMap);
	CloseWindow();
}

void App::Update() {
	accumulator += GetFrameTime();

	while(accumulator >= TICK_RATE) {
		Tick();
		accumulator -= TICK_RATE;
	}

	float alpha = accumulator / TICK_RATE;
	Draw(alpha);

	if(pMap) MapUpdate(pMap, TICK_RATE);
}

void App::Draw(float alpha) {
	BeginDrawing();
	ClearBackground(RAYWHITE);

	if(pMap) { 
		MapDraw(pMap, camera, alpha); 

		cc_Point(&cc, TICK_RATE);
		cc_Resolve(&cc, alpha);
	}

	Bar();

	EndDrawing();
}

void App::Tick() {
	if(!pMap) return;

	cc_Zoom(&cc, TICK_RATE);
	cc_Fly(&cc,  TICK_RATE);
	cc_Pan(&cc,  TICK_RATE);
}

