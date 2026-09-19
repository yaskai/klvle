#include "raylib.h"

#pragma once

typedef struct {
	Vector3 pos_curr;
	Vector3 pos_prev;
	Vector3 forward;

	Camera3D *cam;

	float pitch, yaw, roll;
	float zoom_vel;
	float speed;

} CamController;

CamController cc_Init(Camera3D *cam);

void cc_Resolve(CamController *cc, float alpha);

void cc_Point(CamController *cc, float dt);
void cc_Zoom(CamController *cc, float dt);
void cc_Fly(CamController *cc, float dt);
void cc_Pan(CamController *cc, float dt);

