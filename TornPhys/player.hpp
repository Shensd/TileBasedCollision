#pragma once
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "tile.hpp"
#include "tile_type.hpp"
#include "map.hpp"

#include <iostream>

class Player {
public:
	bool jumping = false, falling = false;
	float speed, gravity;
	float jump_height;
	float x, y, w, h;
	float vel_x = 0, vel_y = 0;
	float max_vel_y = -15;
	float map_offset_x = 0, map_offset_y = 0;
	Map *current_map;
	//float max_vel_x, max_vel_y;
	Player(float, float, float, float, float, float, float, Map*);
	Player();
	void update_collision();
private:
	void _vertical();
	void _horizontal();
};

#endif // !PLAYER_HPP