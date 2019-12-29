#ifndef NOTE_H
#define NOTE_H

#include "constants.h"
#include "functions.h"

#include <SDL.h>
#include <iostream>

class Note
{
protected:
    float speed;

    Uint32 hit;
    Uint32 takeoff;
    Uint32 finish;

    SDL_Rect rect;

    int state;
    char type;

public:
    Note(){};

    Note(int col_num, Uint32 given_time, float given_speed = 0.5)
    {
        type = 'n'; // n for note
        speed = given_speed;
        hit = given_time;
        takeoff = hit - goal_height / speed;
        finish = takeoff + SCREEN_HEIGHT / speed;

        rect.x = col_num * col_width;
        rect.y = 0;
        rect.w = col_width - bd_thickness;
        rect.h = notes_thickness;

        state = irrelevent;
    }

    // version with one argument
    virtual void update_state_without_event(Uint32 current_time, int prev = irrelevent)
    {
        if (state == irrelevent && current_time >= takeoff)
        {
            state = existing;
        }
        else if (state == existing && current_time > finish)
        {
            state = miss;
        }
    }

    virtual void update_position(Uint32 current_time, int prev = irrelevent)
    {
        if(state==existing) // makes sure current_time >= takeoff
        {
            rect.y = (current_time - takeoff) * speed;
        }
    }

    virtual Uint32 handle_event(Uint32 current_time)
    {
        if (hit - perfect_range < current_time &&
            current_time < hit + perfect_range)
        {
            state = perfect;
            return points_array[state];
        }
        else if (hit - good_range < current_time &&
                 current_time < hit + good_range)
        {
            state = good;
            return points_array[state];
        }
        else if (hit - bad_range < current_time &&
                 current_time < hit + bad_range)
        {
            state = miss;
            return points_array[state];
        }
        else
        {
            return 0;
        }
    }

    virtual void render(SDL_Renderer *renderer)
    {
        if (state == existing)
        {
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
    }

    void print()
    {
        printf("hit time = %d, state = %d, type = %c\n", hit, state, type);
    }

    int get_state() { return state; }

    char get_type() { return type; }

    Uint32 get_time() {return hit;}

    virtual void set_time(Uint32 given_time)
    {
        hit = given_time;
        takeoff = hit - goal_height / speed;
        finish = takeoff + SCREEN_HEIGHT / speed;
    }

    virtual void render_block(SDL_Renderer *renderer, int prev = irrelevent){}
};

class ReleaseNote : public Note
{
private:
    SDL_Rect rect_block;
    Uint32 press;
    int original_height;


public:
    ReleaseNote(int col_num, Uint32 given_press, Uint32 given_release, float given_speed = 0.5)
        : Note(col_num, given_release, given_speed)
    {
        type = 'r'; // r for release
        if (given_press < given_release)
        {
            rect_block.x = col_num * col_width;
            rect_block.w = col_width - bd_thickness;

            press = given_press;
            original_height = speed * (given_release - given_press);
        }
        else
        {
            printf("ReleaseNote initialization error: pres >= release\n");
        }
    }

    void set_time(Uint32 given_time)
    {
        Note::set_time(given_time);
        original_height = speed * (given_time - press);
    }

    void update_position(Uint32 current_time, int prev = irrelevent)
    {
        //update note rect
        Note::update_position(current_time);

        //update block rect
        if(state!=miss && prev > irrelevent)
        {
            if(current_time < takeoff) // block cut off from the top
            {
                rect_block.y = 0;
                rect_block.h = original_height - (takeoff-current_time) * speed;
            }
            else
            {
                rect_block.y = (current_time - takeoff) * speed;
                rect_block.h = original_height;
            }
            
            // make rectangle cut off at goal line
            if(prev > miss)
            {
                if(rect_block.y > goal_height)
                {
                    rect_block.h = 0;
                }
                else if(rect_block.y + rect_block.h > goal_height)
                {
                    rect_block.h = goal_height - rect_block.y;
                }
            }
            
        }
    }

    void update_state_without_event(Uint32 current_time, int prev)
    {
        if (prev == miss)
        {
            state = miss;
        }

        else
        {
            Note::update_state_without_event(current_time);
        }
    }

    void render_block(SDL_Renderer *renderer,int prev = irrelevent)
    {
        if (state != miss && prev > irrelevent)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, block_r, block_g, block_b, block_a);
            if (SDL_RenderFillRect(renderer, &rect_block))
            {
                logSDLError(std::cout, "RenderFillRect block rect");
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }

    void render(SDL_Renderer *renderer)
    {
        // render release note
        if (state == existing)
        {
            SDL_SetRenderDrawColor(renderer, release_r, release_g, release_b, release_a);

            if (SDL_RenderFillRect(renderer, &rect))
            {
                logSDLError(std::cout, "RenderFillRect release rect");
            }
        }
    }

    Uint32 handle_event(Uint32 current_time)
    {
        if (hit - perfect_range < current_time &&
            current_time < hit + perfect_range)
        {
            state = perfect;
            return points_array[state];
        }
        else if (hit - good_range < current_time &&
                 current_time < hit + good_range)
        {
            state = good;
            return points_array[state];
        }
        else
        {
            state = miss;
            return points_array[state];
        }
    }
};

#endif
