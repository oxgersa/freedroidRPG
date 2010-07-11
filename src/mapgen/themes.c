/* 
 *
 *   Copyright (c) 2010, Alexander Solovets
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 * */ 
#include "defs.h"
#include "proto.h"

#include "mapgen/mapgen.h"
#include "mapgen/themes.h"

static void apply_default_theme(int, int, int);
static void apply_metal_theme(int, int, int);
static void apply_glass_theme(int, int, int);
static void apply_red_theme(int, int, int);
static void apply_green_theme(int, int, int);

const struct theme_info theme_data[] = {
	{ ISO_V_WALL, ISO_H_WALL, ISO_V_WALL, ISO_H_WALL },
	{ ISO_GREY_WALL_END_E, ISO_GREY_WALL_END_S, ISO_GREY_WALL_END_E, ISO_GREY_WALL_END_S },
	{ ISO_GLASS_WALL_1, ISO_GLASS_WALL_2, ISO_GLASS_WALL_1, ISO_GLASS_WALL_2 },
	{ ISO_ROOM_WALL_V_RED, ISO_ROOM_WALL_H_RED, ISO_V_WALL, ISO_H_WALL },
	{ ISO_BROKEN_GLASS_WALL_1, ISO_GLASS_WALL_2, ISO_GLASS_WALL_1, ISO_GLASS_WALL_2 },
	{ ISO_LIGHT_GREEN_WALL_1, ISO_LIGHT_GREEN_WALL_2, ISO_V_WALL, ISO_H_WALL }
};

typedef void (*theme_proc)(int, int, int);
const theme_proc themes[] = {
	apply_default_theme,
	apply_metal_theme,
	apply_glass_theme,
	apply_red_theme,
	apply_glass_theme,
	apply_green_theme
};

// A list of themes are used to specify basic parameters for a room
// such as wall and floor tiles that may be partly ommited in others themes.
const int basic_themes[] = {
	THEME_METAL,
	THEME_GRAY,
	THEME_RED,
	THEME_GREEN
};

static int set_generic_wall(int x, int y, int wall, int theme)
{
	// A value '1' of 'processed' means that data was processed, otherwise
	// the caller should process them itself.
	int processed = 1;
	switch (wall) {
		case 0:
			break;
		case WALL_N:
			mapgen_add_obstacle(x + 0.5, y, theme_data[theme].wall_n);
			break;
		case WALL_S:
			mapgen_add_obstacle(x + 0.5, y + 1, theme_data[theme].wall_s);
			break;
		case WALL_W:
			mapgen_add_obstacle(x, y + 0.5, theme_data[theme].wall_w);
			break;
		case WALL_E:
			mapgen_add_obstacle(x + 1, y + 0.5, theme_data[theme].wall_e);
			break;
		default:
			processed = 0;
	}

	return processed;
}

static void set_simple_wall(int x, int y, int wall, int theme)
{
	if (set_generic_wall(x, y, wall, theme)) return;
	switch (wall) {
		case WALL_NW:
			mapgen_add_obstacle(x + 0.5, y, theme_data[theme].wall_n);
			mapgen_add_obstacle(x, y + 0.5, theme_data[theme].wall_w);
			break;
		case WALL_NE:
			mapgen_add_obstacle(x + 0.5, y, theme_data[theme].wall_n);
			mapgen_add_obstacle(x + 1, y + 0.5, theme_data[theme].wall_e);
			break;
		case WALL_SW:
			mapgen_add_obstacle(x + 0.5, y + 1, theme_data[theme].wall_s);
			mapgen_add_obstacle(x, y + 0.5, theme_data[theme].wall_w);
			break;
		case WALL_SE:
			mapgen_add_obstacle(x + 0.5, y + 1, theme_data[theme].wall_s);
			mapgen_add_obstacle(x + 1, y + 0.5, theme_data[theme].wall_e);
			break;
		default:
			ErrorMessage(__FUNCTION__, "Unknown type of wall %d at (%d, %d)\n ", PLEASE_INFORM, IS_FATAL, wall, x, y);
	}
}

static void apply_default_theme(int x, int y, int object)
{
	set_simple_wall(x, y, object, THEME_METAL);
	mapgen_set_floor(x, y, ISO_FLOOR_ERROR_TILE);
}

static void apply_metal_theme(int x, int y, int object)
{
	set_simple_wall(x, y, object, THEME_GRAY);
	mapgen_set_floor(x, y, ISO_FLOOR_STONE_FLOOR);
}

static void apply_glass_theme(int x, int y, int object)
{
	int theme = object == WALL_W && MyRandom(100) < 15 ? THEME_BROKEN_GLASS : THEME_GLASS;
	set_simple_wall(x, y, object, theme);
	mapgen_set_floor(x, y, ISO_MINI_SQUARE_0003);
}

static void apply_red_theme(int x, int y, int object)
{
	set_simple_wall(x, y, object, THEME_RED);
	mapgen_set_floor(x, y, ISO_CARPET_TILE_0004);
} 

static void apply_green_theme(int x, int y, int object)
{
	set_simple_wall(x, y, object, THEME_GREEN);
	mapgen_set_floor(x, y, ISO_CARPET_TILE_0002);
} 

static void set_interior_objects()
{
	int i;

	for (i = 0; i < total_rooms; i++)
		if(MyRandom(100) < 30)
			mapgen_gift(&rooms[i]);
}

void mapgen_place_obstacles(int w, int h, unsigned char *tiles) 
{
	int x, y;
	int wall, room, theme;

	for (x = 0; x < total_rooms; x++) {
		theme = rand() % (sizeof(basic_themes) / sizeof(basic_themes[0]));
		rooms[x].theme = basic_themes[theme];
	}
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			room = mapgen_get_room(x, y);
			switch(tiles[y * w + x]) {
				case TILE_FLOOR:
					wall = 0;
 					if (tiles[y * w + x - 1] == TILE_WALL)
						wall |= WALL_W;
					if (tiles[y * w + x + 1] == TILE_WALL)
						wall |= WALL_E;
					if (tiles[(y - 1) * w + x] == TILE_WALL)
						wall |= WALL_N;
					if (tiles[(y + 1) * w + x] == TILE_WALL)
						wall |= WALL_S;
					themes[rooms[room].theme](x, y, wall);
					break;	
				case TILE_WALL:
					mapgen_set_floor(x, y, 31);
					break;
				default:
					mapgen_set_floor(x, y, tiles[y * w + x]);
			}
		}
	} 

	set_interior_objects();
} 
