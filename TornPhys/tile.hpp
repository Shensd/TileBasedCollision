#pragma once
#ifndef TILE_HPP
#define TILE_HPP

#include "tile_type.hpp"

class Tile {
public:
	TileType type;
	float x, y, w, h;
	Tile(TileType, float, float, float, float);
	Tile();
};

#endif // !TILE_HPP