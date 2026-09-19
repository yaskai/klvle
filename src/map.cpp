#include <cfloat>
#include <cstdlib>
#include <stdlib.h>
#include "brush.h"
#include "raylib.h"
#include "map.h"

Ray cursor_ray;
Vector3 cam_pos;

Map MapInitNew() {
	Map map = (Map) {0};	

	map.cap_brushes = 128;
	map.brushes = (Brush*)calloc(map.cap_brushes, sizeof(Brush));

	return map;
}

void MapClose(Map *map) {
	for(u32 i = 0; i < map->num_brushes; i++) {
		Brush *brush = &map->brushes[i];

		free(brush->vertices);
		free(brush->planes);

		UnloadMesh(brush->mesh);

		*brush = (Brush) {0};
	}

	free(map->brushes);
	*map = (Map) {0};
}

void MapUpdate(Map *map, float dt) {
	if(IsKeyPressed(KEY_B)) {
		Brush new_brush = BrushInitCube(Vector3Add(cursor_ray.position, Vector3Scale(cursor_ray.direction, 10)), Vector3Scale(Vector3One(), 5));	
		map->brushes[map->num_brushes++] = new_brush;
	}

	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))	{
		bool hit_any = false;
		float closest_dist = FLT_MAX;
		u32 closest_brush = 0;

		for(u32 i = 0; i < map->num_brushes; i++) {
			Brush *brush = &map->brushes[i];

			RayCollision coll = GetRayCollisionBox(cursor_ray, brush->bounds);

			if(!coll.hit) continue;
			hit_any = true;

			if(coll.distance > closest_dist) continue;

			closest_dist = coll.distance; 
			closest_brush = i;
		}

		map->selected_brush = (hit_any) ? closest_brush : -1;
	}
}

void MapDraw(Map *map, Camera3D camera, float alpha) {
	cursor_ray = GetScreenToWorldRay(GetMousePosition(), camera);
	cam_pos = camera.position;

	BeginMode3D(camera);
	ClearBackground(BLACK);

	for(u32 i = 0; i < map->num_brushes; i++) {
		Brush *brush = &map->brushes[i];
		BrushDraw(brush, (map->selected_brush == i) ? F_BRUSH_DRAW_IS_SELECTED : 0);
	}

	EndMode3D();
}

