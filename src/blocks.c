/*
 *
 *   Copyright (c) 2004-2010 Arthur Huillet
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
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
 *
 */

#define _blocks_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "SDL_rotozoom.h"

#include "lvledit/lvledit_display.h"

char *PrefixToFilename[ENEMY_ROTATION_MODELS_AVAILABLE];

/**
 *
 *
 */
void make_sure_zoomed_surface_is_there(iso_image * our_iso_image)
{
	if (our_iso_image->zoomed_out_surface == NULL) {
		our_iso_image->zoomed_out_surface = zoomSurface(our_iso_image->surface, lvledit_zoomfact_inv(),
								lvledit_zoomfact_inv(), FALSE);
	}
};				// void make_sure_zoomed_surface_is_there ( iso_image* our_iso_image )

/**
 * The automap (in OpenGL mode) uses some smaller version of the graphics
 * used to assemble the in-game obstacle images.  These smaller versions 
 * of the graphics have to be generated.  We do that on the fly at 
 * runtime once and for all obstacles at the first game startup.
 */
void make_sure_automap_surface_is_there(obstacle_spec * our_obstacle_spec)
{
	our_obstacle_spec->automap_version =
	    zoomSurface(our_obstacle_spec->image.surface, (1.0 / AUTOMAP_ZOOM_OUT_FACT), (1.0 / AUTOMAP_ZOOM_OUT_FACT), FALSE);
	if (!our_obstacle_spec->automap_version) {
		ErrorMessage(__FUNCTION__, "\
Creation of automap surface failed", PLEASE_INFORM, IS_FATAL);
	}
};				// void make_sure_automap_surface_is_there ( iso_image* our_iso_image )

/**
 * This function loads the Blast image and decodes it into the multiple
 * small Blast surfaces.
 */
void Load_Blast_Surfaces(void)
{
	int j;
	char fpath[2048] = "";
	char constructed_filename[5000];

	// Now that we're here, we can as well load the blast surfaces, that we might be using
	// later...
	//
	for (j = 0; j < PHASES_OF_EACH_BLAST; j++) {
		sprintf(constructed_filename, "blasts/iso_blast_bullet_%04d.png", j + 1);
		find_file(constructed_filename, GRAPHICS_DIR, fpath, 0);
		get_iso_image_from_file_and_path(fpath, &(Blastmap[0].image[j]), TRUE);
	}

	for (j = 0; j < PHASES_OF_EACH_BLAST; j++) {
		sprintf(constructed_filename, "blasts/iso_blast_droid_%04d.png", j + 1);
		find_file(constructed_filename, GRAPHICS_DIR, fpath, 0);
		get_iso_image_from_file_and_path(fpath, &(Blastmap[1].image[j]), TRUE);
	}

	for (j = 0; j < PHASES_OF_EACH_BLAST; j++) {
		sprintf(constructed_filename, "blasts/iso_blast_exterminator_%04d.png", j + 1);
		find_file(constructed_filename, GRAPHICS_DIR, fpath, 0);
		get_iso_image_from_file_and_path(fpath, &(Blastmap[2].image[j]), TRUE);
	}

/*Now also set up values for blasts*/
	Blastmap[0].phases = 6;
	Blastmap[1].phases = 9;
	Blastmap[2].phases = 9;
	Blastmap[0].total_animation_time = 0.6;
	Blastmap[1].total_animation_time = 1.0;
	Blastmap[2].total_animation_time = 1.0;

};				// void Load_Blast_Surfaces( void )

/**
 *
 *
 */
void load_item_surfaces_for_item_type(int item_type)
{
	SDL_Surface *original_img;
	SDL_Surface *tmp_surf2 = NULL;
	char fpath[2048];
	char our_filename[2000];

	// First we load the inventory image.
	//
	sprintf(our_filename, "items/%s", ItemMap[item_type].item_inv_file_name);

	find_file(our_filename, GRAPHICS_DIR, fpath, 0);
	original_img = NULL;
	original_img = IMG_Load(fpath);
	if (original_img == NULL) {
		ErrorMessage(__FUNCTION__, "\
Inventory image for item type %d, at path %s was not found", PLEASE_INFORM, IS_FATAL, item_type, fpath);
	}

	if (use_open_gl)
		flip_image_vertically(original_img);

	// Now we try to guess the inventory image tile sizes (in the 
	// inventory screen) from the pixel size of the inventory image
	// loaded.
	//
	// (AH)This approach is not perfect due to resolution considerations. Scaling may or may not be done.  We keep it by default, except
	// if the size is already set.

	if (!ItemMap[item_type].inv_image.inv_size.x) {
		if (original_img->w % 32) {
			ErrorMessage(__FUNCTION__, "\
Inventory image for item %d (%s) given does not have a multiple-of-32 width.\n\
FreedroidRPG needs a width of this type, so it can associate the right\n\
number of inventory screen tiles with the item.", PLEASE_INFORM, IS_FATAL, item_type, fpath);
		} else {
			ItemMap[item_type].inv_image.inv_size.x = original_img->w / 32;
		}
	}

	if (!ItemMap[item_type].inv_image.inv_size.y) {
		if (original_img->h % 32) {
			ErrorMessage(__FUNCTION__, "\
	      Inventory image for item %d (%s) given does not have a multiple-of-32 height.\n\
	      FreedroidRPG needs a height of this type, so it can associate the right\n\
	      number of inventory screen tiles with the item.", PLEASE_INFORM, IS_FATAL, item_type, fpath);
		} else {
			ItemMap[item_type].inv_image.inv_size.y = original_img->h / 32;
		}
	}
	// Does the image need scaling ? (currently only one item needs it, but as I'd like to raise the standard sizes from 32x32 to at least 64x64,
	// this code makes sense)
	int target_x = ItemMap[item_type].inv_image.inv_size.x * 32;
	int target_y = ItemMap[item_type].inv_image.inv_size.y * 32;
	float factor_x, factor_y;
	if ((target_x != original_img->w) || (target_y != original_img->h)) {
		factor_x = (float)target_x / (float)original_img->w;
		factor_y = (float)target_y / (float)original_img->h;
		tmp_surf2 = zoomSurface(original_img, factor_x, factor_y, FALSE);
		ItemMap[item_type].inv_image.Surface = our_SDL_display_format_wrapperAlpha(tmp_surf2);
		SDL_FreeSurface(tmp_surf2);
	} else
		ItemMap[item_type].inv_image.Surface = our_SDL_display_format_wrapperAlpha(original_img);

	// For the shop, we need versions of each image, where the image is scaled so
	// that it takes up a whole 64x64 shop display square.  So we prepare scaled
	// versions here and now...

	if (original_img->w >= original_img->h) {
		target_x = 64;
		target_y = original_img->h * 64.0 / (float)original_img->w;	//keep the scaling ratio !
	}
	if (original_img->h > original_img->w) {
		target_y = 64;
		target_x = original_img->w * 64.0 / (float)original_img->h;
	}
	factor_x = ((float)GameConfig.screen_width / 640.0) * ((float)target_x / (float)original_img->w);
	factor_y = ((float)GameConfig.screen_height / 480.0) * ((float)target_y / (float)original_img->h);
	tmp_surf2 = zoomSurface(original_img, factor_x, factor_y, FALSE);
	ItemMap[item_type].inv_image.scaled_surface_for_shop = our_SDL_display_format_wrapperAlpha(tmp_surf2);
	SDL_FreeSurface(original_img);
	SDL_FreeSurface(tmp_surf2);

};				// void load_item_surfaces_for_item_type ( int item_type )

