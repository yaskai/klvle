#include "brush.h"

#pragma once

enum ACTION_TYPES : u8 {
	ACTION_INSTERT,
	ACTION_DELETE,
	ACTION_MODIFY
};

typedef struct { 
	u16 object_count;
	u8 type;

} EditAction;

enum SELECTION_TYPES : u8 {
	SEL_VERTICES,
	SEL_EDGES,
	SEL_FACES
};

typedef struct {
	char *name;	

	u32 num_brushes;
	u32 cap_brushes;
	Brush *brushes;

	i64 selected_brush;

	u8 selection_tool;
	
} Map;

Map MapInitNew();
void MapClose(Map *map);

void MapUpdate(Map *map, float dt);
void MapDraw(Map *map, Camera3D camera, float alpha);

void SelectBrush(Map *map);
void SelectVertex(Map *map, u32 brush_id);
void SelectEdge(Map *map, u32 brush_id);
void SelectFace(Map *map, u32 brush_id);

