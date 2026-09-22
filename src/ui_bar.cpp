#include "raylib.h"
#include "app.h"
#include "ui.h"
#include "map.h"

enum TITLES_ENUM : i8 {
	TITLE_NONE = -1,
	TITLE_FILE =  0,
	TITLE_EDIT =  1,
	TITLE_VIEW =  2,
	TITLE_HELP =  4,
};
u8 num_titles = 4;

const char *titles[] = {
	"File",
	"Edit",
	"View",
	"Help",
};
i8 active_title = -1;

enum buttons_file_enum : u8 {
	FILE_NEW,
	FILE_OPEN,
	FILE_SAVE,
	FILE_SAVE_AS,
	FILE_QUIT
};

const char *buttons_file[] = {
	"new",
	"open",
	"save",
	"save as",
	"quit"
};
u8 num_file_buttons = 5;

void DoButtons_File(App *app) {
	ui_DisableOutlines();

	Rectangle base_rec = (Rectangle) { 0, 0, 128, 32 };

	for(u8 i = 0; i < num_file_buttons; i++) {
		Rectangle rec = base_rec;
		rec.y = (i + 1) * 32;

		if(ui_Button(rec, buttons_file[i])) {
			switch(i) {
				case FILE_NEW:
					app->map_buffer = MapInitNew();
					app->pMap = &app->map_buffer;
					break;

				case FILE_OPEN:
					break;

				case FILE_SAVE:
					break;

				case FILE_SAVE_AS:
					break;

				case FILE_QUIT:
					app->running = false;
					break;
			}
		}
	}

	Rectangle outline_rec = base_rec;
	outline_rec.y += 32;
	outline_rec.height = 32 * num_file_buttons;
	DrawRectangleLinesEx(outline_rec, 2.0f, BLACK);

	ui_EnableOutlines();
}

void App::Bar() {
	for(u8 i = 0; i < num_titles; i++) {
		if(ui_Button( (Rectangle) { 128 * (float)i, 0, 128, 32 } , titles[i])) {
			active_title = (active_title == i) ? -1 : i; 
		}
	}

	switch(active_title) {
		case TITLE_FILE:
			DoButtons_File(this);
			break;

		case TITLE_EDIT:
			break;

		case TITLE_VIEW:
			break;
	}
}

