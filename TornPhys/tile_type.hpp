#pragma once
#ifndef TILE_TYPE_HPP
#define TILE_TYPE_HPP

#include <allegro5\allegro.h>
#include <allegro5\allegro_color.h>
#include <allegro5\allegro_image.h>

#include <stdexcept>

class TileType {
public:
	char identifier;
	bool is_solid, is_spawn = false, draw_wireframe = false, has_exec = false, has_image = false, has_color = false;
	void (*exec)();
	ALLEGRO_COLOR color;
	ALLEGRO_BITMAP *image = NULL;
	TileType(char, bool);
	TileType();
	void add_exec(void(*exec)());
	void add_color(ALLEGRO_COLOR);
	void add_texture(char*);
};

#endif // !TILE_TYPE_HPP