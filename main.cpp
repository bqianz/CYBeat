#include "functions.h"
#include "render_functions.h"
#include "constants.h"


void D_pressed() {
}
int main(int, char**)
{
	// make window
	SDL_Window* window = SDL_CreateWindow("CYBeat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		logSDLError(std::cout, "CreateWindow");
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // vsync: refreshes at same rate as monitor
	if (renderer == nullptr) {
		logSDLError(std::cout, "CreateRenderer");
		cleanup(window);
		SDL_Quit();
		return 1;
	}

	// set render color to border colour
	SDL_SetRenderDrawColor(renderer, bd_r, bd_g, bd_b, bd_a);

	// goal line
	SDL_Rect goal_rect;
	goal_rect.x = 0;
	goal_rect.y = goal_height-bd_thickness/2;
	goal_rect.w = SCREEN_WIDTH;
	goal_rect.h = bd_thickness;

	// vertical line
	SDL_Rect vt[line_num];
	for (int i = 0; i < line_num; i++) {
		vt[i].x = col_width*(i+1) - bd_thickness/2;
		vt[i].y = 0;
		vt[i].w = bd_thickness;
		vt[i].h = SCREEN_HEIGHT;
	}

	// buttons

	// change
	// load button images
	SDL_Texture* bt_images[col_num];
	const std::string resPath = getResourcePath();
	// std:: string resPath = "res\\YCY\\";
	for (int i = 0; i < col_num; i++) {
		bt_images[i] = loadTexture(resPath + "button" + std::to_string(i) + ".png", renderer);
		if (bt_images[i] == nullptr) {
			cleanup(bt_images[i], renderer, window);
			IMG_Quit();
			SDL_Quit();
			return 1;
		}
	}

	// button rectangles
	SDL_Rect bt_rect[col_num];
	for (int i = 0; i < col_num; i++) {
		bt_rect[i].x = i * col_width + bd_thickness / 2;
		bt_rect[i].y = goal_height + bd_thickness / 2;
		bt_rect[i].w = col_width - bd_thickness;
		bt_rect[i].h = bt_rect[i].w;
	}



	// load short note images
	SDL_Texture* short_images[col_num];
	for (int i = 0; i < col_num; i++) {
		short_images[i] = loadTexture(resPath + "short" + std::to_string(i) + ".png", renderer);
		if (short_images[i] == nullptr) {
			cleanup(bt_images[i], renderer, window);
			IMG_Quit();
			SDL_Quit();
			return 1;
		}
	}
	// short note rectangles
	SDL_Rect short_rect[col_num];
	for (int i = 0; i < col_num; i++) {
		short_rect[i].x = i * col_width;
		short_rect[i].y = goal_height;
		short_rect[i].w = col_width;
		short_rect[i].h = short_rect[i].w;
	}




	bool quit = false;
	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				default:
					break; // if generic key pressed, nothing happens

				}
			}
		}
		//change
		// background
		SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
		SDL_RenderClear(renderer); // encouraged for code reusability
		
		// lines
		SDL_SetRenderDrawColor(renderer, bd_r, bd_g, bd_b, bd_a);
		if (SDL_RenderFillRect(renderer, &goal_rect)) {
			logSDLError(std::cout, "RenderFillRect");
			cleanup(window, renderer);
			SDL_Quit();
			IMG_Quit();
			return 1;
		}
		for (int i = 0; i < line_num; i++) {
			if (SDL_RenderFillRect(renderer, &vt[i])) {
				logSDLError(std::cout, "RenderFillRect");
				cleanup(window, renderer);
				SDL_Quit();
				IMG_Quit();
				return 1;
			}
		}

		// buttons
		for (int i = 0; i < col_num; i++) {
			renderTexture(bt_images[i], renderer, bt_rect[i]);
		}
		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
		if (currentKeyStates[SDL_SCANCODE_D])
		{
			renderTexture(short_images[0], renderer, short_rect[0]);
		}
		if (currentKeyStates[SDL_SCANCODE_F])
		{
			renderTexture(short_images[1], renderer, short_rect[1]);
		}
		if (currentKeyStates[SDL_SCANCODE_J])
		{
			renderTexture(short_images[2], renderer, short_rect[2]);
		}
		if (currentKeyStates[SDL_SCANCODE_K])
		{
			renderTexture(short_images[3], renderer, short_rect[3]);
		}

		SDL_RenderPresent(renderer);
	}



	cleanup(renderer, window);
	for (int i = 0; i < col_num; i++) {
		cleanup(bt_images[i]);
		cleanup(short_images[i]);
	}
	IMG_Quit();
	SDL_Quit();

	return 0;
}