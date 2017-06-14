// TODO
/*
 * Add map transitions
 * Add more flexiblity in physics engine
 * Clean up various files
 * Add more variety to types of maps loaded, possibly menus
 * Improve player movement
 * Add entities more than just the player
 */

#include <allegro5\allegro.h>
#include <allegro5\allegro_color.h>
#include <allegro5\allegro_primitives.h>

#include <vector>
#include <iostream>

#include "window.hpp"
#include "map.hpp"
#include "tile.hpp"
#include "tile_type.hpp"
#include "player.hpp"

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

//slow the game down for closer inspection when in time debug mode
#ifdef TIME_DEBUG
Window window(640, 480, "Game Window", float(1.0 / 30.0));
#else
Window window(640, 480, "Game Window", float(1.0 / 120.0));
#endif
//init the map variables
Map map1("map1.txt", 16, 16);
Map map2("map2.txt", 16, 16);
Map *current_map;

//create render queue vector
std::vector<rect> render_queue;

Player player;

float map_offset_x = 0, map_offset_y = -240;

//add key listeners to window
void add_key_listeners() {
	window.add_key_event(ALLEGRO_KEY_A, []() {
		player.vel_x = -player.speed;
	}, []() {
		if (player.vel_x == -player.speed) {
			player.vel_x = 0;
		}
	});
	window.add_key_event(ALLEGRO_KEY_D, []() {
		player.vel_x = player.speed;
	}, []() {
		if (player.vel_x == player.speed) {
			player.vel_x = 0;
		}
	});
	window.add_key_event(ALLEGRO_KEY_SPACE, []() {
		if (!player.jumping) {
			player.jumping = true;
			player.falling = false;
			player.vel_y = player.jump_height;
		}
	}, []() {
		if (player.jumping && !player.falling) {
			player.falling = true;
			player.vel_y -= (player.vel_y / 2);
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

void collision() {
	player.update_collision();
	map_offset_x -= player.vel_x;
	map_offset_y -= player.vel_y;
}

void render() {
	al_clear_to_color(al_map_rgb(255, 255, 255));
	//render the current map with no offset
	current_map->render_map(map_offset_x, 0);
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
	player_rect.x1 = 320;
	player_rect.y1 = player.y;
	player_rect.x2 = player_rect.x1 + player.w;
	player_rect.y2 = player_rect.y1 + player.h;
	player_rect.color = al_map_rgb(0, 255, 100);
	render_queue.push_back(player_rect);

	//render map and render queue
	render();
}

void init_game() {
	//set the current map

	TileType solid('1', true, al_map_rgb(0, 0, 0));
	TileType spawn('2', false, al_map_rgb(255, 255, 255));
	spawn.is_spawn = true;
	spawn.draw_wireframe = true;

	//add tile types
	map1.add_tile_type(TileType('0', false, al_map_rgb(255, 255, 255)));
	map1.add_tile_type(solid);
	map1.add_tile_type(spawn);
	map1.parse_map();

	TileType map_trans('A', false, al_map_rgb(244, 237, 105));
	map_trans.add_exec([]() {
		current_map = &map1;
		player.x = current_map->spawn_x;
		player.y = current_map->spawn_y;
		player.current_map = current_map;
		map_offset_x = 320 - current_map->spawn_x;
	});

	map2.add_tile_type(TileType('0', false, al_map_rgb(255, 255, 255)));
	map2.add_tile_type(TileType('1', true, al_map_rgb(255, 0, 0)));
	map2.add_tile_type(spawn);
	map2.add_tile_type(map_trans);
	map2.parse_map();

	current_map = &map2;

	map_offset_x = 320 - current_map->spawn_x;
	//map_offset_y = 0;

	//init the player's variables
	player = Player(current_map->spawn_x, current_map->spawn_y, 16, 16, 2.5F, 0.2F, 8, current_map);

	window.start_game_loop(&game_loop);
}

int main() {
	//add the key listeners and call init game method
	add_key_listeners();

	init_game();

	return 0;
}