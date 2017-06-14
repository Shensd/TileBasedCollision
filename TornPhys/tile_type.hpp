#pragma once
#ifndef TILE_TYPE_HPP
#define TILE_TYPE_HPP

#include <allegro5\allegro.h>
#include <allegro5\allegro_color.h>

class TileType {
public:
	char identifier;
	bool is_solid, is_spawn = false, draw_wireframe = false, has_exec = false;
	void (*exec)();
	ALLEGRO_COLOR color;
	TileType(char, bool, ALLEGRO_COLOR);
	TileType();
	void add_exec(void(*exec)());
};

#endif // !TILE_TYPE_HPP