#include <cstdio>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "geo.h"
#include "brush.h"

Mesh basic_sphere;
Material mat_vert_sphere[2];
Material basic_material;
Texture2D tex_default;

void LoadBrushGraphics() {
	basic_sphere = GenMeshSphere(0.25f, 24, 24);

	basic_material = LoadMaterialDefault();
	basic_material.maps->color = GRAY;

	for(u8 i = 0; i < 2; i++) mat_vert_sphere[i] = LoadMaterialDefault();
	mat_vert_sphere[0].maps->color = BLUE;
	mat_vert_sphere[1].maps->color = ORANGE;

	tex_default = LoadTexture("resources/tex_default.png");
}

Brush BrushInitCube(Vector3 position, Vector3 size) {
	Brush brush = (Brush) {0};

	brush.bounds = (BoundingBox) {
		.min = Vector3Add(position, Vector3Scale(size, -0.5f)),
		.max = Vector3Add(position, Vector3Scale(size,  0.5f))
	};

	Plane planes[6] = {
		(Plane) { .normal = (Vector3) {  1,  0,  0  }, .distance =  position.x + size.x * 0.5f }, 
		(Plane) { .normal = (Vector3) {  0,  1,  0  }, .distance =  position.y + size.y * 0.5f }, 
		(Plane) { .normal = (Vector3) {  0,  0,  1  }, .distance =  position.z + size.z * 0.5f }, 
		(Plane) { .normal = (Vector3) { -1,  0,  0  }, .distance = -position.x + size.x * 0.5f }, 
		(Plane) { .normal = (Vector3) {  0, -1,  0  }, .distance = -position.y + size.y * 0.5f }, 
		(Plane) { .normal = (Vector3) {  0,  0, -1  }, .distance = -position.z + size.z * 0.5f }, 
	};	

	brush.num_planes = 6;
	brush.planes = (Plane*)malloc(sizeof(Plane) * brush.num_planes);
	memcpy(brush.planes, planes, sizeof(Plane) * brush.num_planes);

	BrushBuildVertices(&brush);
	BrushBuildFaces(&brush);
	BrushBuildEdges(&brush);
	BrushBuildMesh(&brush);

	return brush;
}

void BrushBuildVertices(Brush *brush) {
	std::vector<Vector3> intersections;
	for(u8 i = 0; i < brush->num_planes; i++) {
		for(u8 j = i+1; j < brush->num_planes; j++) {
			for(u8 k = j+1; k < brush->num_planes; k++) {
				Vector3 v = Vector3Zero();
				if(!ThreePlaneIntersect(brush->planes[i], brush->planes[j], brush->planes[k], &v))
					continue;

				// Check for duplicates
				bool dup = false;
				for(u8 d = 0; d < intersections.size(); d++) {
					if(Vector3DistanceSqr(intersections[d], v) <= 1e-4f) {
						dup = true;
						break;
					}
				}

				// Skip adding duplicate vertices
				if(dup) 
					continue;

				intersections.push_back(v);
			}
		}
	}

	brush->num_vertices = intersections.size();
	brush->vertices = (Vector3*)malloc(sizeof(Vector3) * brush->num_vertices);
	memcpy(brush->vertices, &intersections[0], sizeof(Vector3)* brush->num_vertices);
}

