#pragma once
#ifndef MAP_HPP
#define MAP_HPP

#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\color.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "tile.hpp"
#include "tile_type.hpp"

class Map {
public:
	std::ifstream map_file;
	std::vector<std::string> lines;
	std::vector<TileType> tile_types;
	char* map_file_path;
	int max_w = 0, max_h = 0;
	float spawn_x, spawn_y;
	float tile_w, tile_h;
	char air, solid, spawn;
	ALLEGRO_COLOR backdrop = al_map_rgb(255, 255, 255);
	Map(char*, float, float);
	void add_tile_type(TileType);
	void render_map(float = 0, float = 0);
	void parse_map();
	TileType get_tile_type_at(float, float, float = 0, float = 0);
	Tile get_tile_at(float, float, float = 0, float = 0);
private:
	void _find_max(void);
	void _find_spawn(void);
	bool _map_parsed = false;
};

#endif // !MAP_HPP