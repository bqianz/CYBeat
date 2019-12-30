#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <string>
#include <cmath>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>

class LTexture
{
	public:
		//Initializes variables
		LTexture()
		{
			//Initialize
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}

		//Deallocates memory
		~LTexture()
		{
			//Deallocate
			free();
		}

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		// #if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font* font);
		// #endif

		//Deallocates texture
		void free()
		{
			//Free texture if it exists
			if( mTexture != NULL )
			{
				SDL_DestroyTexture( mTexture );
				mTexture = NULL;
				mWidth = 0;
				mHeight = 0;
			}
		}

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		void setAlpha( Uint8 alpha )
		{
			//Modulate texture alpha
			SDL_SetTextureAlphaMod( mTexture, alpha );
		}
		
		//Renders texture at given point
		void render( int x, int y, SDL_Renderer* renderer)
		{
			//Set rendering space and render to screen
			SDL_Rect renderQuad = { x, y, mWidth, mHeight };
			SDL_RenderCopy( renderer, mTexture, NULL, &renderQuad );
		}

		void render(SDL_Renderer* renderer, std::string position = "center", int bottom_margin = 0)
		{
			if(position == "center")
			{
				render((SCREEN_WIDTH - mWidth)/2, (SCREEN_HEIGHT - bottom_margin - mHeight)/2, renderer);
			}
			else if(position == "top center")
			{
				render((SCREEN_WIDTH - mWidth)/2, 0, renderer);
			}
			else if(position == "center1")
			{
				render((SCREEN_WIDTH - mWidth)/2, (SCREEN_HEIGHT-bottom_margin)/2 - mHeight, renderer);
			}
			else if (position == "center2")
			{
				render((SCREEN_WIDTH - mWidth)/2, (SCREEN_HEIGHT-bottom_margin)/2, renderer);
			}
			else if (position == "quater from top")
			{
				render((SCREEN_WIDTH - mWidth)/2, (SCREEN_HEIGHT - bottom_margin)/4 - mHeight/2, renderer);
			}
		}

		//Gets image dimensions
		int getWidth() {return mWidth;}
		int getHeight() {return mHeight;}

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

#endif