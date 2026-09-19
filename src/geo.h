#pragma once

#include <float.h>
#include "raylib.h"
#include "raymath.h"
#include "nums.h"

#define WORLD_UP 		(Vector3) {  0,  0,  1 }
#define WORLD_DOWN 		(Vector3) {  0,  0, -1 }

// Plane primitive
typedef struct {
	Vector3 normal;
	float distance;

} Plane;

float PlaneDistance(Vector3 point, Plane plane);
u8 IntersectSegmentPlane(Vector3 a, Vector3 b, Plane plane, float *t, Vector3 *point);

bool ThreePlaneIntersect(Plane a, Plane b, Plane c, Vector3 *v);

Plane *BoxToPlanes(BoundingBox aabb);

// Triangle primitive
typedef struct {
	Vector3 vertices[3];
	Vector3 normal;
	
} Tri;

Vector3 BoxExtents(BoundingBox aabb);
Vector3 BoxCenter(BoundingBox aabb);

BoundingBox BoxEmpty();
BoundingBox BoxRecenter(BoundingBox aabb, Vector3 point);
BoundingBox BoxTranslate(BoundingBox aabb, Vector3 v);
BoundingBox BoxExpandToPoint(BoundingBox aabb, Vector3 point);
float BoxSurfaceArea(BoundingBox aabb);

// Collection of the eight vertices that make a box
typedef struct { Vector3 v[8]; } BoxPoints;
BoxPoints BoxGetPoints(BoundingBox aabb);

// Collection of six normals for each face of a box (always the same but sometimes helpful shorthand) 
typedef struct { Vector3 n[6]; } BoxNormals;
BoxNormals BoxGetNormals();

typedef struct {
	Vector3 p1;	
	Vector3 p2;

} Edge;


