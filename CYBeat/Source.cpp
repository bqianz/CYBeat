#include "functions.h"
#include "render_functions.h"
#include "constants.h"

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

	// background colour
	if (SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a)) {
		logSDLError(std::cout, "SetRenderDrawColor");
		cleanup(window, renderer);
		SDL_Quit();
		IMG_Quit();
		return 1;
	}
	// this colours the bg?
	SDL_RenderClear(renderer);

	
	// goal line
	SDL_Rect goal_rect;
	goal_rect.x = 0;
	goal_rect.y = SCREEN_HEIGHT * 5 / 6;
	goal_rect.w = SCREEN_WIDTH;
	goal_rect.h = SCREEN_HEIGHT / 100;

	SDL_SetRenderDrawColor(renderer, bd_r, bd_g, bd_b, bd_a);

	int goal_success = SDL_RenderFillRect(renderer, &goal_rect);
	if (SDL_RenderDrawRect(renderer, &goal_rect)) {
		logSDLError(std::cout, "RenderFillRect");
		cleanup(window, renderer);
		SDL_Quit();
		IMG_Quit();
		return 1;
	}

	SDL_RenderPresent(renderer);
	SDL_Delay(10000);




	//const std::string resPath = getResourcePath("YCY");
	//SDL_Texture* image = loadTexture(resPath + "image.png", renderer);
	////Make sure image loaded
	//if (image == nullptr) {
	//	cleanup(image, renderer, window);
	//	IMG_Quit();
	//	SDL_Quit();
	//	return 1;
	//}





	////clip
	//int cw = 100, ch = 100;

	//// draws centered foreground image
	//int x = (SCREEN_WIDTH - cw) / 2;
	//int y = (SCREEN_HEIGHT - ch) / 2;

	//SDL_Rect image_clips[4];
	//for (int i = 0; i < 4; i++) {
	//	image_clips[i].x = i / 2 * cw;
	//	image_clips[i].y = i % 2 * ch;
	//	image_clips[i].w = cw;
	//	image_clips[i].h = ch;
	//}

	//int currentClip = 0;
	//bool quit = false;


	//SDL_Event e;
	//while (!quit) {
	//	while (SDL_PollEvent(&e)) {
	//		if (e.type == SDL_QUIT) {
	//			quit = true;
	//		}
	//		if (e.type == SDL_KEYDOWN) {
	//			switch (e.key.keysym.sym) {
	//			case SDLK_1:
	//				currentClip = 0;
	//				break;
	//			case SDLK_2:
	//				currentClip = 1;
	//				break;
	//			case SDLK_3:
	//				currentClip = 2;
	//				break;
	//			case SDLK_4:
	//				currentClip = 3;
	//				break;
	//			case SDLK_ESCAPE:
	//				quit = true;
	//				break;
	//			default:
	//				break; // if generic key pressed, nothing happens

	//			}
	//		}
	//	}
	//	SDL_RenderClear(renderer); // encouraged for code reusability
	//	// renderTexture(image, renderer, x, y, &image_clips[currentClip]);
	//	SDL_RenderPresent(renderer);
	//}



	cleanup(renderer, window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}