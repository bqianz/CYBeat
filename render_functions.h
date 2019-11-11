#ifndef RENDERFUNCTIONS_H
#define RENDERFUNCTIONS_H

#include <SDL2/SDL_image.h>

/* TEXTURE LOADING FUNCTION: image to texture
image -> SDL_LoadBMP() -> SDL_Surface -> SDL_CreateTextureFromSurface() - > SDL_Texture
@param filepath: the bmp image file to load
@param ren the renderer to load the texture onto
*/
SDL_Texture* loadTexture(const std::string& filepath, SDL_Renderer* ren) {
	SDL_Texture* texture = IMG_LoadTexture(ren, filepath.c_str());
	if (texture == nullptr) {
		logSDLError(std::cout, "LoadTexture");
	}
	return texture;
}

void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, SDL_Rect rect, SDL_Rect* clip = nullptr) {
	SDL_RenderCopy(ren, tex, clip, &rect);
}

void renderTexture(SDL_Texture* texture, SDL_Renderer* ren, int x, int y, SDL_Rect* clip = nullptr) {
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	if (clip != nullptr) {
		dst.w = clip->w;
		dst.h = clip->h;
	}
	else {
		SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
	}
	renderTexture(texture, ren, dst, clip);
}

#endif