/**
 *
 *
 */
void try_to_load_ingame_item_surface(int item_type)
{
	char ConstructedFileName[5000];
	char fpath[2048];
	SDL_Surface *Whole_Image;

	// First we handle a case, that shouldn't really be happening due to
	// calling function checking already.  But it can't hurt to always double-check
	//
	if ((ItemMap[item_type].inv_image.ingame_iso_image.surface != NULL)
	    || (ItemMap[item_type].inv_image.ingame_iso_image.texture_has_been_created)) {
		ErrorMessage(__FUNCTION__, "\
Surface for item type %d has been already loaded", PLEASE_INFORM, IS_FATAL, item_type);
	}
	// Now we should try to load the real in-game item surface...
	// That will be added later...
	//
	// At first we will try to find some item rotation models in the
	// new directory structure.
	//
	sprintf(ConstructedFileName, "items/%s/ingame.png", ItemMap[item_type].item_rotation_series_prefix);
	find_file(ConstructedFileName, GRAPHICS_DIR, fpath, 0);
	Whole_Image = our_IMG_load_wrapper(fpath);	// This is a surface with alpha channel, since the picture is one of this type

	// If that didn't work, then it's time to try out the 'classic' rotation models directory.
	// Maybe there's still some rotation image there.
	//
	if (Whole_Image == NULL) {
		// No ingame item surface found? -- give error message and then use
		// the inventory item_surface for the job.
		//
		ErrorMessage(__FUNCTION__, "\
Unable to load item %d's ingame surface. (path tried %s)\n\
Since there seems to be no ingame item surface yet, the inventory\n\
item surface will be used as a substitute for now.", NO_NEED_TO_INFORM, IS_WARNING_ONLY, item_type, fpath);

		if (use_open_gl) {
			ItemMap[item_type].inv_image.ingame_iso_image.surface =
			    SDL_DisplayFormatAlpha(ItemMap[item_type].inv_image.Surface);
		} else {
			ItemMap[item_type].inv_image.ingame_iso_image.surface = ItemMap[item_type].inv_image.Surface;
		}

		ItemMap[item_type].inv_image.ingame_iso_image.offset_x = -ItemMap[item_type].inv_image.Surface->w / 2;
		ItemMap[item_type].inv_image.ingame_iso_image.offset_y = -ItemMap[item_type].inv_image.Surface->h / 2;

	} else {
		// So if an image of the required type can be found there, we 
		// can start to load it.  But for this we will use standard iso
		// object loading function, so that offset gets respected too...
		//
		/*
		   SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
		   ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_surface = 
		   our_SDL_display_format_wrapperAlpha( Whole_Image ); // now we have an alpha-surf of right size
		   SDL_SetColorKey( ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_surface , 0 , 0 ); // this should clear any color key in the dest surface
		 */
		get_iso_image_from_file_and_path(fpath, &(ItemMap[item_type].inv_image.ingame_iso_image), TRUE);
		SDL_FreeSurface(Whole_Image);
	}

	// Now that it has been made sure, that a dispensable image is
	// loaded for the ingame surface, we can destroy it and make a
	// textured quad from it...
	//
	if (use_open_gl) {
		make_texture_out_of_surface(&(ItemMap[item_type].inv_image.ingame_iso_image));
	}

};				// void try_to_load_ingame_item_surface ( int item_number )

/**
 * This function loads the items image and decodes it into the multiple
 * small item surfaces.
 */
void Load_Mouse_Move_Cursor_Surfaces(void)
{
	int j;
	char fpath[2048];
	char our_filename[2000] = "";

	for (j = 0; j < NUMBER_OF_MOUSE_CURSOR_PICTURES; j++) {
		sprintf(our_filename, "mouse_move_cursor_%d.png", j);
		find_file(our_filename, GRAPHICS_DIR, fpath, 0);

		get_iso_image_from_file_and_path(fpath, &(MouseCursorImageList[j]), TRUE);

		if (use_open_gl)
			make_texture_out_of_surface(&(MouseCursorImageList[j]));
	}

};				// void Load_Mouse_Move_Cursor_Surfaces( void )

/**
 * This function loads the image containing the different buttons for the
 * different skills in the skill book of the Tux.
 */
