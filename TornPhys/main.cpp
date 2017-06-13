#include <allegro5\allegro.h>
#include <allegro5\allegro_color.h>
#include <allegro5\allegro_primitives.h>

#include <vector>
#include <iostream>

#include "window.hpp"
#include "map.hpp"
#include "tile.hpp"

//toggle debug modes
//#define H_DEBUG //used for horizontal debug
//#define V_DEBUG //used for vertical debug
//#define TIME_DEBUG //used for time debug

//rect struct for rendering
struct rect {
	float x1, y1, x2, y2;
	ALLEGRO_COLOR color;
	rect() :color(al_map_rgb(255, 0, 255)) {}
	void add_x(float x) {
		x1 += x;
		x2 += x;
	}
	void add_y(float y) {
		y1 += y;
		y2 += y;
	}
	void set_x(float x) {
		float w = x2 - x1;
		x1 = x;
		x2 = x + w;
	}
	void set_y(float y) {
		float h = y2 - y1;
		y1 = y;
		y2 = y + h;
	}
};

//player struct, auto create obj_player
struct player {
	bool jumping;
	float speed, gravity;
	float jump_height;
	float x, y, w, h;
	float vel_x, vel_y;
	float max_vel_x, max_vel_y;
} obj_player;


//slow the game down for closer inspection when in time debug mode
#ifdef TIME_DEBUG
Window window(640, 480, "Game Window", float(1.0 / 30.0));
#else
Window window(640, 480, "Game Window", float(1.0 / 120.0));
#endif
//init the map variables
Map map1("map1.txt", 16, 16);
Map map2("map1.txt", 16, 16);
Map *current_map;

//create render queue vector
std::vector<rect> render_queue;

//add key listeners to window
void add_key_listeners() {
	window.add_key_event(ALLEGRO_KEY_A, []() {
		obj_player.vel_x = -obj_player.speed;
	}, []() {
		if (obj_player.vel_x == -obj_player.speed) {
			obj_player.vel_x = 0;
		}
	});
	window.add_key_event(ALLEGRO_KEY_D, []() {
		obj_player.vel_x = obj_player.speed;
	}, []() {
		if (obj_player.vel_x == obj_player.speed) {
			obj_player.vel_x = 0;
		}
	});
	window.add_key_event(ALLEGRO_KEY_SPACE, []() {
		if (!obj_player.jumping) {
			obj_player.jumping = true;
			obj_player.vel_y = obj_player.jump_height;
		}
	}, []() {
		if (obj_player.jumping) {
			obj_player.vel_y *= 0.5;
		}
	});
}

//used to convert the tile class into a render-able rectangle 
rect tile_to_rect(Tile tile) {
	rect r;
	r.x1 = tile.x;
	r.y1 = tile.y;
	r.x2 = tile.x + tile.w;
	r.y2 = tile.y + tile.h;
	return r;
}

void horizontal() {
	//First test if the horizontal velocity is non-zero
	if (obj_player.vel_x != 0) {
		//Add an offset so the tiles used for collision are better centered
		int offset_wall = 0, offset_floor = obj_player.w / 2;
		if (obj_player.vel_x > 0) {
			offset_wall = obj_player.w; //For some reason this value needs to be higher when going to the right (probably rounding down)
		}

		//Find the corresponding tiles 
		Tile top = current_map->get_tile_at(obj_player.x + obj_player.vel_x + offset_wall, obj_player.y + (obj_player.h / 2));
		Tile bottom = current_map->get_tile_at(obj_player.x + obj_player.vel_x + offset_wall, obj_player.y - (obj_player.h / 2));
		Tile floor_left = current_map->get_tile_at(obj_player.x + offset_floor + obj_player.vel_x + (obj_player.w / 2), obj_player.y + obj_player.h);
		Tile floor_right = current_map->get_tile_at(obj_player.x + offset_floor + obj_player.vel_x - (obj_player.w / 2), obj_player.y + obj_player.h);

		//Render these tiles if in horizontal debug mode
#ifdef H_DEBUG
		render_queue.push_back(tile_to_rect(top));
		render_queue.push_back(tile_to_rect(bottom));
		render_queue.push_back(tile_to_rect(floor_left));
		render_queue.push_back(tile_to_rect(floor_right));
#endif
		//Test if the wall tiles in the direction they are going are solid
		if (top.type == current_map->solid ||
			bottom.type == current_map->solid) {
			//if they are set the velocity to 0 and align them with the tile that is solid
			obj_player.vel_x = 0;

			Tile wall = (top.type == current_map->solid ? top : bottom);
			//Render which tile is solid when in horizontal debug mode
#ifdef H_DEBUG
			rect wall_rect = tile_to_rect(wall);
			wall_rect.color = al_map_rgb(255, 0, 0);
			render_queue.push_back(wall_rect);
#endif
			//align them with solid tile
			if (obj_player.vel_x > 0) { //moving right
				obj_player.x = wall.x - obj_player.w;
			} else if (obj_player.vel_x < 0) { //moving left
				obj_player.x = wall.x;
			}
		}
		//if there isn't any solid tiles below them set jumping to true to have them fall
		if (floor_right.type != current_map->solid &&
			floor_left.type != current_map->solid) {
			obj_player.jumping = true;
		}
	}

	//add velocity to position
	obj_player.x += obj_player.vel_x;
}

