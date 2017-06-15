#include "window.hpp"

//constructor
Window::Window(int height, int width, char* title, float fps) {
	Window::height = height;
	Window::width = width;
	Window::title = title;

	//Init allegro
	if (!al_init()) { 
		throw std::runtime_error("Unable to initialize allegro");
	}

	//init display
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	Window::display = al_create_display(640, 480);
	al_set_window_title(Window::display, Window::title);
	if (!Window::display) {
		throw std::runtime_error("Unable to create display");
	}

	if (!al_install_keyboard()) {
		throw std::runtime_error("Unable to install display");
	}

	//init event_queue and timer
	Window::event_queue = al_create_event_queue();
	Window::timer = al_create_timer(fps);
	Window::fps_timer = al_create_timer(1);

	//double check they are valid
	if (!Window::event_queue) {
		throw std::runtime_error("Unable to create event queue");
	}
	if (!Window::timer) {
		throw std::runtime_error("Unable to create timer");
	}
	if (!Window::fps_timer) {
		throw std::runtime_error("Unable to create timer");
	}

	//register the event sources
	al_register_event_source(event_queue, al_get_display_event_source(Window::display));
	al_register_event_source(event_queue, al_get_timer_event_source(Window::timer));
	al_register_event_source(event_queue, al_get_timer_event_source(Window::fps_timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	
	//init primitives
	if (!al_init_primitives_addon()) { 
		throw std::runtime_error("Unable to init primitives addon");
	}
	if (!al_init_image_addon()) {
		throw std::runtime_error("Unable to init image addon");
	}

	//Other settings
	al_hold_bitmap_drawing(true);
}

//start the game loop with the function given
void Window::start_game_loop(void(*fptr)(void)) {
	bool done = false;

	ALLEGRO_EVENT events;

	//start the timer
	al_start_timer(timer);
	al_start_timer(fps_timer);

	while (!done) {
		//set redraw to false by default
		bool redraw = false, fps = false;

		Window::temp_fps++;

		//done this way to process multiple events at once
		while (!al_is_event_queue_empty(event_queue)) {
			al_wait_for_event(event_queue, &events);
			//switch event type
			switch (events.type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				done = true;
				break;
			case ALLEGRO_EVENT_TIMER:
				//if the timer goes off then set redraw to true and call given function
				if (events.timer.source == Window::timer) {
					redraw = true;
				} else {
					fps = true;
				}
				break;
			case ALLEGRO_EVENT_KEY_DOWN:
				//run all the key down events when a key is pressed
				for (unsigned int i = 0; i < Window::key_down_callbacks.size(); i++) {
					if (key_down_callbacks.at(i).first == events.keyboard.keycode) {
						key_down_callbacks.at(i).second();
					}
				}
				break;
			case ALLEGRO_EVENT_KEY_UP:
				//run all the key up events when a key is pressed
				for (unsigned int i = 0; i < Window::key_up_callbacks.size(); i++) {
					if (key_up_callbacks.at(i).first == events.keyboard.keycode) {
						key_up_callbacks.at(i).second();
					}
				}
				break;
			}
		}

		if (redraw) {
			//call given function
			fptr();
		}
		if (fps) {
			Window::fps = Window::temp_fps;
			Window::temp_fps = 0;
		}
	}
}

void Window::add_key_down_event(int key, void(*fptr)(void)) {
	Window::key_down_callbacks.push_back(std::pair<int, void(*)(void)>(key, fptr));
}

void Window::add_key_up_event(int key, void(*fptr)(void)) {
	Window::key_up_callbacks.push_back(std::pair<int, void(*)(void)>(key, fptr));
}

void Window::add_key_event(int key, void(*fptr_down)(void), void(*fptr_up)(void)) {
	Window::key_down_callbacks.push_back(std::pair<int, void(*)(void)>(key, fptr_down));
	Window::key_up_callbacks.push_back(std::pair<int, void(*)(void)>(key, fptr_up));
}

//deconstructor
Window::~Window() {
	al_destroy_display(Window::display);
	al_destroy_event_queue(Window::event_queue);
	al_destroy_timer(Window::timer);
	al_destroy_thread(Window::thread);
}