void Load_Skill_Level_Button_Surfaces(void)
{
	static int SkillLevelButtonsAreAlreadyLoaded = FALSE;
	SDL_Surface *Whole_Image;
	SDL_Surface *tmp_surf;
	SDL_Rect Source;
	SDL_Rect Target;
	int i = 0;
	int j;
	char fpath[2048];

	// Maybe this function has been called before.  Then we do not
	// need to do anything (again) here and can just return.
	//
	if (SkillLevelButtonsAreAlreadyLoaded)
		return;

	// Now we proceed to load all the skill circle buttons.
	//
	find_file(SKILL_LEVEL_BUTTON_FILE, GRAPHICS_DIR, fpath, 0);

	Whole_Image = our_IMG_load_wrapper(fpath);	// This is a surface with alpha channel, since the picture is one of this type
	SDL_SetAlpha(Whole_Image, 0, SDL_ALPHA_OPAQUE);

	tmp_surf = SDL_CreateRGBSurface(0, SKILL_LEVEL_BUTTON_WIDTH, SKILL_LEVEL_BUTTON_HEIGHT, vid_bpp, 0, 0, 0, 0);
	SDL_SetColorKey(tmp_surf, 0, 0);	// this should clear any color key in the source surface

	for (j = 0; j < NUMBER_OF_SKILL_PAGES; j++) {
		Source.x = j * (SKILL_LEVEL_BUTTON_WIDTH);
		Source.y = i * (SKILL_LEVEL_BUTTON_HEIGHT);
		Source.w = SKILL_LEVEL_BUTTON_WIDTH;
		Source.h = SKILL_LEVEL_BUTTON_HEIGHT;
		Target.x = 0;
		Target.y = 0;
		Target.w = Source.w;
		Target.h = Source.h;

		SpellLevelButtonImageList[j].surface = our_SDL_display_format_wrapperAlpha(tmp_surf);	// now we have an alpha-surf of right size
		SDL_SetColorKey(SpellLevelButtonImageList[j].surface, 0, 0);	// this should clear any color key in the dest surface
		// Now we can copy the image Information
		our_SDL_blit_surface_wrapper(Whole_Image, &Source, SpellLevelButtonImageList[j].surface, &Target);
		SDL_SetAlpha(SpellLevelButtonImageList[j].surface, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
	}

	SDL_FreeSurface(tmp_surf);
	SDL_FreeSurface(Whole_Image);

	SkillLevelButtonsAreAlreadyLoaded = TRUE;

};				// void Load_Skill_Level_Button_Surfaces( void )

/**
 *
 *
 */
void iso_load_bullet_surfaces(void)
{
	int i, j, k;
	char fpath[2048];
	char constructed_filename[5000];

	// Important Note : the following array HAS to be in sync with the
	// enum _bullets (in defs.h), and so also with the 'bullet_image_type'
	// values in freedroid.item_archetypes
	char *bullet_identifiers[] = {
		"pulse",	// 0 "pluse" or "classic 001"
		"single",	// 1 "single" or "classic 476"
		"military",	// 2 "military" or "classic 821"
		"flash_dummy",	// 3 dummy:  "classic flash", will be left out
		"exterminator",	// 4 "exterminator" , same as in classic
		"laser_rifle",	// 5 "laser rifle" , same as in classic
		"half_pulse",	// 6 "just one half of the two classic 001"
		"plasma_white",	// 7 small round white plasma ball
		"laser_sword",	// 8
		"laser_axe",	// 9
		"single",	// 10 repetition of the single
		"half_green",	// 11 that's the poison
		"half_blue",	// 12 that's the cold
		"half_magenta",	// 13 that's the ??
		"half_white",	// 14 that's the stone
		"ERROR:  UNHANDLED BULLET IMAGE TYPE",	// 15 error-code
		"ERROR:  UNHANDLED BULLET IMAGE TYPE",	// 16 error-code
		"ERROR:  UNHANDLED BULLET IMAGE TYPE",	// 17 error-code
		"ERROR:  UNHANDLED BULLET IMAGE TYPE",	// 18 error-code
		"ERROR:  UNHANDLED BULLET IMAGE TYPE",	// 19 error-code
	};

	DebugPrintf(1, "Number_Of_Bullet_Types: %d.", Number_Of_Bullet_Types);

	for (i = 0; i < Number_Of_Bullet_Types; i++) {
		// Flash is not something we would have to load.
		//
		if (strlen(bullet_identifiers[i]) && !strcmp(bullet_identifiers[i], "flash_dummy"))
			continue;
		if (strlen(bullet_identifiers[i]) && !strcmp(bullet_identifiers[i], "ERROR:  UNHANDLED BULLET IMAGE TYPE"))
			continue;

		for (j = 0; j < Bulletmap[i].phases; j++) {
			for (k = 0; k < BULLET_DIRECTIONS; k++) {
				// We construct the file name
				//
				sprintf(constructed_filename, "bullets/iso_bullet_%s_%02d_%04d.png", bullet_identifiers[i], k, j + 1);
				find_file(constructed_filename, GRAPHICS_DIR, fpath, 0);

				get_iso_image_from_file_and_path(fpath, &(Bulletmap[i].image[k][j]), TRUE);

			}
		}
	}

};				// void iso_load_bullet_surfaces ( void )

/**
 *
 *
 */
void LoadOneSkillSurfaceIfNotYetLoaded(int SkillSpellNr)
{
	SDL_Surface *Whole_Image;
	char fpath[2048];
	char AssembledFileName[2000];

	// Maybe this spell/skill icon surface has already been loaded, i.e. it's not
	// NULL any more.  Then we needn't do anything here.
	//
	if (SpellSkillMap[SkillSpellNr].icon_surface.surface || SpellSkillMap[SkillSpellNr].icon_surface.texture_has_been_created)
		return;

	// Now it's time to assemble the file name to get the image from
	//
	strcpy(AssembledFileName, "skill_icons/");
	strcat(AssembledFileName, SpellSkillMap[SkillSpellNr].icon_name);
	find_file(AssembledFileName, GRAPHICS_DIR, fpath, 0);

	// Now we can load and prepare the image and that's it
	//
	Whole_Image = our_IMG_load_wrapper(fpath);	// This is a surface with alpha channel, since the picture is one of this type
	if (!Whole_Image) {
		ErrorMessage(__FUNCTION__, "\
Freedroid was unable to load skill %d surface into memory\npath tried was %s\n\
This error indicates some installation problem with freedroid.", PLEASE_INFORM, IS_FATAL, SkillSpellNr, fpath);
	}

	SpellSkillMap[SkillSpellNr].icon_surface.surface = our_SDL_display_format_wrapperAlpha(Whole_Image);
	SDL_FreeSurface(Whole_Image);

	SDL_SetColorKey(SpellSkillMap[SkillSpellNr].icon_surface.surface, 0, 0);
	SDL_SetAlpha(SpellSkillMap[SkillSpellNr].icon_surface.surface, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

	if (use_open_gl)
		make_texture_out_of_surface(&(SpellSkillMap[SkillSpellNr].icon_surface));

};				// void LoadOneSkillSurfaceIfNotYetLoaded ( int SkillSpellNr )

/**
 *
 *
 */
void blit_iso_image_to_map_position(iso_image * our_iso_image, float pos_x, float pos_y)
{
	SDL_Rect target_rectangle;
	int ii, jj;

	translate_map_point_to_screen_pixel(pos_x, pos_y, &ii, &jj, 1.0);
	target_rectangle.x = ii + our_iso_image->offset_x;
	target_rectangle.y = jj + our_iso_image->offset_y;

	our_SDL_blit_surface_wrapper(our_iso_image->surface, NULL, Screen, &target_rectangle);

};				// void blit_iso_image_to_map_position ( iso_image * our_iso_image , float pos_x , float pos_y )

/**
 *
 *
 */
void blit_outline_of_iso_image_to_map_position(iso_image * our_iso_image, float pos_x, float pos_y)
{
	SDL_Rect target_rectangle;
	Uint32 previous_test_color = 0;
	Uint32 new_test_color = 0;
	int x, y;
	Uint32 light_frame_color = SDL_MapRGB(Screen->format, 255, 255, 0);
	Uint32 color_key_value = SDL_MapRGB(our_iso_image->surface->format, 255, 0, 255);
	Uint8 previous_test_alpha = 0;
	Uint8 new_test_alpha = 0;

	DebugPrintf(1, "\nblit_outline_of_iso_image_to_map_position: function invoked.");

	translate_map_point_to_screen_pixel(pos_x, pos_y, &x, &y, 1.0);
	target_rectangle.x = x + our_iso_image->offset_x;
	target_rectangle.y = y + our_iso_image->offset_y;

	if (our_iso_image->surface->flags & SDL_SRCCOLORKEY) {

		for (x = 0; x < our_iso_image->surface->w; x++) {
			for (y = 0; y < our_iso_image->surface->h; y++) {

				new_test_color = FdGetPixel(our_iso_image->surface, x, y);
				// DebugPrintf ( -5 , "\nAlpha received: %d." , (int) new_alpha_component );

				if (((new_test_color == color_key_value) && (previous_test_color != color_key_value)) ||
				    ((new_test_color != color_key_value) && (previous_test_color == color_key_value))) {
					if (y != 0) {
						PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y, light_frame_color);
						PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y - 1, light_frame_color);
						// DebugPrintf ( -5 , "\nPIXEL FILLED!" );
					}
				}

				if ((y == 0) && (new_test_color != color_key_value)) {
					PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y - 1, light_frame_color);
					PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y - 2, light_frame_color);
				}

				if ((y == our_iso_image->surface->h - 1) && (new_test_color != color_key_value)) {
					PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y + 1, light_frame_color);
					PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y + 2, light_frame_color);
				}

				previous_test_color = new_test_color;

			}
		}

		for (y = 0; y < our_iso_image->surface->h; y++) {
			for (x = 0; x < our_iso_image->surface->w; x++) {

				new_test_color = FdGetPixel(our_iso_image->surface, x, y);
				// DebugPrintf ( -5 , "\nAlpha received: %d." , (int) new_alpha_component );

				if (((new_test_color == color_key_value) && (previous_test_color != color_key_value)) ||
				    ((new_test_color != color_key_value) && (previous_test_color == color_key_value))) {
					if (x != 0) {
						PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y, light_frame_color);
						PutPixel(Screen, target_rectangle.x + x - 1, target_rectangle.y + y, light_frame_color);
						// DebugPrintf ( -5 , "\nPIXEL FILLED!" );
					}
				}

				if ((x == 0) && (new_test_color != color_key_value)) {
					PutPixel(Screen, target_rectangle.x + x - 1, target_rectangle.y + y, light_frame_color);
					PutPixel(Screen, target_rectangle.x + x - 2, target_rectangle.y + y, light_frame_color);
				}

				if ((x == our_iso_image->surface->w - 1) && (new_test_color != color_key_value)) {
					PutPixel(Screen, target_rectangle.x + x + 1, target_rectangle.y + y, light_frame_color);
					PutPixel(Screen, target_rectangle.x + x + 2, target_rectangle.y + y, light_frame_color);
				}

				previous_test_color = new_test_color;

			}
		}

	} else {

		for (x = 0; x < our_iso_image->surface->w; x++) {
			for (y = 0; y < our_iso_image->surface->h; y++) {

				new_test_alpha = GetAlphaComponent(our_iso_image->surface, x, y);
				// DebugPrintf ( -5 , "\nAlpha received: %d." , (int) new_alpha_component );

				if (((new_test_alpha == SDL_ALPHA_TRANSPARENT) && (previous_test_alpha != SDL_ALPHA_TRANSPARENT)) ||
				    ((new_test_alpha != SDL_ALPHA_TRANSPARENT) && (previous_test_alpha == SDL_ALPHA_TRANSPARENT))) {
					if (y != 0) {
						PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y, light_frame_color);
						PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y - 1, light_frame_color);
						// DebugPrintf ( -5 , "\nPIXEL FILLED!" );
					}
				}

				if ((y == 0) && (new_test_alpha != SDL_ALPHA_TRANSPARENT)) {
					PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y - 1, light_frame_color);
					PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y - 2, light_frame_color);
				}

				if ((y == our_iso_image->surface->h - 1) && (new_test_alpha != SDL_ALPHA_TRANSPARENT)) {
					PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y + 1, light_frame_color);
					PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y + 2, light_frame_color);
				}

				previous_test_alpha = new_test_alpha;

			}
		}

		for (y = 0; y < our_iso_image->surface->h; y++) {
			for (x = 0; x < our_iso_image->surface->w; x++) {

				new_test_alpha = FdGetPixel(our_iso_image->surface, x, y);
				// DebugPrintf ( -5 , "\nAlpha received: %d." , (int) new_alpha_component );

				if (((new_test_alpha == SDL_ALPHA_TRANSPARENT) && (previous_test_alpha != SDL_ALPHA_TRANSPARENT)) ||
				    ((new_test_alpha != SDL_ALPHA_TRANSPARENT) && (previous_test_alpha == SDL_ALPHA_TRANSPARENT))) {
					if (x != 0) {
						PutPixel(Screen, target_rectangle.x + x, target_rectangle.y + y, light_frame_color);
						PutPixel(Screen, target_rectangle.x + x - 1, target_rectangle.y + y, light_frame_color);
						// DebugPrintf ( -5 , "\nPIXEL FILLED!" );
					}
				}

				if ((x == 0) && (new_test_alpha != SDL_ALPHA_TRANSPARENT)) {
					PutPixel(Screen, target_rectangle.x + x - 1, target_rectangle.y + y, light_frame_color);
					PutPixel(Screen, target_rectangle.x + x - 2, target_rectangle.y + y, light_frame_color);
				}

				if ((x == our_iso_image->surface->w - 1) && (new_test_alpha != SDL_ALPHA_TRANSPARENT)) {
					PutPixel(Screen, target_rectangle.x + x + 1, target_rectangle.y + y, light_frame_color);
					PutPixel(Screen, target_rectangle.x + x + 2, target_rectangle.y + y, light_frame_color);
				}

				previous_test_alpha = new_test_alpha;

			}
		}

	}
};				// void blit_outline_of_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y )

