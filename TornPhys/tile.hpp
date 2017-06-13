#pragma once
#ifndef TILE_HPP
#define TILE_HPP

class Tile {
public:
	char type;
	float x, y, w, h;
	Tile(char, float, float, float, float);
	Tile();
};

#endif // !TILE_HPP