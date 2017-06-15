#include "map.hpp"

//constructor, choose file path, tile render width and height, and the characters used in parsing
Map::Map(char* file_path, float tile_w, float tile_h) {
	Map::map_file_path = file_path;

	Map::tile_w = tile_w;
	Map::tile_h = tile_h;

	Map::map_file.open(file_path);
	if (!map_file.is_open()) {
		throw std::runtime_error("Inavlid path to map file");
	}
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
	//don't you just love for loops?
	for (int i = 0; i < lines.size(); i++) {
		for (int j = 0; j < lines.at(i).size(); j++) {

			for (int k = 0; k < tile_types.size(); k++) {
				if (tile_types.at(k).identifier == lines.at(i).at(j) &&
					tile_types.at(k).is_spawn) {
					Map::spawn_x = j * tile_w;
					Map::spawn_y = i * tile_h;
					return;
				}
			}

		}
	}
}

void Map::parse_map() {
	_map_parsed = true;
	_find_max();
	_find_spawn();
}

void Map::add_tile_type(TileType tile_type) {
	tile_types.push_back(tile_type);
}

//renders the map with an optional offset
void Map::render_map(float offset_x, float offset_y) {

	if (!_map_parsed) {
		throw std::runtime_error("Map has not been parsed! call parse_map to parse.");
	}
	
	al_hold_bitmap_drawing(true);

	for (int i = 0; i < max_h; i++) {
		for (int j = 0; j < max_w; j++) {
			char tile;
			if (lines.at(i).length() < max_w) {
				tile = '0';
			} else {
				tile = lines.at(i).at(j);
			}
			TileType temp;

			for (int k = 0; k < tile_types.size(); k++) {
				if (tile_types.at(k).identifier == tile) {
					temp = tile_types.at(k);
				}
			}

			if (temp.identifier == NULL) {
				temp = TileType('?', false);
				temp.color = al_map_rgb(220, 66, 244);
			}

			if (temp.has_image) {
				al_draw_bitmap(temp.image, (j * tile_w) + offset_x, (i * tile_h) + offset_y, 0);
			} else {
				if (temp.draw_wireframe) {
					al_draw_rectangle(
						(j * tile_w) + offset_x,
						(i * tile_h) + offset_y,
						((j * tile_w) + tile_w) + offset_x,
						((i * tile_h) + tile_h) + offset_y, temp.color, 3.0);
				} else {
					al_draw_filled_rectangle(
						(j * tile_w) + offset_x,
						(i * tile_h) + offset_y,
						((j * tile_w) + tile_w) + offset_x,
						((i * tile_h) + tile_h) + offset_y, temp.color);
				}
			}
		}
	}

	al_hold_bitmap_drawing(false);
}

//used to get the tile type at a given location
TileType Map::get_tile_type_at(float x, float y, float offset_x, float offset_y) {
	//round the x y coords given and divide them to grab the tile from the map array
	int x_con = int(((x + offset_x) / tile_w));
	int y_con = int(((y + offset_y) / tile_w));

	//if they are behind the map return air
	if (y < 0 || x < 0) {
		for (int i = 0; i < tile_types.size(); i++) {
			if (!tile_types.at(i).is_solid) {
				return tile_types.at(i);
			}
		}
	}
	//if they are outside of it return solid
	if (x_con > max_w || y_con > max_h - 1) {
		for (int i = 0; i < tile_types.size(); i++) {
			if (tile_types.at(i).is_solid) {
				return tile_types.at(i);
			}
		}
	}

	for (int i = 0; i < tile_types.size(); i++) {
		if (tile_types.at(i).identifier == lines.at(y_con).at(x_con)) {
			return tile_types.at(i);
		}
	}

	return TileType();
}

//used to return the tile at a given location
Tile Map::get_tile_at(float x, float y, float offset_x, float offset_y) {
	//round the x y coords given 
	int x_con = (int(((x + offset_x) / tile_w)) * tile_w);
	int y_con = (int(((y + offset_y) / tile_h)) * tile_h);
	//get the tile type at the given location
	TileType type = get_tile_type_at(x, y, offset_x, offset_y);

	if (type.identifier != NULL) {
		return Tile(type, x_con, y_con, tile_w, tile_h);
	} else {
		return Tile(TileType(), x_con, y_con, tile_w, tile_h);
	}
}