#ifndef NOTE_H
#define NOTE_H

#include "constants.h"
#include "functions.h"

#include <SDL.h>

// goal_height
// SCREEN_HEIGHT
class Note
{
    public:
        //Initializes the variables
        Note(Uint32 given_time = 2000, float given_speed = 0.5)
        {
            speed = given_speed;
            hit_time = given_time;
            takeoff_time = hit_time - goal_height / speed;

            rect.x = 0;
            rect.y = 0;
            rect.w = col_width - bd_thickness;
            rect.h = notes_thickness;

            showing = false;
            is_hit = false;
        }

        bool get_showing()
        {
            return showing;
        }

        bool in_duration(Uint32 current_time)
        {
            if (is_hit)
            {
            }
            else
            {
                if(current_time <= hit_time && current_time >= takeoff_time)
                {
                    showing = true;
                    return showing;
                }
                else
                {
                    showing = false;
                    return showing;
                }
            }
            

        }

        void render(Uint32 current_time, SDL_Renderer* renderer)
        {
            // distance_left_to_travel = (hit_time - current_time) * speed
            rect.y = goal_height - (hit_time - current_time) * speed;
            if(current_time)
            SDL_SetRenderDrawColor(renderer, notes_r, notes_g, notes_b, notes_a);
			if (SDL_RenderFillRect(renderer, &rect)) {
				logSDLError(std::cout, "RenderFillRect note rect");
				// cleanup(window, renderer);
				// SDL_Quit();
				// IMG_Quit();
				// return 1;
			}
        }

        // void handleEvent(SDL_Event& e)
        // {
        //     //If a key was pressed
        //     if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
        //     {
        //         //Adjust the velocity
        //         switch( e.key.keysym.sym )
        //         {
        //             case SDLK_d: 
        //                 vel -= DOT_VEL; 
        //                 break;
        //         }
        //     }

        //     // If a key was released
        //     else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
        //     {
        //         //Adjust the velocity
        //         switch( e.key.keysym.sym )
        //         {
        //             case SDLK_UP: mVelY += DOT_VEL; break;
        //             case SDLK_DOWN: mVelY -= DOT_VEL; break;
        //             case SDLK_LEFT: mVelX += DOT_VEL; break;
        //             case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        //         }
        //     }
        // }

    private:

        float speed;

        Uint32 hit_time;

        Uint32 takeoff_time;

        SDL_Rect rect; // position along track is in rect.y

        bool is_hit;

        bool showing;

};

#endif

