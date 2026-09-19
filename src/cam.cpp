#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include "cam.h"

CamController cc_Init(Camera3D *cam) {
	CamController cc = (CamController) {0};

	cc.cam = cam;
	cc.forward = Vector3Normalize(Vector3Subtract(cam->target, cam->position));
	cc.pos_curr = cam->position;
	cc.pos_prev = cam->position;
	cc.speed = 10.0f;

	return cc;
}

void cc_Resolve(CamController *cc, float alpha) {
	Vector3 pos = Vector3Lerp(cc->pos_prev, cc->pos_curr, alpha); 
	cc->cam->position = pos;
	cc->cam->target = Vector3Add(pos, cc->forward);
}

#define PITCH_MAX (89.0f * DEG2RAD)
void cc_Point(CamController *cc, float dt) {
	if(IsMouseButtonUp(MOUSE_RIGHT_BUTTON)) return;

	Vector2 md = Vector2Scale(GetMouseDelta(), 0.01f);
		
	cc->yaw -= md.x;	
	cc->pitch = Clamp(cc->pitch - md.y, -PITCH_MAX, PITCH_MAX);

	Vector3 look = (Vector3) {
		.x = cosf(cc->yaw) * cosf(cc->pitch),
		.y = sinf(cc->yaw) * cosf(cc->pitch),
		.z = sinf(cc->pitch)
	};

	cc->forward = look;
}

#define ZOOM_WHEEL_MIN 0.2f
void cc_Zoom(CamController *cc, float dt) {
	float wheel = GetMouseWheelMove();

	cc->zoom_vel += wheel;
	if(fabsf(cc->zoom_vel) < ZOOM_WHEEL_MIN) {
		cc->zoom_vel = 0.0f;
		return;
	}

	Vector3 targ = Vector3Add(cc->pos_curr, Vector3Scale(cc->forward, cc->zoom_vel));
	cc->pos_prev = cc->pos_curr;
	cc->pos_curr = targ;

	cc->zoom_vel = Lerp(cc->zoom_vel, 0.0f, dt*10);
}

void cc_Fly(CamController *cc, float dt) {
	Vector3 right = Vector3Normalize(Vector3CrossProduct(cc->forward, cc->cam->up));

	Vector3 move = Vector3Zero();
	if(IsKeyDown(KEY_D)) move = Vector3Add(move, Vector3Scale(right,  	    cc->speed));
	if(IsKeyDown(KEY_A)) move = Vector3Add(move, Vector3Scale(right, 	   -cc->speed));
	if(IsKeyDown(KEY_W)) move = Vector3Add(move, Vector3Scale(cc->forward,  cc->speed));
	if(IsKeyDown(KEY_S)) move = Vector3Add(move, Vector3Scale(cc->forward, -cc->speed));

	cc->pos_prev = cc->pos_curr;
	cc->pos_curr = Vector3Add(cc->pos_curr, Vector3Scale(move, dt));
}

void cc_Pan(CamController *cc, float dt) {
	if(IsMouseButtonUp(MOUSE_MIDDLE_BUTTON)) return;	

	cc->zoom_vel = 0.0f;

	Vector2 md = Vector2Scale(GetMouseDelta(), 0.1f);

	Vector3 right = Vector3Normalize(Vector3CrossProduct(cc->forward, cc->cam->up));
	Vector3 up = Vector3Normalize(Vector3CrossProduct(right, cc->forward)); 

	Vector3 move = Vector3Zero();
	move = Vector3Add(move, Vector3Scale(up, md.y));
	move = Vector3Add(move, Vector3Scale(right, md.x));

	cc->pos_prev = cc->pos_curr;
	cc->pos_curr = Vector3Add(cc->pos_curr, move);
}

