#include "brush.h"

#pragma once

enum SELECTION_TYPES : u8 {
	LAYER_VERTICES,
	LAYER_EDGES,
	LAYER_FACES
};

typedef struct {
	char *name;	

	u32 num_brushes;
	u32 cap_brushes;
	Brush *brushes;

	i64 selected_brush;

	u8 selection_type;
	
} Map;

Map MapInitNew();
void MapClose(Map *map);

void MapUpdate(Map *map, float dt);
void MapDraw(Map *map, Camera3D camera, float alpha);

