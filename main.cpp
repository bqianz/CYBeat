#include "functions.h"
#include "render_functions.h"
#include "constants.h"
#include "ltexture.h"
#include "ltimer.h"
#include "note.h"

#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

//The window we'll be rendering to
SDL_Window* window = NULL;

//The window renderer
SDL_Renderer* renderer = NULL;

// goal line
SDL_Rect goal_rect;

// vertical divisor lines
SDL_Rect vt[line_num];

// button texture
SDL_Texture* bt_images[col_num];

// button rectangles
SDL_Rect bt_rect[col_num];

// short press button texture
SDL_Texture* short_images[col_num];

// short press button rectangles
SDL_Rect short_rect[col_num];

//Globally used font
TTF_Font* font = NULL;

//Text textures
LTexture gTimeTextTexture;
LTexture gPromptTextTexture;

//The application timer
LTimer timer;

// like iostreams but instead of writing to the console, we can read and write to string in memory
std::stringstream timeio;

Note note1;

Uint32 current_time;

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureText.c_str(), textColor );
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    
    //Return success
    return mTexture != NULL;
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::render( int x, int y )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_RenderCopy( renderer, mTexture, NULL, &renderQuad );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		window = SDL_CreateWindow("CYBeat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == nullptr) {
			logSDLError(std::cout, "CreateWindow");
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // vsync: refreshes at same rate as monitor
			if (renderer == nullptr) {
				logSDLError(std::cout, "CreateRenderer");
				success = false;
			}
			else
			{
				// initialize renderer colour
				SDL_SetRenderDrawColor(renderer, bd_r, bd_g, bd_b, bd_a);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadLines(){
	goal_rect.x = 0;
	goal_rect.y = goal_height-bd_thickness/2;
	goal_rect.w = SCREEN_WIDTH;
	goal_rect.h = bd_thickness;

	for (int i = 0; i < line_num; i++) {
		vt[i].x = col_width*(i+1) - bd_thickness/2;
		vt[i].y = 0;
		vt[i].w = bd_thickness;
		vt[i].h = SCREEN_HEIGHT;
	}
}

bool loadButtons(){
	// button images
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
	for (int i = 0; i < col_num; i++) {
		bt_rect[i].x = i * col_width + bd_thickness / 2;
		bt_rect[i].y = goal_height + bd_thickness / 2;
		bt_rect[i].w = col_width - bd_thickness;
		bt_rect[i].h = bt_rect[i].w;
	}

	// short press button images
	for (int i = 0; i < col_num; i++) {
		short_images[i] = loadTexture(resPath + "short" + std::to_string(i) + ".png", renderer);
		if (short_images[i] == nullptr) {
			cleanup(bt_images[i], renderer, window);
			IMG_Quit();
			SDL_Quit();
			return 1;
		}
	}

	// short press button rectangles
	for (int i = 0; i < col_num; i++) {
		short_rect[i].x = i * col_width;
		short_rect[i].y = goal_height;
		short_rect[i].w = col_width;
		short_rect[i].h = short_rect[i].w;
	}
}

bool loadFont()
{
	//Loading success flag
	bool success = true;

	//Open the font
	const std::string resPath = getResourcePath() + "lazy.ttf";
	font = TTF_OpenFont( resPath.c_str(), 28 );
	if( font == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Set text color as black
		SDL_Color textColor = { 0, 0, 0, 255 };
		
		//Load prompt texture
		if( !gPromptTextTexture.loadFromRenderedText( "Press S or P", textColor ) )
		{
			printf( "Unable to render prompt texture!\n" );
			success = false;
		}
	}

	return success;
}

void close(){
	gTimeTextTexture.free();
	gPromptTextTexture.free();

	//Free global font
	TTF_CloseFont( font );
	font = NULL;

	cleanup(renderer, window);

	// destroy button textures
	// don't have to destory rectangles i think? only textures
	for (int i = 0; i < col_num; i++) {
		cleanup(bt_images[i]);
		cleanup(short_images[i]);
	}

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int, char**)
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}

	else
	{
		//Load media
		loadLines();

		loadButtons();

		loadFont();

		// main loop flag
		bool quit = false;

		// event handler
		SDL_Event e;

		// unsigned integer 32bit type for protability across platforms

		SDL_Color textColor = { 0, 0, 0, 255 };

		// while application is running
		while (!quit) {

			// handle events on queue
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					quit = true;
				}

				// keys that don't happen at the same time
				else if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
						
						//quit
						case SDLK_ESCAPE:
							quit = true;
							break;
						
						//start stop
						case SDLK_s:
						{
							if(timer.isStarted())
							{
								timer.stop();
							}
							else
							{
								timer.start();
							}
							break;
						}

						// pause unpause
						case SDLK_p:
						{
							if(timer.isPaused())
							{
								timer.unpause();
							}
							else
							{
								timer.pause();
							}
							break;
						}
						default:
							break; // if generic key pressed, nothing happens
					}
				}
			}


			// draw everything starting from background
			SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
			SDL_RenderClear(renderer); // encouraged for code reusability
			
			// goal line
			SDL_SetRenderDrawColor(renderer, bd_r, bd_g, bd_b, bd_a);
			if (SDL_RenderFillRect(renderer, &goal_rect)) {
				logSDLError(std::cout, "RenderFillRect goal_rect");
				cleanup(window, renderer);
				SDL_Quit();
				IMG_Quit();
				return 1;
			}

			// 
			for (int i = 0; i < line_num; i++) {
				if (SDL_RenderFillRect(renderer, &vt[i])) {
					logSDLError(std::cout, "RenderFillRect vt");
					cleanup(window, renderer);
					SDL_Quit();
					IMG_Quit();
					return 1;
				}
			}


			// set text
			current_time = timer.get_current_time();

			timeio.str("");
			timeio << "Seconds: " << (current_time/1000.f);

			// buttons

			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL); 
			// arguments gets the number of keys available?
			const int key_indices[4]= {SDL_SCANCODE_D, SDL_SCANCODE_F,SDL_SCANCODE_J,SDL_SCANCODE_K};

			for (int i = 0; i < col_num; i++){
				if (currentKeyStates[key_indices[i]])
				{
					renderTexture(short_images[i], renderer, short_rect[i]);
				}
				else
				{
					renderTexture(bt_images[i], renderer, bt_rect[i]);
				}
			}

			// notes

			if (note1.check_showing(current_time))
			{
				note1.render(renderer);
				// if it's the first note in the column
				// std::cout<<e.type<<SDL_KEYDOWN<<"\n";
				note1.handleEvent(e, current_time);

			}
			// text to texture
			if( !gTimeTextTexture.loadFromRenderedText( timeio.str().c_str(), textColor ) )
			{
				printf( "Unable to render time texture!\n" );
			}
			//Render textures
			gPromptTextTexture.render( ( SCREEN_WIDTH - gPromptTextTexture.getWidth() ) / 2, 0 );
			gTimeTextTexture.render( ( SCREEN_WIDTH - gPromptTextTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gPromptTextTexture.getHeight() ) / 2 );

			SDL_RenderPresent(renderer);
		}
	}



	close();

	return 0;
}