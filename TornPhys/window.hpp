#pragma once
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <allegro5\allegro.h>
#include <allegro5\allegro_color.h>
#include <allegro5\allegro_primitives.h>

#include <iostream>

#include <stdexcept>

#include <vector>

class Window {
public:
	int height, width;
	char* title;
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_THREAD *thread = NULL;
	std::vector<std::pair<int, void(*)(void)>> key_down_callbacks;
	std::vector<std::pair<int, void(*)(void)>> key_up_callbacks;
	Window(int, int, char*, float = 1.0 / 60);
	void start_game_loop(void(*fptr)(void));
	void add_key_down_event(int, void(*fptr)(void));
	void add_key_up_event(int, void(*fptr)(void));
	void add_key_event(int, void(*fptr_down)(void), void(*fptr_up)(void));
	~Window();
};

#endif // !WINDOW_HPP