/**
 *
 *
 */
void blit_iso_image_to_screen_position(iso_image * our_iso_image, float pos_x, float pos_y)
{
	SDL_Rect target_rectangle;

	// target_rectangle . x = pos_x + our_iso_image -> offset_x ;
	// target_rectangle . y = pos_y + our_iso_image -> offset_y ;
	target_rectangle.x = pos_x;
	target_rectangle.y = pos_y;

	our_SDL_blit_surface_wrapper(our_iso_image->surface, NULL, Screen, &target_rectangle);

};				// void blit_iso_image_to_screen_position ( iso_image * our_iso_image , float pos_x , float pos_y )

/**
 *
 *
 */
void blit_zoomed_iso_image_to_map_position(iso_image * our_iso_image, float pos_x, float pos_y)
{
	SDL_Rect target_rectangle;
	int x, y;
	float zoom_factor = lvledit_zoomfact_inv();

	translate_map_point_to_screen_pixel(pos_x, pos_y, &x, &y, zoom_factor);
	target_rectangle.x = x + our_iso_image->offset_x * zoom_factor;
	target_rectangle.y = y + our_iso_image->offset_y * zoom_factor;

	if (use_open_gl) {
		raise(SIGSEGV);
		/*blit_zoomed_open_gl_texture_to_screen_position ( our_iso_image , target_rectangle . x , 
		   target_rectangle . y , TRUE , 0.25 ) ; */
	} else {
		make_sure_zoomed_surface_is_there(our_iso_image);
		our_SDL_blit_surface_wrapper(our_iso_image->zoomed_out_surface, NULL, Screen, &target_rectangle);
	}

};				// void blit_zoomed_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y )

