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
        Note(){}

        //Initializes the variables
        Note(Uint32 given_time, float given_speed = 0.5)
        {
            //printf("intializing note\n");
            speed = given_speed;
            perfect_hit_time = given_time;
            takeoff_time = perfect_hit_time - goal_height / speed;
            disappear_time = takeoff_time + SCREEN_HEIGHT / speed;

            rect.x = 0;
            rect.y = 0;
            rect.w = col_width - bd_thickness;
            rect.h = notes_thickness;

            state = 'i';
            // printf("current note initialized\n");
        }

        // updates state, rect.y
        // called by every note within the max_num_of_notes_per_col limit
        void update_state(Uint32 current_time)
        {
            // printf("current state is %c\n", state);
            if(state=='i' && current_time >= takeoff_time)
            {
                state = 'e';
                printf("note taking off at %d \n", current_time);
            }
            else if (state=='e' && current_time >= disappear_time)
            {
                state = 't';
                printf("note reached finish line at %d miliseconds with rect.y = %d\n", current_time, rect.y);
            }
            // printf("updated state is %c\n",state);
        }

        void update_position(Uint32 current_time)
        {
            if (state=='e')
            {
                rect.y = (current_time - takeoff_time) * speed;
                // printf("current_time = %lu\n",current_time);
                // printf("perfect_hit_time = %lu\n",perfect_hit_time);
                // printf("rect.y = %d\n",rect.y);
            }
        }

        char get_state()
        {
            return state;
        }

        // NOTE: this also updates state!!!
        // called by first note in a column ONLY
        void handleEvent(SDL_Event& e, Uint32 current_time)
        {
            // printf("handling event\n");   
            if (e.type == SDL_KEYDOWN)
            {
                printf("key down\n");
                if(e.key.keysym.sym == SDLK_d && e.key.repeat == 0)
                {
                    // printf("%lu\n",current_time);
                    // printf("%lu\n",perfect_hit_time);

                    // if time of press is within perfect_range of perfect_hit_time
                    // if perfect_hit_time - perfect_range < current_time < perfect_hit_time + perfect_range
                    // # TODO: make sure perfect_hit_time - perfect_range isn't negative??
                    printf("pressing key\n");
                    if(perfect_hit_time - perfect_range < current_time && current_time < perfect_hit_time+perfect_range)
                    {
                        state = 't';
                        printf("note was hit");
                    }
                }
            }
        }



        void render(SDL_Renderer* renderer) // render a single note
        {
            // distance_left_to_travel = (perfect_hit_time - current_time) * speed
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

        void print()
        {
            printf("hit time = %d, disappear time = %d, state = %c \n", perfect_hit_time, disappear_time, state);
        }

    private:

        float speed;

        Uint32 perfect_hit_time;
        Uint32 takeoff_time;
        Uint32 disappear_time;

        SDL_Rect rect; // position along track is in rect.y

        // bool showing;
        // bool is_hit;

        char state;
        // 'i' = irrelevent, 'e' = existing, 't' = terminated

};

#endif

