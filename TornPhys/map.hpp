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

class Map {
public:
	std::ifstream map_file;
	std::vector<std::string> lines;
	char* map_file_path;
	int max_w = 0, max_h = 0;
	float spawn_x, spawn_y;
	float tile_w, tile_h;
	char air, solid, spawn;
	Map(char*, float, float, char = '0', char = '1', char = '2');
	void render_map(float = 0, float = 0);
	char get_tile_type_at(float, float);
	Tile get_tile_at(float, float);
private:
	void _find_max(void);
	void _find_spawn(void);
};

#endif // !MAP_HPP