void BrushBuildFaces(Brush *brush) {
	brush->num_faces = brush->num_planes;
	brush->faces = (Brush_Face*)calloc(brush->num_faces, sizeof(Brush_Face));

	for(u8 i = 0; i < brush->num_planes; i++) {
		for(u8 j = 0; j < brush->num_vertices; j++) {
			Brush_Face *f = &brush->faces[i];
			f->plane = i;

			float denom = PlaneDistance(brush->vertices[j], brush->planes[i]);
			if(fabsf(denom) <= 0.01f) {
				f->vertices[f->num_vertices++] = j;
			}
		}
	}

	for(u8 i = 0; i < brush->num_faces; i++) {
		Brush_Face *face = &brush->faces[i];
		Plane *plane = &brush->planes[i];

		face->center = Vector3Zero();
		for(u8 j = 0; j < face->num_vertices; j++) {
			face->center = Vector3Add(face->center, brush->vertices[face->vertices[j]]);
		}
		face->center = Vector3Scale(face->center, 1.0f / face->num_vertices);

		// Reference vector
		Vector3 r = (fabsf(plane->normal.x) < 0.01f && fabsf(plane->normal.z) < 0.01f) ? WORLD_UP : (Vector3) { 0, 1, 0 };
		// U & V vectors for texture mapping (derived from reference)
		Vector3 u = Vector3Normalize(Vector3CrossProduct(plane->normal, r)); 
		Vector3 v = Vector3CrossProduct(plane->normal, u);

		// Measure vertex angles relative to center of face
		float angles[face->num_vertices];
		for(u8 j = 0; j < face->num_vertices; j++) {
			Vector3 vertex = brush->vertices[face->vertices[j]];

			Vector3 d = Vector3Subtract(vertex, face->center); 
			angles[j] = atan2f(Vector3DotProduct(d, v), Vector3DotProduct(d, u));
		}

		// Sort each faces vertices by angle in counter-clockwise winding order
		for(u8 a = 0; a < face->num_vertices; a++) {
			for(u8 b = a + 1; b < face->num_vertices; b++) {
				if(angles[a] > angles[b]) {
					u8 temp_id = face->vertices[b];
					face->vertices[b] = face->vertices[a];
					face->vertices[a] = temp_id;

					float temp_angle = angles[b]; 
					angles[b] = angles[a];
					angles[a] = temp_angle;
				}
			}
		}

		// Populate face triangles
		for(u8 j = 1; j < face->num_vertices - 1; j++) {
			Tri tri = (Tri) {0};
			TriRef tri_ref = (TriRef) {0};
			
			u8 vert_ids[3] = { face->vertices[0], face->vertices[j], face->vertices[j+1] };
			Vector3 verts[3] = { brush->vertices[face->vertices[0]], brush->vertices[face->vertices[j]], brush->vertices[face->vertices[j+1]] };
			memcpy(tri.vertices, verts, sizeof(Vector3) * 3);
			memcpy(tri_ref.vertices, vert_ids, sizeof(u8) * 3);

			tri.normal = brush->planes[face->plane].normal;

			face->tris[face->num_tris] = tri;
			face->tri_refs[face->num_tris] = tri_ref;

			face->num_tris++;
		}
	}
}

void BrushBuildEdges(Brush *brush) {
	std::vector<Brush_Edge> edges; 

	for(u8 i = 0; i < brush->num_faces; i++) {
		Brush_Face *face = &brush->faces[i];
		
		for(u8 j = 0; j < face->num_vertices; j++) {
			for(u8 k = j+1; k < face->num_vertices; k++) {
				Vector3 p0 = brush->vertices[face->vertices[j]];
				Vector3 p1 = brush->vertices[face->vertices[k]];

				Vector3 dir_p = Vector3Normalize(Vector3Subtract(p1, p0));
				Vector3 dir_c = Vector3Normalize(Vector3Subtract(p1, face->center));

				if(Vector3DotProduct(dir_p, dir_c) >= 1.0f - 1e-4f)
					continue;

				bool dup = false;
				for(u8 n = 0; n < edges.size(); n++) {
					Brush_Edge *other = &edges[n];
					
					if( (other->p[0] == face->vertices[j] && other->p[1] == face->vertices[j]) ||
						(other->p[0] == face->vertices[k] && other->p[1] == face->vertices[j]) 
						) dup = true;
				}

				if(dup)
					continue;

				Brush_Edge edge = (Brush_Edge) {0};
				edge.p[0] = face->vertices[j];
				edge.p[1] = face->vertices[k];
				edges.push_back(edge);
			}
		}
	}

	brush->num_edges = edges.size();
	brush->edges = (Brush_Edge*)malloc(sizeof(Brush_Edge) * brush->num_edges);
	memcpy(brush->edges, edges.data(), sizeof(Brush_Edge) * brush->num_edges);
}

