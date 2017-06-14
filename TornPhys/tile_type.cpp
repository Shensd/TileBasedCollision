#include "tile_type.hpp"

TileType::TileType(char identifier, bool is_solid, ALLEGRO_COLOR color) : identifier(identifier), is_solid(is_solid), color(color) {}

TileType::TileType() {}

void TileType::add_exec(void(*exec)()) {
	TileType::exec = exec;
	has_exec = true;
}