/**
 *
 *
 */
static void get_offset_for_iso_image_from_file_and_path(char *fpath, iso_image * our_iso_image)
{
	char offset_file_name[10000];
	FILE *OffsetFile;
	char *offset_data;
	// Now we try to load the associated offset file, that we'll be needing
	// in order to properly fine-position the image later when blitting is to
	// a map location.
	//
	strcpy(offset_file_name, fpath);
	offset_file_name[strlen(offset_file_name) - 4] = 0;
	strcat(offset_file_name, ".offset");

	// Let's see if we can find an offset file...
	//
	if ((OffsetFile = fopen(offset_file_name, "rb")) == NULL) {
		ErrorMessage(__FUNCTION__, "\
Freedroid was unable to open offset file %s for an isometric image.\n\
Since the offset could not be obtained from the offset file, some default\n\
values will be used instead.  This can lead to minor positioning pertubations\n\
in graphics displayed, but FreedroidRPG will continue to work.", NO_NEED_TO_INFORM, IS_WARNING_ONLY, offset_file_name);
		our_iso_image->offset_x = -INITIAL_BLOCK_WIDTH / 2;
		our_iso_image->offset_y = -INITIAL_BLOCK_HEIGHT / 2;
		return;
	} else {
		fclose(OffsetFile);
	}

	// So at this point we can be certain, that the offset file is there.
	// That means, that we can now use the (otherwise terminating) read-and-malloc-...
	// functions.
	//
	offset_data = ReadAndMallocAndTerminateFile(offset_file_name, END_OF_OFFSET_FILE_STRING);

	ReadValueFromString(offset_data, OFFSET_FILE_OFFSETX_STRING, "%hd", &(our_iso_image->offset_x), offset_data + strlen(offset_data));

	ReadValueFromString(offset_data, OFFSET_FILE_OFFSETY_STRING, "%hd", &(our_iso_image->offset_y), offset_data + strlen(offset_data));
	free(offset_data);

};				// void get_offset_for_iso_image_from_file_and_path ( fpath , our_iso_image )

/**
 * The concept of an iso image involves an SDL_Surface or an OpenGL
 * texture and also suitable offset values, such that the image can be
 * correctly placed in an isometric image.
 *
 * This function is supposed to load the SDL_Surface (from which the 
 * OpenGL texture can be made later) AND also to load the corresponding
 * offset file for the image (or supply substitute values) such that the
 * offset values are suitably initialized.
 */
void get_iso_image_from_file_and_path(char *fpath, iso_image * our_iso_image, int use_offset_file)
{
	SDL_Surface *Whole_Image;

	// First we (try to) load the image given in the parameter
	// from hard disk into memory and convert it to the right
	// format for fast blitting later.
	//
	Whole_Image = our_IMG_load_wrapper(fpath);	// This is a surface with alpha channel, since the picture is one of this type
	if (Whole_Image == NULL) {
		fprintf(stderr, "\n\nfpath: '%s'\n", fpath);
		ErrorMessage(__FUNCTION__, "Could not load image\n File name: %s \n", PLEASE_INFORM, IS_FATAL, fpath);
	}

	SDL_SetAlpha(Whole_Image, 0, SDL_ALPHA_OPAQUE);
	our_iso_image->surface = our_SDL_display_format_wrapperAlpha(Whole_Image);	// now we have an alpha-surf of right size
	our_iso_image->zoomed_out_surface = NULL;
	our_iso_image->texture_has_been_created = FALSE;

	SDL_SetColorKey(our_iso_image->surface, 0, 0);	// this should clear any color key in the dest surface
	// Some test here...
	//
	// our_iso_image -> surface -> format -> Bmask = 0 ; 
	// our_iso_image -> surface -> format -> Rmask = 0 ; 

	SDL_FreeSurface(Whole_Image);

	// Now that we have loaded the image, it's time to get the proper
	// offset information for it.
	//
	if (use_offset_file)
		get_offset_for_iso_image_from_file_and_path(fpath, our_iso_image);
	else {
		// We _silently_ assume there is no offset file...
		//
		our_iso_image->offset_x = -INITIAL_BLOCK_WIDTH / 2;
		our_iso_image->offset_y = -INITIAL_BLOCK_HEIGHT / 2;
	}

	// In the case of no open_gl (and therefore no conversion to a texture)
	// we make sure, that the open_gl optiomized methods will also find
	// suitable correspondents in the SDL-loaded images, like the original
	// image size and such...
	//
	our_iso_image->original_image_width = our_iso_image->surface->w;
	our_iso_image->original_image_height = our_iso_image->surface->h;

};				// void get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image ) 

/**
 *
 *
 */