void BrushBuildMesh(Brush *brush) {
	Mesh *mesh = &brush->mesh;
	*mesh = (Mesh) {0};

	for(u8 i = 0; i < brush->num_faces; i++) mesh->triangleCount += brush->faces[i].num_tris;
	mesh->vertexCount = mesh->triangleCount * 3;

	mesh->vertices 		= (float*)MemAlloc(sizeof(float) * mesh->vertexCount * 3);
	mesh->normals  		= (float*)MemAlloc(sizeof(float) * mesh->vertexCount * 3);
	mesh->texcoords 	= (float*)MemAlloc(sizeof(float) * mesh->vertexCount * 2);
	mesh->texcoords2 	= (float*)MemAlloc(sizeof(float) * mesh->vertexCount * 2);

	u16 vert_id = 0;
	for(u8 i = 0; i < brush->num_faces; i++) {
		Brush_Face *face = &brush->faces[i];
		Plane *plane = &brush->planes[face->plane];

		for(u8 j = 0; j < face->num_tris; j++) {
			Tri tri = face->tris[j];

			for(u8 k = 0; k < 3; k++) {
				memcpy(&mesh->vertices[vert_id*3], &tri.vertices[k], sizeof(Vector3));
				memcpy(&mesh->normals[vert_id*3], &tri.normal, sizeof(Vector3));

				Vector2 uv = Vector2Zero();
				Vector3 vabs = (Vector3) { fabsf(plane->normal.x), fabsf(plane->normal.y), fabsf(plane->normal.z) };

				if(vabs.x >= vabs.y && vabs.x >= vabs.z) {
					uv.x = tri.vertices[k].z;
					uv.y = tri.vertices[k].y;
				} else if(vabs.y >= vabs.x && vabs.y >= vabs.z) {
					uv.x = tri.vertices[k].x;
					uv.y = tri.vertices[k].z;
				} else {
					uv.x = tri.vertices[k].x;
					uv.y = tri.vertices[k].y;
				}

				memcpy(&mesh->texcoords[vert_id*2], &uv, sizeof(Vector2));

				vert_id++;
			}
		}
	}

	UploadMesh(mesh, true);
	brush->model = LoadModelFromMesh(brush->mesh);
	brush->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex_default;
}

void BrushDraw(Brush *brush, u8 flags) {
	//DrawModel(brush->model, Vector3Zero(), 1.0f, GRAY);

	for(u8 i = 0; i < brush->num_faces; i++) {
		Brush_Face *face = &brush->faces[i];

		for(u8 j = 0; j < 2; j++) {
			Tri *tri = &face->tris[j];
			DrawTriangle3D(tri->vertices[0], tri->vertices[1], tri->vertices[2], GRAY);
		}
	}

	if(flags & F_BRUSH_DRAW_WIRES)
		DrawModelWires(brush->model, Vector3Zero(), 1.0f, MAGENTA);

	if(flags & F_BRUSH_DRAW_AABB)
		DrawBoundingBox(brush->bounds, MAGENTA);

	if(flags & F_BRUSH_DRAW_IS_SELECTED) {
		/*
		for(u8 i = 0; i < brush->num_faces; i++) {
			Brush_Face *face = &brush->faces[i];

			for(u8 j = 0; j < face->num_tris; j++) {
				Tri *tri = &face->tris[j];
				DrawTriangle3D(tri->vertices[0], tri->vertices[1], tri->vertices[2], ColorAlpha(ORANGE, 0.1f));

				if(flags & F_BRUSH_DRAW_FACES) {
					DrawMesh(basic_sphere, mat_vert_sphere[0], MatrixTranslate(face->center.x, face->center.y, face->center.z));
				}
			}
		}
		*/

		for(u8 i = 0; i < brush->num_selected_faces; i++) {
			Brush_Face *face = &brush->faces[brush->selected_faces[i]];

			for(u8 j = 0; j < face->num_tris; j++) {
				Tri *tri = &face->tris[j];
				DrawTriangle3D(tri->vertices[0], tri->vertices[1], tri->vertices[2], ColorAlpha(ORANGE, 0.1f));

				if(flags & F_BRUSH_DRAW_FACES) {
					DrawMesh(basic_sphere, mat_vert_sphere[1], MatrixTranslate(face->center.x, face->center.y, face->center.z));
				}
			}
		}

		if(flags & F_BRUSH_DRAW_VERTICES) {
			BrushDrawVertices(brush);

			for(u8 j = 0; j < brush->num_selected_vertices; j++) {
				Vector3 v = brush->vertices[brush->selected_vertices[j]];
				DrawMesh(basic_sphere, mat_vert_sphere[1], MatrixTranslate(v.x, v.y, v.z));
			}
		}
	}
}

