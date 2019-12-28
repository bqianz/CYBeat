#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <stdio.h>
#include <string>
#include <cmath>
#include <sstream>

#include "functions.h"
#include "render_functions.h"
#include "constants.h"
#include "ltexture.h"
#include "ltimer.h"
#include "note.h"
#include "score.h"

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
TTF_Font* font_big = NULL;

// background music
Mix_Music* calorie = NULL;

//Text textures
// LTexture timeTexture;
LTexture pointsTexture;
LTexture feedbackTextures[3];
LTexture promptTextures[2];
LTexture promptKeysTextures[4];

//The application timer
LTimer timer;

// like iostreams but instead of writing to the console, we can read and write to string in memory
std::stringstream timeio;
std::stringstream pointsio;

Uint32 current_time;

// event handler
SDL_Event event;

// score
Score* score = NULL;

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font* font)
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

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
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

				//Initialize SDL_mixer
				// might need to play around with 4th argument to minimize lag
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
			}
		}
	}

	return success;
}

void load_lines(){
	for (int i = 0; i < line_num; i++) {
		vt[i].x = col_width*(i+1) - bd_thickness/2;
		vt[i].y = 0;
		vt[i].w = bd_thickness;
		vt[i].h = SCREEN_HEIGHT;
	}
}

void load_goal_line()
{
	goal_rect.x = 0;
	goal_rect.y = goal_height-goal_thickness/2;
	goal_rect.w = SCREEN_WIDTH;
	goal_rect.h = goal_thickness;
}