void vertical() {
	//test if player is jumping
	if (obj_player.jumping) {
		//Add an offset to better orient collision tiles
		int offset = obj_player.w / 2;

		//Select corresponding tiles
		Tile floor_left = current_map->get_tile_at(obj_player.x + offset + obj_player.vel_x + (obj_player.w / 2), obj_player.y + obj_player.h);
		Tile floor_right = current_map->get_tile_at(obj_player.x + offset + obj_player.vel_x - (obj_player.w / 2), obj_player.y + obj_player.h);
		Tile ceiling_left = current_map->get_tile_at(obj_player.x + offset + obj_player.vel_x + (obj_player.w / 2), obj_player.y - obj_player.h);
		Tile ceiling_right = current_map->get_tile_at(obj_player.x + offset + obj_player.vel_x - (obj_player.w / 2), obj_player.y - obj_player.h);

		//Render them if in vertical debug mode
#ifdef V_DEBUG
		render_queue.push_back(tile_to_rect(ceiling_left));
		render_queue.push_back(tile_to_rect(ceiling_right));
		render_queue.push_back(tile_to_rect(floor_left));
		render_queue.push_back(tile_to_rect(floor_right));
#endif

		if (obj_player.vel_y < 0) { //going up
			if (ceiling_right.type == current_map->solid ||
				ceiling_left.type == current_map->solid) {
				//test if either of the tile above them are solid
				Tile ceiling = (ceiling_left.type == current_map->solid ? ceiling_left : ceiling_right);

				if (ceiling_right.type == current_map->solid &&
					ceiling_left.type == current_map->solid) { //Both are solid, not suspicious
					obj_player.vel_y = 0;

					obj_player.y = ceiling.y + ceiling.h;
				} else { //only one is solid, double check if wall 
					Tile wall_up = current_map->get_tile_at(ceiling.x, ceiling.y + ceiling.h);
					if (wall_up.type != current_map->solid) { //it is not a wall, fall
						obj_player.vel_y = 0;

						obj_player.y = ceiling.y + ceiling.h;
					}
				}

				//render the solid ceiling tile if in vertical debug mode
#ifdef V_DEBUG
				rect ceiling_rect = tile_to_rect(ceiling);
				ceiling_rect.color = al_map_rgb(255, 0, 0);
				render_queue.push_back(ceiling_rect);
#endif
			}
		} else if (obj_player.vel_y > 0) { //going down
			if (floor_right.type == current_map->solid ||
				floor_left.type == current_map->solid) {
				//check if with of the floor tiles are solid
				Tile floor = (floor_left.type == current_map->solid ? floor_left : floor_right);

				if (floor_right.type == current_map->solid &&
					floor_left.type == current_map->solid) { //if both of the tiles are solid then they are definitely on a floor
					obj_player.vel_y = 0; //cancel their velocity and set jumping to false
					obj_player.jumping = false;

					obj_player.y = floor.y - obj_player.h; //align them with the floor
				} else { // only one tile is solid, check if it is a wall
					Tile wall_up = current_map->get_tile_at(floor.x, floor.y - floor.h); //get the tile immediatley above the single tile that is solid
					//render it if vertical debug is on
#ifdef V_DEBUG
					rect wall_up_rect = tile_to_rect(wall_up);
					wall_up_rect.color = al_map_rgb(0, 0, 255);
					render_queue.push_back(wall_up_rect);
#endif 
					//if the tile immeditely above the single solid tile is not solid then it is not a wall
					if (wall_up.type != current_map->solid) { 
						//cancel their velocity and set them on the tile
						obj_player.vel_y = 0;
						obj_player.jumping = false;

						obj_player.y = floor.y - obj_player.h;
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
		obj_player.vel_y += obj_player.gravity;
	} else {
		//just to cover all bases, set the velocity to 0 if they are not jumping
		obj_player.vel_y = 0;
	}

	//add the y velocty to their position
	obj_player.y += obj_player.vel_y;
}

void collision() {
	//calculate horizontal and vertical collision
	horizontal();
	vertical();
}

void render() {
	//render the current map with no offset
	current_map->render_map(0, 0);
	//go through render queue and render each item
	for (unsigned int i = 0; i < render_queue.size(); i++) {
		rect r = render_queue.at(i);
		al_draw_rectangle(r.x1, r.y1, r.x2, r.y2, r.color, 2.0);
	}
	//flip display and empty the render queue
	al_flip_display();
	render_queue.clear();
}

void game_loop() {
	//check collision
	collision();

	//create the player rectangle and add it to the render queue
	rect player_rect;
	player_rect.x1 = obj_player.x;
	player_rect.y1 = obj_player.y;
	player_rect.x2 = obj_player.x + obj_player.w;
	player_rect.y2 = obj_player.y + obj_player.h;
	player_rect.color = al_map_rgb(0, 255, 100);
	render_queue.push_back(player_rect);

	//render map and render queue
	render();
}

void init_game() {
	//set the current map
	current_map = &map1;


	//init the player's variables
	obj_player.w = 16;
	obj_player.h = 16;

	obj_player.x = current_map->spawn_x;
	obj_player.y = current_map->spawn_y;

	obj_player.speed = 2.5;
	obj_player.gravity = 0.2;

	obj_player.jump_height = -8;

	obj_player.max_vel_x = 2000;
	obj_player.max_vel_y = 2000;

	window.start_game_loop(&game_loop);
}

int main() {
	//add the key listeners and call init game method
	add_key_listeners();

	init_game();

	return 0;
}