static void get_iso_image_with_colorkey_from_file_and_path(char *fpath, iso_image * our_iso_image)
{
	SDL_Surface *Whole_Image;
	int x, y;
	Uint32 color_key_value;

	// First we (try to) load the image given in the parameter
	// from hard disk into memory and convert it to the right
	// format for fast blitting later.
	//
	Whole_Image = our_IMG_load_wrapper(fpath);	// This is a surface with alpha channel, since the picture is one of this type
	if (Whole_Image == NULL) {
		ErrorMessage(__FUNCTION__, "\
Freedroid was unable to load file %s into memory.\n\
This error indicates some installation problem with freedroid.", PLEASE_INFORM, IS_FATAL, fpath);
	}

	SDL_SetAlpha(Whole_Image, 0, SDL_ALPHA_OPAQUE);
	our_iso_image->surface = SDL_DisplayFormat(Whole_Image);	// now we have an alpha-surf of right size

	color_key_value = SDL_MapRGB(our_iso_image->surface->format, 255, 0, 255);

	for (x = 0; x < Whole_Image->w; x++) {
		for (y = 0; y < Whole_Image->h; y++) {
			// Any pixel that is halfway transparent will now be made 
			// into the color key...
			//
			if (GetAlphaComponent(Whole_Image, x, y) < 50) {
				PutPixel(our_iso_image->surface, x, y, color_key_value);
			}
		}
	}

	our_iso_image->zoomed_out_surface = NULL;
	SDL_SetColorKey(our_iso_image->surface, SDL_SRCCOLORKEY, color_key_value);	// this should clear any color key in the dest surface

	// Some test here...
	//
	// our_iso_image -> surface -> format -> Bmask = 0 ; 
	// our_iso_image -> surface -> format -> Rmask = 0 ; 
	//
	SDL_FreeSurface(Whole_Image);

	// Now that we have loaded the image, it's time to get the proper
	// offset information for it.
	//
	get_offset_for_iso_image_from_file_and_path(fpath, our_iso_image);

	// Now finally we need to set the 'original_image_width/height', because
	// this is the default value used with both, OpenGL and SDL.
	//
	our_iso_image->original_image_width = our_iso_image->surface->w;
	our_iso_image->original_image_height = our_iso_image->surface->h;

};				// void get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image ) 

/**
 *
 *
 */
void LoadAndPrepareEnemyRotationModelNr(int ModelNr)
{
	int i;
	static int FirstCallEver = TRUE;
	static int EnemyFullyPrepared[ENEMY_ROTATION_MODELS_AVAILABLE];

	// Maybe this function has just been called for the first time ever.
	// Then of course we need to initialize the array, that is used for
	// keeping track of the currently loaded enemy rotation surfaces.
	// This we do here.
	//
	if (FirstCallEver) {
		for (i = 0; i < ENEMY_ROTATION_MODELS_AVAILABLE; i++) {
			EnemyFullyPrepared[i] = FALSE;
		}
		FirstCallEver = FALSE;
	}
	// Now a sanity check against using rotation types, that don't exist
	// in Freedroid RPG at all!
	//
	if ((ModelNr < 0) || (ModelNr >= ENEMY_ROTATION_MODELS_AVAILABLE)) {
		ErrorMessage(__FUNCTION__, "\
Freedroid received a rotation model number that does not exist: %d\n", PLEASE_INFORM, IS_FATAL, ModelNr);
	}
	// Now we can check if the given rotation model type was perhaps already
	// allocated and loaded and fully prepared.  Then of course we need not 
	// do anything here...  Otherwise we can have trust and mark it as loaded
	// already...
	//
	if (EnemyFullyPrepared[ModelNr])
		return;
	EnemyFullyPrepared[ModelNr] = TRUE;
	Activate_Conservative_Frame_Computation();

	grab_enemy_images_from_archive(ModelNr);
	return;

};				// void LoadAndPrepareEnemyRotationModelNr ( int j )

/**
 * If needed, we will assemble differently colored versions of the enemy
 * rotation models...
 */
void LoadAndPrepareGreenEnemyRotationModelNr(int ModelNr)
{
	int i;
	static int FirstCallEver = TRUE;
	static int EnemyFullyPrepared[ENEMY_ROTATION_MODELS_AVAILABLE];

	// Maybe this function has just been called for the first time ever.
	// Then of course we need to initialize the array, that is used for
	// keeping track of the currently loaded enemy rotation surfaces.
	// This we do here.
	//
	if (FirstCallEver) {
		for (i = 0; i < ENEMY_ROTATION_MODELS_AVAILABLE; i++) {
			EnemyFullyPrepared[i] = FALSE;
		}
		FirstCallEver = FALSE;
	}
	// Now we can check if the given rotation model type was perhaps already
	// allocated and loaded and fully prepared.  Then of course we need not 
	// do anything here...  Otherwise we can have trust and mark it as loaded
	// already...
	//
	if (EnemyFullyPrepared[ModelNr])
		return;
	EnemyFullyPrepared[ModelNr] = TRUE;
	Activate_Conservative_Frame_Computation();

	// Now that we have our enemy surfaces ready, we can create some modified
	// copies of those surfaces but this a color filter applied to them...
	//
	// But of course, this only needs to be done, if there is no OpenGL present
	// on this machine, cause OpenGL can do that color filtering on the fly
	// anyway, so no need to waste memory for this...
	//
	if (!use_open_gl) {
		for (i = 0; i < ROTATION_ANGLES_PER_ROTATION_MODEL; i++) {
			GreenEnemyRotationSurfacePointer[ModelNr][i][0].surface =
			    CreateColorFilteredSurface(enemy_iso_images[ModelNr][i][0].surface, FILTER_GREEN);
			GreenEnemyRotationSurfacePointer[ModelNr][i][0].offset_x = enemy_iso_images[ModelNr][i][0].offset_x;
			GreenEnemyRotationSurfacePointer[ModelNr][i][0].offset_y = enemy_iso_images[ModelNr][i][0].offset_y;
		}
	}
};				// void LoadAndPrepareGreenEnemyRotationModelNr ( int ModelNr )

/**
 * If needed, we will assemble differently colored versions of the enemy
 * rotation models...
 */
void LoadAndPrepareBlueEnemyRotationModelNr(int ModelNr)
{
	int i;
	static int FirstCallEver = TRUE;
	static int EnemyFullyPrepared[ENEMY_ROTATION_MODELS_AVAILABLE];

	// Maybe this function has just been called for the first time ever.
	// Then of course we need to initialize the array, that is used for
	// keeping track of the currently loaded enemy rotation surfaces.
	// This we do here.
	//
	if (FirstCallEver) {
		for (i = 0; i < ENEMY_ROTATION_MODELS_AVAILABLE; i++) {
			EnemyFullyPrepared[i] = FALSE;
		}
		FirstCallEver = FALSE;
	}
	// Now we can check if the given rotation model type was perhaps already
	// allocated and loaded and fully prepared.  Then of course we need not 
	// do anything here...  Otherwise we can have trust and mark it as loaded
	// already...
	//
	if (EnemyFullyPrepared[ModelNr])
		return;
	EnemyFullyPrepared[ModelNr] = TRUE;
	Activate_Conservative_Frame_Computation();

	// Now that we have our enemy surfaces ready, we can create some modified
	// copies of those surfaces but this a color filter applied to them...
	//
	for (i = 0; i < ROTATION_ANGLES_PER_ROTATION_MODEL; i++) {
		BlueEnemyRotationSurfacePointer[ModelNr][i][0].surface =
		    CreateColorFilteredSurface(enemy_iso_images[ModelNr][i][0].surface, FILTER_BLUE);
		BlueEnemyRotationSurfacePointer[ModelNr][i][0].offset_x = enemy_iso_images[ModelNr][i][0].offset_x;
		BlueEnemyRotationSurfacePointer[ModelNr][i][0].offset_y = enemy_iso_images[ModelNr][i][0].offset_y;
	}
};				// void LoadAndPrepareBlueEnemyRotationModelNr ( int ModelNr )

