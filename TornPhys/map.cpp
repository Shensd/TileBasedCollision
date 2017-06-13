#include "map.hpp"

//constructor, choose file path, tile render width and height, and the characters used in parsing
Map::Map(char* file_path, float tile_w, float tile_h, char air, char solid, char spawn) {
	Map::map_file_path = file_path;

	Map::tile_w = tile_w;
	Map::tile_h = tile_h;

	Map::air = air;
	Map::solid = solid;
	Map::spawn = spawn;

	Map::map_file.open(file_path);
	if (!map_file.is_open()) {
		throw std::runtime_error("Inavlid path to map file");
	}
	_find_max();
	_find_spawn();
}

//used to find the longest line in the file
void Map::_find_max(void) {
	std::string line;
	int max_x = 0;
	while (std::getline(map_file, line)) {
		lines.push_back(line);
		if (line.length() > max_x) {
			max_x = line.length();
		}
	}
	Map::max_w = max_x;
	Map::max_h = lines.size();
}

//finds the last spawn point listed in the file
void Map::_find_spawn(void) {
	for (int i = 0; i < lines.size(); i++) {
		for (int j = 0; j < lines.at(i).size(); j++) {
			if (lines.at(i).at(j) == Map::spawn) {
				Map::spawn_x = j * tile_w;
				Map::spawn_y = i * tile_h;
			}
		}
	}
}

//renders the map with an optional offset
void Map::render_map(float offset_x, float offset_y) {

	for (int i = 0; i < max_h; i++) {
		for (int j = 0; j < max_w; j++) {
			char tile;
			if (lines.at(i).length() < max_w) {
				tile = '0';
			} else {
				tile = lines.at(i).at(j);
			}
			ALLEGRO_COLOR color;

			if (tile == air) {
				color = al_map_rgb(255, 255, 255);
			} else if (tile == solid) {
				color = al_map_rgb(0, 0, 0);
			} else if (tile == spawn) {
				color = al_map_rgb(255, 255, 255);
			} else {
				color = al_map_rgb(255, 255, 255);
			}

			al_draw_filled_rectangle(
				(j * tile_w) + offset_x,
				(i * tile_h) + offset_y,
				((j * tile_w) + tile_w) + offset_x, 
				((i * tile_h) + tile_h) + offset_y, color);
		}
	}
}

//used to get the tile type at a given location
char Map::get_tile_type_at(float x, float y) {
	//round the x y coords given and divide them to grab the tile from the map array
	int x_con = int((x / tile_w));
	int y_con = int((y / tile_w));

	//if they are behind the map return air
	if (y < 0 || x < 0) {
		return air;
	}
	//if they are outside of it return solid
	if (x_con > max_w || y_con > max_h - 1) {
		return solid;
	}
	return lines.at(y_con).at(x_con);
}

//used to return the tile at a given location
Tile Map::get_tile_at(float x, float y) {
	//round the x y coords given 
	int x_con = int((x / tile_w)) * tile_w;
	int y_con = int((y / tile_h)) * tile_h;
	//get the tile type at the given location
	char type = get_tile_type_at(x, y);

	//return the tile
	return Tile(type, x_con, y_con, tile_w, tile_h);
}