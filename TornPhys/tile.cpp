#include "tile.hpp"

Tile::Tile(char type, float x, float y, float w, float h) {
	Tile::type = type;
	Tile::x = x;
	Tile::y = y;
	Tile::w = w;
	Tile::h = h;
}

Tile::Tile() {}