/**
 * If needed, we will assemble differently colored versions of the enemy
 * rotation models...
 */
void LoadAndPrepareRedEnemyRotationModelNr(int ModelNr)
{
	int i;
	static int FirstCallEver = TRUE;
	static int EnemyFullyPrepared[ENEMY_ROTATION_MODELS_AVAILABLE];

	// Maybe this function has just been called for the first time ever.
	// Then of course we need to initialize the array, that is used for
	// keeping track of the currently loaded enemy rotation surfaces.
	// This we do here.
	//
	if (FirstCallEver) {
		for (i = 0; i < ENEMY_ROTATION_MODELS_AVAILABLE; i++) {
			EnemyFullyPrepared[i] = FALSE;
		}
		FirstCallEver = FALSE;
	}
	// Now we can check if the given rotation model type was perhaps already
	// allocated and loaded and fully prepared.  Then of course we need not 
	// do anything here...  Otherwise we can have trust and mark it as loaded
	// already...
	//
	if (EnemyFullyPrepared[ModelNr])
		return;
	EnemyFullyPrepared[ModelNr] = TRUE;
	Activate_Conservative_Frame_Computation();

	// Now that we have our enemy surfaces ready, we can create some modified
	// copies of those surfaces but this a color filter applied to them...
	//
	for (i = 0; i < ROTATION_ANGLES_PER_ROTATION_MODEL; i++) {
		RedEnemyRotationSurfacePointer[ModelNr][i][0].surface =
		    CreateColorFilteredSurface(enemy_iso_images[ModelNr][i][0].surface, FILTER_RED);
		RedEnemyRotationSurfacePointer[ModelNr][i][0].offset_x = enemy_iso_images[ModelNr][i][0].offset_x;
		RedEnemyRotationSurfacePointer[ModelNr][i][0].offset_y = enemy_iso_images[ModelNr][i][0].offset_y;
	}
};				// void LoadAndPrepareRedEnemyRotationModelNr ( int ModelNr )

/**
 * Read the Enemy Surfaces details from the data stream.
 */
void get_enemy_surfaces_data(char *DataPointer)
{
	char *SurfacePointer;
	char *EndOfSurfaceData;
	int SurfaceIndex = 0;

#define ENEMY_SURFACES_SECTION_BEGIN_STRING "*** Start of Enemy Surfaces Section: ***"
#define ENEMY_SURFACES_SECTION_END_STRING "*** End of Enemy Surfaces Section: ***"
#define NEW_SURFACE_BEGIN_STRING "** Start of new surface specification subsection **"

#define SURFACES_FILE_NAME_BEGIN_STRING "PrefixToFilename=\""
#define SURFACES_WALK_ANI_SPEED_BEGIN_STRING "droid_walk_animation_speed_factor="
#define SURFACES_ATTACK_ANI_SPEED_BEGIN_STRING "droid_attack_animation_speed_factor="
#define SURFACES_GETHIT_ANI_SPEED_BEGIN_STRING "droid_gethit_animation_speed_factor="
#define SURFACES_DEATH_ANI_SPEED_BEGIN_STRING "droid_death_animation_speed_factor="
#define SURFACES_STAND_ANI_SPEED_BEGIN_STRING "droid_stand_animation_speed_factor="


	SurfacePointer = LocateStringInData(DataPointer, ENEMY_SURFACES_SECTION_BEGIN_STRING);
	EndOfSurfaceData = LocateStringInData(DataPointer, ENEMY_SURFACES_SECTION_END_STRING);

 	DebugPrintf(1, "\n\nStarting to read surfaces data...\n\n");

	SurfacePointer = DataPointer;

	while ((SurfacePointer = strstr(SurfacePointer, NEW_SURFACE_BEGIN_STRING)) != NULL) {
 		DebugPrintf(1, "\n\nFound another surface specification entry!  Lets add that to the others!");
		SurfacePointer++;

		PrefixToFilename[SurfaceIndex] = ReadAndMallocStringFromData(SurfacePointer, SURFACES_FILE_NAME_BEGIN_STRING, "\"");

		ReadValueFromStringWithDefault(SurfacePointer, SURFACES_WALK_ANI_SPEED_BEGIN_STRING,
			"%d", "0", &(droid_walk_animation_speed_factor[SurfaceIndex]), EndOfSurfaceData);
		ReadValueFromStringWithDefault(SurfacePointer, SURFACES_ATTACK_ANI_SPEED_BEGIN_STRING,
			"%d", "0", &(droid_attack_animation_speed_factor[SurfaceIndex]), EndOfSurfaceData);
		ReadValueFromStringWithDefault(SurfacePointer, SURFACES_GETHIT_ANI_SPEED_BEGIN_STRING,
			"%d", "0", &(droid_gethit_animation_speed_factor[SurfaceIndex]), EndOfSurfaceData);
		ReadValueFromStringWithDefault(SurfacePointer, SURFACES_DEATH_ANI_SPEED_BEGIN_STRING,
			"%d", "0", &(droid_death_animation_speed_factor[SurfaceIndex]), EndOfSurfaceData);
		ReadValueFromStringWithDefault(SurfacePointer, SURFACES_STAND_ANI_SPEED_BEGIN_STRING,
			"%d", "0", &(droid_stand_animation_speed_factor[SurfaceIndex]), EndOfSurfaceData);

		SurfaceIndex++;
	}

 	DebugPrintf(1, "\nEnd of get_enemy_surfaces_data ( char* DataPointer ) reached.");
}

/**
 * This function creates all the surfaces, that are nescessary to blit the
 * 'head' and 'shoes' of an enemy.  The numbers are not dealt with here.
 */
