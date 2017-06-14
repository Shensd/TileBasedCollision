#include "player.hpp"

Player::Player(float x, float y, float w, float h, float speed, float gravity, float jump_height, Map *current_map) : x(x), y(y), w(w), h(h), speed(speed), gravity(gravity), jump_height(-jump_height), current_map(current_map) {}
Player::Player() {}

void Player::update_collision() {
	_vertical();
	_horizontal();

	Tile top_left = current_map->get_tile_at(x, y, map_offset_x, map_offset_y);
	Tile top_right = current_map->get_tile_at(x + w, y, map_offset_x, map_offset_y);
	Tile btm_left = current_map->get_tile_at(x, y + h, map_offset_x, map_offset_y);
	Tile btm_right = current_map->get_tile_at(x + w, y + h, map_offset_x, map_offset_y);

	if (top_left.type.has_exec) top_left.type.exec();
	if (top_right.type.has_exec) top_right.type.exec();
	if (btm_left.type.has_exec) btm_left.type.exec();
	if (btm_right.type.has_exec) btm_right.type.exec();
}

void Player::_vertical() {
	//test if player is jumping
	if (jumping) {
		//Add an offset to better orient collision tiles
		int offset = w / 2;

		//Select corresponding tiles
		Tile floor_left = current_map->get_tile_at(x + offset + vel_x + (w / 2), y + h, map_offset_x, map_offset_y);
		Tile floor_right = current_map->get_tile_at(x + offset + vel_x - (w / 2), y + h, map_offset_x, map_offset_y);
		Tile ceiling_left = current_map->get_tile_at(x + offset + vel_x + (w / 2), y - h, map_offset_x, map_offset_y);
		Tile ceiling_right = current_map->get_tile_at(x + offset + vel_x - (w / 2), y - h, map_offset_x, map_offset_y);

		//Render them if in vertical debug mode
#ifdef V_DEBUG
		render_queue.push_back(tile_to_rect(ceiling_left));
		render_queue.push_back(tile_to_rect(ceiling_right));
		render_queue.push_back(tile_to_rect(floor_left));
		render_queue.push_back(tile_to_rect(floor_right));
#endif

		if (vel_y < 0) { //going up
			if (ceiling_right.type.is_solid ||
				ceiling_left.type.is_solid) {
				//test if either of the tile above them are solid
				Tile ceiling = (ceiling_left.type.is_solid ? ceiling_left : ceiling_right);

				if (ceiling_right.type.is_solid &&
					ceiling_left.type.is_solid) { //Both are solid, not suspicious
					vel_y = 0;

					y = ceiling.y + ceiling.h;
				} else { //only one is solid, double check if wall 
					Tile wall_up = current_map->get_tile_at(ceiling.x, ceiling.y + ceiling.h, map_offset_x, map_offset_y);
					if (!wall_up.type.is_solid) { //it is not a wall, fall
						vel_y = 0;

						y = ceiling.y + ceiling.h;
					}
				}

				//render the solid ceiling tile if in vertical debug mode
#ifdef V_DEBUG
				rect ceiling_rect = tile_to_rect(ceiling);
				ceiling_rect.color = al_map_rgb(255, 0, 0);
				render_queue.push_back(ceiling_rect);
#endif
			}
		} else if (vel_y > 0) { //going down
			falling = true;
			if (floor_right.type.is_solid ||
				floor_left.type.is_solid) {
				//check if with of the floor tiles are solid
				Tile floor = (floor_left.type.is_solid ? floor_left : floor_right);

				if (floor_right.type.is_solid &&
					floor_left.type.is_solid) { //if both of the tiles are solid then they are definitely on a floor
					vel_y = 0; //cancel their velocity and set jumping to false
					jumping = false;

					y = floor.y - h; //align them with the floor
				} else { // only one tile is solid, check if it is a wall
					Tile wall_up = current_map->get_tile_at(floor.x, floor.y - floor.h, map_offset_x, map_offset_y); //get the tile immediatley above the single tile that is solid
																						 //render it if vertical debug is on
#ifdef V_DEBUG
					rect wall_up_rect = tile_to_rect(wall_up);
					wall_up_rect.color = al_map_rgb(0, 0, 255);
					render_queue.push_back(wall_up_rect);
#endif 
					//if the tile immeditely above the single solid tile is not solid then it is not a wall
					if (!wall_up.type.is_solid) {
						if (y + h < floor.y + (floor.h * 0.5)) { //only snap them if in the top 50% of the block
							vel_y = 0;
							jumping = false;
							y = floor.y - h;
						}
					}
				}
				//render some of the tiles if in vertical debug mode
#ifdef V_DEBUG
				rect floor_rect = tile_to_rect(floor);
				floor_rect.color = al_map_rgb(255, 0, 0);
				render_queue.push_back(floor_rect);
#endif
			}
		}
		//add the velocity to the player's gravity
		vel_y += gravity;
	} else {
		//just to cover all bases, set the velocity to 0 if they are not jumping
		vel_y = 0;
		falling = true;
	}

	//add the y velocty to their position
	y += vel_y;
	//map_offset_y -= vel_y;
}

