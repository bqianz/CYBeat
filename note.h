#ifndef NOTE_H
#define NOTE_H

#include "constants.h"
#include "functions.h"

#include <SDL.h>
#include <iostream>

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

        bool check_showing(Uint32 current_time)
        {
            if(is_hit)
            {
                // printf("is_hit");
                showing = false;
            }
            else if(current_time >= takeoff_time && rect.y < SCREEN_HEIGHT - notes_thickness)
            {
                // printf("current_time = %lu\n",current_time);
                // printf("hit_time = %lu\n",hit_time);
                rect.y = (current_time - takeoff_time) * speed;
                // printf("rect.y = %d\n",rect.y);
                showing = true;
            }
            else
            {
                // printf("exited");
                showing = false;
            }

            return showing;
        }

        void render(SDL_Renderer* renderer)
        {
            // distance_left_to_travel = (hit_time - current_time) * speed
            SDL_SetRenderDrawColor(renderer, notes_r, notes_g, notes_b, notes_a);
			if (SDL_RenderFillRect(renderer, &rect))
            {
				logSDLError(std::cout, "RenderFillRect note rect");
				// cleanup(window, renderer);
				// SDL_Quit();
				// IMG_Quit();
				// return 1;
			}
        }

        void handleEvent(SDL_Event& e, Uint32 current_time)
        {
            if (e.type == SDL_KEYDOWN)
            {
                if(e.key.keysym.sym == SDLK_d && e.key.repeat == 0)
                {
                    // printf("%lu\n",current_time);
                    // printf("%lu\n",hit_time);

                    // if time of press is within perfect_range of hit_time
                    // if hit_time - perfect_range < current_time < hit_time + perfect_range
                    // # TODO: make sure hit_time - perfect_range isn't negative??
                    if(hit_time - perfect_range < current_time && current_time < hit_time+perfect_range)
                    {
                        is_hit = true;
                        // printf("note was hit");
                    }
                }
            }
        }

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

        bool showing;
        bool is_hit;

};

#endif

