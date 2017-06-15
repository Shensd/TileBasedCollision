#include "tile_type.hpp"

TileType::TileType(char identifier, bool is_solid) : identifier(identifier), is_solid(is_solid) {}

TileType::TileType() {}

void TileType::add_exec(void(*exec)()) {
	TileType::exec = exec;
	has_exec = true;
}

void TileType::add_color(ALLEGRO_COLOR color) {
	TileType::color = color;
	has_color = true;
}

void TileType::add_texture(char* path) {
	TileType::image = al_load_bitmap(path);
	if (!image) {
		throw std::runtime_error("Unable to load image");
	}
	has_image = true;
}