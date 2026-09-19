#pragma once

#include "raylib.h"
#include "nums.h"

enum WIDGET_STATES : u8 {
	WG_DEFAULT,
	WG_FOCUSED,
	WG_PRESSED
};

enum TEXT_ALIGNMENTS : u8 {
	ALIGN_CENTER,
	ALIGN_LEFT,
	ALIGN_RIGHT
};

enum ELEMENTS_MASK_ENUM : u8 {
	EL_BACKGROUND 	= 0x01,
	EL_OUTLINE		= 0x02,
	EL_TEXT			= 0x04,
};

typedef struct { 
	Font font;

	Color colors[3];
	Color bg_colors[3];

	Vector2 text_padding;

	float text_size, text_spacing;
	float line_thick;

	u8 enabled_elements;
	u8 align;
	u8 state;
	u8 flags;

} UiImplementation;

void ui_Init(Font font);
void ui_Close();

Vector2 ui_TextCenter(Rectangle rect, const char *text, float size, float spacing);

void ui_SetAlignment(u8 alignment);

void ui_DisableOutlines();
void ui_EnableOutlines();

void ui_DrawText(Rectangle rect, const char *text, Color color);

bool ui_Button(Rectangle rect, const char *text);

void ui_CheckBox(Rectangle rect, const char *text, bool *val);

void ui_Label(Rectangle rect, const char *text);

typedef struct {
	const char **text;		
	i32 count, idx;
	
} UI_DirButtonData;

void ui_DirectionalButton(Rectangle rect, UI_DirButtonData *data);

