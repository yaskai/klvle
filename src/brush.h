#include "raylib.h"
#include "geo.h"
#include "nums.h"

#pragma once

void LoadBrushGraphics();

typedef struct {

} Brush_Edge;

typedef struct {
	Tri tris[8];
	u8 num_tris;

	u8 plane;

	u8 num_vertices;	
	u8 vertices[8];

} Brush_Face;

typedef struct {
	Model model;	
	Mesh mesh;

	BoundingBox bounds;

	u8 num_vertices;
	Vector3 *vertices;

	u8 num_planes;
	Plane *planes;

	u8 num_faces;
	Brush_Face *faces;

} Brush;

Brush BrushInitCube(Vector3 position, Vector3 size);

void BrushBuildVertices(Brush *brush);
void BrushBuildFaces(Brush *brush);
void BrushBuildMesh(Brush *brush);

#define F_BRUSH_DRAW_IS_SELECTED	0x01
#define F_BRUSH_DRAW_WIRES			0x02
#define F_BRUSH_DRAW_VERTICES		0x04
#define F_BRUSH_DRAW_EDGES			0x08
#define F_BRUSH_DRAW_FACES			0x10
#define F_BRUSH_DRAW_AABB			0x20
void BrushDraw(Brush *brush, u8 flags);

void BrushDrawVertices(Brush *brush);