void BrushDrawVertices(Brush *brush) {
	for(u8 i = 0; i < brush->num_vertices; i++) {
		Vector3 v = brush->vertices[i];

		DrawMesh(basic_sphere, mat_vert_sphere[0], MatrixTranslate(v.x, v.y, v.z));
	}
}

void BrushDrawEdges(Brush *brush) {
	for(u8 i = 0; i < brush->num_edges; i++) {
		Brush_Edge *edge = &brush->edges[i];

		Vector3 p0 = brush->vertices[edge->p[0]];
		Vector3 p1 = brush->vertices[edge->p[1]];

		DrawLine3D(p0, p1, MAGENTA);
	}
}

void BrushMoveVertex(Brush *brush) {
	Vector3 move = Vector3Zero();

	if(IsKeyPressed(KEY_PAGE_UP)) 
		move = Vector3Add(move, WORLD_UP);

	if(IsKeyPressed(KEY_PAGE_DOWN)) 
		move = Vector3Add(move, WORLD_DOWN);

	if(IsKeyPressed(KEY_UP))
		move = Vector3Add(move, (Vector3) { 0, 1, 0 } );

	if(IsKeyPressed(KEY_DOWN))
		move = Vector3Add(move, (Vector3) { 0, -1, 0 } );

	if(IsKeyPressed(KEY_LEFT))
		move = Vector3Add(move, (Vector3) { -1, 0, 0 } );

	if(IsKeyPressed(KEY_RIGHT))
		move = Vector3Add(move, (Vector3) { 1, 0, 0 } );

	if(!Vector3LengthSqr(move)) return;

	for(u8 i = 0; i < brush->num_selected_vertices; i++) {
		u8 vert_id = brush->selected_vertices[i];
		brush->vertices[vert_id] = Vector3Add(brush->vertices[brush->selected_vertices[i]], move);
	}

	for(u8 i = 0; i < brush->num_selected_faces; i++) {
		Brush_Face *face = &brush->faces[brush->selected_faces[i]];

		for(u8 j = 0; j < face->num_vertices; j++) {
			for(u8 k = 0; k < 2; k++) {
				TriRef *tri_ref = &face->tri_refs[k];
				Tri *tri = &face->tris[k];

				Vector3 verts[3] = { brush->vertices[tri_ref->vertices[0]], brush->vertices[tri_ref->vertices[1]], brush->vertices[tri_ref->vertices[2]] };
				memcpy(&tri->vertices, verts, sizeof(Vector3) * 3);
			}
		}
	}

	for(u8 i = 0; i < brush->num_faces; i++) {
		Brush_Face *face = &brush->faces[i];
		Plane plane = brush->planes[face->plane];

		for(u8 j = 0; j < face->num_vertices; j++) {
			u8 vert_id = face->vertices[j];

			if(fabsf(PlaneDistance(brush->vertices[vert_id], plane)) > 0.01f) {
			} 
		}
	}

	/*
	Mesh *mesh = &brush->mesh;

	u16 vert_id = 0;
	for(u8 i = 0; i < brush->num_faces; i++) {
		Brush_Face *face = &brush->faces[i];
		Plane *plane = &brush->planes[face->plane];

		for(u8 j = 0; j < face->num_tris; j++) {
			Tri tri = face->tris[j];

			for(u8 k = 0; k < 3; k++) {
				memcpy(&mesh->vertices[vert_id*3], &tri.vertices[k], sizeof(Vector3));
				memcpy(&mesh->normals[vert_id*3], &tri.normal, sizeof(Vector3));
				vert_id++;
			}
		}
	}
		
	UploadMesh(&brush->mesh, true);
	*/
}

