#include "functions.h"
#include "render_functions.h"
#include "constants.h"

int main(int, char**)
{
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Lesson 3", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		logSDLError(std::cout, "CreateWindow");
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		logSDLError(std::cout, "CreateRenderer");
		cleanup(window);
		SDL_Quit();
		return 1;
	}

	const std::string resPath = getResourcePath("Lesson5");
	SDL_Texture *image = loadTexture(resPath + "image.png", renderer);
	//Make sure image loaded
	if (image == nullptr) {
		cleanup(image, renderer, window);
		IMG_Quit();
		SDL_Quit();
		return 1;
	}


	//clip
	int cw = 100, ch = 100;

	// draws centered foreground image
	int x = (SCREEN_WIDTH - cw) / 2;
	int y = (SCREEN_HEIGHT - ch) / 2;

	SDL_Rect image_clips[4];
	for (int i = 0; i < 4; i++) {
		image_clips[i].x = i / 2 * cw;
		image_clips[i].y = i % 2 * ch;
		image_clips[i].w = cw;
		image_clips[i].h = ch;
	}

	int currentClip = 0;
	bool quit = false;


	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_1:
					currentClip = 0;
					break;
				case SDLK_2:
					currentClip = 1;
					break;
				case SDLK_3:
					currentClip = 2;
					break;
				case SDLK_4:
					currentClip = 3;
					break;
				case SDLK_ESCAPE:
					quit = true;
					break;
				default:
					break; // if generic key pressed, nothing happens

				}
			}
		}
		SDL_RenderClear(renderer); // encouraged for code reusability
		renderTexture(image, renderer, x, y, &image_clips[currentClip]);
		SDL_RenderPresent(renderer);
	}

	cleanup(image, renderer, window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}