void load_buttons(){
	// button images
	const std::string resPath = getResourcePath();
	// std:: string resPath = "res\\YCY\\";
	for (int i = 0; i < col_num; i++) {
		bt_images[i] = loadTexture(resPath + "button" + std::to_string(i) + ".png", renderer);
		if (bt_images[i] == nullptr) {
			cleanup(bt_images[i], renderer, window);
			IMG_Quit();
			SDL_Quit();
			// return 1;
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
			// return 1;
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

void load_audio()
{
	bool success = true;
	calorie = Mix_LoadMUS( (getResourcePath() + "calorie.mp3").c_str() );
	if( calorie == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
    }
}

bool load_font()
{
	//Loading success flag
	bool success = true;

	//Open the font
	const std::string resPath = getResourcePath() + font_name;
	font = TTF_OpenFont( resPath.c_str(), regular_font_size);
	font_big = TTF_OpenFont(resPath.c_str(),big_font_size);
	if( font == NULL || font_big == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Set text color as black
		SDL_Color textColor = { 0, 0, 0, 255 };

		// load prompt texture
		if( !promptTextures[0].loadFromRenderedText( "Press S for start and stop.", textColor, font) )
		{
			printf( "Unable to render Prompt texture!\n" );
			success = false;
		}
		if( !promptTextures[1].loadFromRenderedText( "Press P for pause and unpause", textColor, font) )
		{
			printf( "Unable to render Prompt texture!\n" );
			success = false;
		}

		for(int i = 0; i < col_num; i++)
		{
			if( !promptKeysTextures[i].loadFromRenderedText( keys[i], textColor, font_big) )
			{
				printf( "Unable to render PromptKeys texture!\n" );
				success = false;
			}
		}

		//Load feedback texture
		if( !feedbackTextures[2].loadFromRenderedText( "Perfect", textColor, font_big) )
		{
			printf( "Unable to render Perfect texture!\n" );
			success = false;
		}
		if( !feedbackTextures[1].loadFromRenderedText( "Good", textColor, font_big) )
		{
			printf( "Unable to render Good texture!\n" );
			success = false;
		}
		if( !feedbackTextures[0].loadFromRenderedText( "Miss", textColor, font_big) )
		{
			printf( "Unable to render Miss texture!\n" );
			success = false;
		}
	}

	return success;
}

void close(){
	// free font textures
	// timeTexture.free();
	pointsTexture.free();
	for(int i = 0; i < 2; i++)
	{
		feedbackTextures[i].free();
	}
	promptTextures[0].free();
	promptTextures[1].free();
	for(int i = 0; i < col_num; i++)
	{
		promptKeysTextures[i].free();
	}

	//Free global font
	TTF_CloseFont( font );
	TTF_CloseFont(font_big);
	font_big = NULL;
	font = NULL;

	cleanup(renderer, window);

	// destroy button textures
	// don't have to destory rectangles i think? only textures
	for (int i = 0; i < col_num; i++) {
		cleanup(bt_images[i]);
		cleanup(short_images[i]);
	}

	Mix_FreeMusic( calorie );
    calorie = NULL;

	TTF_Quit();
	IMG_Quit();
	Mix_Quit();
	SDL_Quit();

	if(score!=NULL)
	{
		score->~Score();
		// delete score;
	}

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
		load_goal_line();
		load_lines();

		load_buttons();

		load_font();

		load_audio();


		// main loop flag
		bool quit = false;

		// unsigned integer 32bit type for protability across platforms

		SDL_Color textColor = { 0, 0, 0, 255 };

		// while application is running
		while (!quit) {

			current_time = timer.get_current_time();

			if(timer.isStarted())
			{
				score->update_score(current_time);
				// printf("head = %d, state is %c\n", score->get_head(), score->get_head_state());
			}

			// handle events on queue
			while (SDL_PollEvent(&event)) {

				if (timer.isStarted() && (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP))
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_d:
						case SDLK_f:
						case SDLK_j:
						case SDLK_k:
							score->handle_event(current_time, event);
					}
				}

				if (event.type == SDL_QUIT)
				{
					quit = true;
				}
				else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
				{	
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						{
							quit = true;
							break;
						}

						//start stop
						case SDLK_s:
						{
							if(timer.isStarted())
							{
								timer.stop();
								score->~Score();
								Mix_HaltMusic();
								// delete score;
							}
							else // if timer is stopped
							{
								score = new Score(true);
								score->print();
								// score = new Score(getResourcePath() + "calorie.txt");
								timer.start();
								Mix_PlayMusic( calorie, -1 );
							}
							break;
						}

						// pause unpause
						case SDLK_p:
						{
							if(timer.isPaused())
							{
								timer.unpause();
								Mix_ResumeMusic();
							}
							else
							{
								timer.pause();
								Mix_PauseMusic();
							}
							break;
						}						
					}
				}
			}

			if(timer.isStarted())
			{
				score->update_head_and_feedback(current_time);
				// printf("head = %d, state is %c\n", score->get_head(), score->get_head_state());
			}

			// draw everything starting from background
			SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
			SDL_RenderClear(renderer); // encouraged for code reusability
			
			// other lines
			SDL_SetRenderDrawColor(renderer, bd_r, bd_g, bd_b, bd_a);
			for (int i = 0; i < line_num; i++) {
				if (SDL_RenderFillRect(renderer, &vt[i])) {
					logSDLError(std::cout, "RenderFillRect vt");
					cleanup(window, renderer);
					SDL_Quit();
					IMG_Quit();
					return 1;
				}
			}

			// goal line
			// SDL_SetRenderDrawColor(renderer, bd_r, goal_g, goal_b, goal_a);
			if (SDL_RenderFillRect(renderer, &goal_rect)) {
				logSDLError(std::cout, "RenderFillRect goal_rect");
				cleanup(window, renderer);
				SDL_Quit();
				IMG_Quit();
				return 1;
			}
			
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


			// score
			if(timer.isStarted())
			{
				score->render(current_time, renderer);
				pointsio.str("");
				pointsio << "Points :" << score->get_points();
				if(pointsTexture.loadFromRenderedText( pointsio.str().c_str(), textColor,font))
				{
					pointsTexture.render("top center", renderer);
				}
				else
				{
					printf( "Unable to render points texture!\n" );
				}


				int feedback = score->get_feedback();
				if(feedback > existing && feedback <=perfect)
				{
					int temp = 255*(1-(current_time - score->get_feedback_start_time())/500.f);// feedback fade duration 500
					int alpha = std::max(0,temp); 
					feedbackTextures[feedback].setAlpha(alpha);
					feedbackTextures[feedback].render("center", renderer);
				}
			}
			else // stop screen rendering
			{
				// S, P controls
				int vertical_margin = 10;
				int total_height = promptTextures[0].getHeight() + promptTextures[1].getHeight() + vertical_margin;
				int top_margin = (SCREEN_HEIGHT - total_height)/2;
				promptTextures[0].render((SCREEN_WIDTH - promptTextures[0].getWidth())/2, top_margin, renderer);
				promptTextures[1].render((SCREEN_WIDTH - promptTextures[1].getWidth())/2, top_margin + vertical_margin + promptTextures[0].getHeight(), renderer);

				// hit buttons
				for(int i = 0; i < col_num; i++)
				{
					int left_margin = (col_width - promptKeysTextures[i].getWidth())/2;
					int top_margin = (col_width - promptKeysTextures[i].getHeight())/2;
					promptKeysTextures[i].render(i*col_width + left_margin, goal_height + top_margin, renderer);
				}
			}
			

			// time texture
			// timeio.str("");
			// timeio << "Seconds: " << current_time/1000.f;
			// text to texture
			// if( !timeTexture.loadFromRenderedText( timeio.str().c_str(), textColor,font ) )
			// {
			// 	printf( "Unable to render time texture!\n" );
			// }
			// timeTexture.render( 0, SCREEN_HEIGHT/2 );

			SDL_RenderPresent(renderer);
		}
	}



	close();

	return 0;
}