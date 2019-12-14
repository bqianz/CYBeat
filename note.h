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
        Note(int col_num, Uint32 given_time, float given_speed = 0.5)
        {
            //printf("intializing note\n");
            speed = given_speed;
            perfect_hit_time = given_time;
            takeoff_time = perfect_hit_time - goal_height / speed;
            disappear_time = takeoff_time + SCREEN_HEIGHT / speed;

            rect.x = col_num * col_width;
            rect.y = 0;
            rect.w = col_width - bd_thickness;
            rect.h = notes_thickness;

            state = irrelevent;
        }

        // updates state, rect.y
        // called by every note within the max_num_of_notes_per_col limit
        void update_state(Uint32 current_time)
        {
            if(state==irrelevent && current_time >= takeoff_time)
            {
                state = existing;
            }
            else if (state==existing && current_time >= disappear_time)
            {
                state = missed;
            }

        }

        void update_position(Uint32 current_time)
        {
            if (state==existing)
            {
                rect.y = (current_time - takeoff_time) * speed;
            }
        }

        int get_state()
        {
            return state;
        }

        // NOTE: this also updates state!!!
        // called by first note in a column ONLY
        Uint32 handle_event(Uint32 current_time)
        {
            // printf("%lu\n",current_time);
            // printf("%lu\n",perfect_hit_time);

            // if time of press is within perfect_range of perfect_hit_time
            // if perfect_hit_time - perfect_range < current_time < perfect_hit_time + perfect_range
            // # TODO: make sure perfect_hit_time - perfect_range isn't negative??
            // printf("pressing key\n");
            Uint32 points = 0;
            if(perfect_hit_time - perfect_range < current_time && current_time < perfect_hit_time+perfect_range)
            {
                state = perfect;
                points = perfect_points;
            
            }
            else if(perfect_hit_time - good_range < current_time && current_time < perfect_hit_time+good_range)
            {
                state = good;
                points = good_points;
            }
            return points;

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

        void print()
        {
            printf("hit time = %d, disappear time = %d, state = %d \n", perfect_hit_time, disappear_time, state);
        }

    private:

        float speed;

        Uint32 perfect_hit_time;
        Uint32 takeoff_time;
        Uint32 disappear_time;

        SDL_Rect rect; // position along track is in rect.y

        // bool showing;
        // bool is_hit;

        int state;

};

#endif