void Load_Enemy_Surfaces(void)
{
	int i;
	int j;

	// We clean out the rotated enemy surface pointers, so that later we
	// can judge securely which of them have been initialized (non-Null)
	// and which of them have not.
	//
	for (j = 0; j < ENEMY_ROTATION_MODELS_AVAILABLE; j++) {
		for (i = 0; i < ROTATION_ANGLES_PER_ROTATION_MODEL; i++) {
			enemy_iso_images[j][i][0].surface = NULL;
		}
	}

	// When using the new tux image collection files, the animation cycle
	// lengthes for droids will be taken from the image collection file itself.
	// That is good, because it's so dynamic.  However, it also means, that
	// the real animation phase lengthes and that will in general not be known
	// until the graphics for that bot has been loaded.  But on the other hand
	// it might happen that some phase computation is done before the first
	// blit already.  In that case, uninitialized data structs might cause 
	// severe harm.  Therefore we initialize some sane default values, that should
	// protect against certain cases of wrong phase counts.
	//
	for (i = 0; i < ENEMY_ROTATION_MODELS_AVAILABLE; i++) {
		first_walk_animation_image[i] = 1;
		last_walk_animation_image[i] = 1;
		first_attack_animation_image[i] = 1;
		last_attack_animation_image[i] = 1;
		first_gethit_animation_image[i] = 1;
		last_gethit_animation_image[i] = 1;
		first_death_animation_image[i] = 1;
		last_death_animation_image[i] = 1;
		first_stand_animation_image[i] = 1;
		last_stand_animation_image[i] = 1;
		use_default_attack_image[i] = TRUE;
		use_default_gethit_image[i] = TRUE;
		use_default_death_image[i] = TRUE;
		use_default_stand_image[i] = TRUE;
	}


	char fpath[2048];
	char *Data;

	find_file("freedroid.enemy_surfaces", MAP_DIR, fpath, 0);
	Data = ReadAndMallocAndTerminateFile(fpath, "*** End of this Freedroid data File ***");
	get_enemy_surfaces_data(Data);
	free(Data);


	// Finally we do some test to make sure we don't write
	// over the bounds of our array or so
	//
	if (i >= ENEMY_ROTATION_MODELS_AVAILABLE) {
		ErrorMessage(__FUNCTION__, "\
There are %d models mentioned in freedroid.enemy_surfaces, but\n\
only ENEMY_ROTATION_MODELS_AVAILABLE is only %d.\n\
This should be investigated as soon as possible.", PLEASE_INFORM, IS_FATAL, i, ENEMY_ROTATION_MODELS_AVAILABLE);
	}
};				// void LoadEnemySurfaces( void )

/**
 * Return a pointer towards the iso_image structure
 * associated to the given obstacle type.
 * Used for lazy loading.
 */
iso_image *get_obstacle_image(int type)
{
	if (!obstacle_map[type].image_loaded) {
		//printf("Just in time loading for obstacle %d\n", type);
		load_obstacle(type);
	}

	return &obstacle_map[type].image;
}

/**
 * Load the images associated to the given
 * obstacle type.
 */
void load_obstacle(int i)
{
	char fpath[2048];
	char ConstructedFileName[2000];
	char shadow_file_name[2000];

	if (obstacle_map[i].image_loaded) {
		ErrorMessage(__FUNCTION__, "Tried to load image for obstacle type %d that was already loaded.\n", PLEASE_INFORM,
			     IS_WARNING_ONLY, i);
		return;
	}
	// At first we construct the file name of the single tile file we are about to load...
	//
	strcpy(ConstructedFileName, "obstacles/");
	strcat(ConstructedFileName, obstacle_map[i].filename);
	find_file(ConstructedFileName, GRAPHICS_DIR, fpath, 0);

	if (use_open_gl) {
		get_iso_image_from_file_and_path(fpath, &(obstacle_map[i].image), TRUE);
		//make_sure_zoomed_surface_is_there ( & ( obstacle_map [ i ] . image ) ); 
		make_sure_automap_surface_is_there(&(obstacle_map[i]));

		make_texture_out_of_surface(&(obstacle_map[i].image));
	} else
		get_iso_image_with_colorkey_from_file_and_path(fpath, &(obstacle_map[i].image));

	obstacle_map[i].image_loaded = 1;

	// Maybe the obstacle in question also has a shadow image?  In that
	// case we should load the shadow image now.  Otherwise we might just
	// mark the shadow image as not in use, so we won't face problems with
	// missing shadow images inside the code
	//
	// We need a new file name of course:  (this assumes, that the filename
	// has been constructed above already...
	if (strlen(ConstructedFileName) >= 8) {
		strcpy(shadow_file_name, ConstructedFileName);
		shadow_file_name[strlen(shadow_file_name) - 8] = 0;
		strcat(shadow_file_name, "shadow_");
		strcat(shadow_file_name, &(ConstructedFileName[strlen(ConstructedFileName) - 8]));
		DebugPrintf(2, "\n%s(): shadow file name: %s ", __FUNCTION__, shadow_file_name);
		if (find_file(shadow_file_name, GRAPHICS_DIR, fpath, 1)) {
			obstacle_map[i].shadow_image.surface = NULL;
			obstacle_map[i].shadow_image.texture_has_been_created = FALSE;
			DebugPrintf(2, "\n%s(): no success with that last shadow image file name.", __FUNCTION__);
			return;
		}
	}

	if (use_open_gl) {
		get_iso_image_from_file_and_path(fpath, &(obstacle_map[i].shadow_image), TRUE);
		// make_sure_zoomed_surface_is_there ( & ( obstacle_map [ i ] . shadow_image ) ); 
		make_texture_out_of_surface(&(obstacle_map[i].shadow_image));
	} else
		get_iso_image_with_colorkey_from_file_and_path(fpath, &(obstacle_map[i].shadow_image));

	DebugPrintf(1, "\n%s(): shadow image %s loaded successfully.", __FUNCTION__, shadow_file_name);

}

void load_all_obstacles(void)
{
	int i;

	for (i = 0; i < NUMBER_OF_OBSTACLE_TYPES; i++) {
		load_obstacle(i);
	}

};				// void load_all_obstacles ( void )

/**
 * This function loads isometric floor tiles, and in OpenGL mode, generates
 * a texture atlas.
 *
 */
void load_floor_tiles(void)
{
	int i;
	char fpath[2048];

	// Try to load the atlas
	if (use_open_gl)
		if (!load_texture_atlas
		    ("floor_tiles/atlas.txt", "floor_tiles/", floor_tile_filenames, floor_iso_images, ALL_ISOMETRIC_FLOOR_TILES)) {
			return;
		}

	// No atlas possible
	for (i = 0; i < ALL_ISOMETRIC_FLOOR_TILES; i++) {
		char ConstructedFileName[2000];

		strcpy(ConstructedFileName, "floor_tiles/");
		strcat(ConstructedFileName, floor_tile_filenames[i]);
		find_file(ConstructedFileName, GRAPHICS_DIR, fpath, 0);

		if (use_open_gl) {
			get_iso_image_from_file_and_path(fpath, &(floor_iso_images[i]), TRUE);
			make_texture_out_of_surface(&(floor_iso_images[i]));
		} else {
			get_iso_image_with_colorkey_from_file_and_path(fpath, &(floor_iso_images[i]));
		}
	}

};				// void load_floor_tiles ( void )

#undef _blocks_c