void Player::_horizontal() {
	//First test if the horizontal velocity is non-zero
	if (vel_x != 0) {
		//Add an offset so the tiles used for collision are better centered
		int offset_wall = 0, offset_floor = w / 2;
		if (vel_x > 0) {
			offset_wall = w; //For some reason this value needs to be higher when going to the right (probably rounding down)
		}

		//Find the corresponding tiles 
		Tile top = current_map->get_tile_at(x + vel_x + offset_wall, y - (h / 2), map_offset_x, map_offset_y);
		Tile bottom = current_map->get_tile_at(x + vel_x + offset_wall, y + (h / 2), map_offset_x, map_offset_y);
		Tile floor_left = current_map->get_tile_at(x + offset_floor + vel_x + (w / 2), y + h, map_offset_x, map_offset_y);
		Tile floor_right = current_map->get_tile_at(x + offset_floor + vel_x - (w / 2), y + h, map_offset_x, map_offset_y);

		//Render these tiles if in horizontal debug mode
#ifdef H_DEBUG
		render_queue.push_back(tile_to_rect(top));
		render_queue.push_back(tile_to_rect(bottom));
		render_queue.push_back(tile_to_rect(floor_left));
		render_queue.push_back(tile_to_rect(floor_right));
#endif
		//Test if the wall tiles in the direction they are going are solid
		if (top.type.is_solid ||
			bottom.type.is_solid) {
			//if they are set the velocity to 0 and align them with the tile that is solid

			Tile wall = (top.type.is_solid ? top : bottom);
			//Render which tile is solid when in horizontal debug mode
#ifdef H_DEBUG
			rect wall_rect = tile_to_rect(wall);
			wall_rect.color = al_map_rgb(255, 0, 0);
			render_queue.push_back(wall_rect);
#endif

			if (top.type.is_solid && !bottom.type.is_solid) { //only top is solid, check if ceiling

				Tile test_ceiling;
				if (vel_x > 0) { //moving right
					test_ceiling = current_map->get_tile_at(top.x - w, top.y, map_offset_x, map_offset_y);
				} else { //moving left
					test_ceiling = current_map->get_tile_at(top.x + w, top.y, map_offset_x, map_offset_y);
				}

#ifdef H_DEBUG
				rect test_ceiling_rect = tile_to_rect(test_ceiling);
				test_ceiling_rect.color = al_map_rgb(0, 0, 255);
				render_queue.push_back(test_ceiling_rect);
#endif

				if (!test_ceiling.type.is_solid) { //isnt ceiling, stop them
					vel_x = 0;
					//align them with solid tile
					if (vel_x > 0) { //moving right
						x = wall.x - w;
					} else if (vel_x < 0) { //moving left
						x = wall.x;
					}
				}
			} else {
				vel_x = 0;
				//align them with solid tile
				if (vel_x > 0) { //moving right
					x = wall.x - w;
				} else if (vel_x < 0) { //moving left
					x = wall.x;
				}
			}
		}
		//if there isn't any solid tiles below them set jumping to true to have them fall
		if (!floor_right.type.is_solid &&
			!floor_left.type.is_solid) {
			jumping = true;
		}
	}

	//add velocity to position
	x += vel_x;
	//map_offset_x -= vel_x;
}