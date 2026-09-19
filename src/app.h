#pragma once

#include "raylib.h"
#include "nums.h"
#include "map.h"
#include "cam.h"

#define TICK_RATE (1.0f / 66.0f)

class App {
public:
	Map map_buffer;
	Map *pMap;

	CamController cc;

	int ww, wh;	
	bool running;

	Camera3D camera;

	void Init();
	void Close();

	void Update();
	void Draw(float alpha);

	void Tick();

private:
	Font font;
	float accumulator;

	void Bar();
};

