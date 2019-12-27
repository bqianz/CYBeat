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

    SDL_Rect *rect;
    int position;

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

        rect->x = col_num * col_width;
        rect->y = 0;
        rect->w = col_width - bd_thickness;
        rect->h = notes_thickness;

        position = 0;

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

    virtual void update_position(Uint32 current_time)
    {
        if (state == existing)
        {
            position = (current_time - takeoff) * speed;
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

            rect->y = position - goal_thickness/2;
            if (SDL_RenderFillRect(renderer, rect))
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
        printf("hit time = %d, state = %d \n", hit, state);
    }

    int get_state() { return state; }

    char get_type() {return type;}

    virtual void render_block(SDL_Renderer *renderer){if(false){printf("ha");}}
};


class ReleaseNote : public Note
{
private:
    SDL_Rect* rect_block;
    Uint32 press;
    int state_block;

public:
    ReleaseNote(int col_num, Uint32 given_press_hit, Uint32 given_release_hit, float given_speed = 0.5)
    : Note(col_num, given_release_hit, given_speed)
    {
        type = 'r'; // r for release
        if(given_press_hit < given_release_hit)
        {
            press = given_press_hit;

            rect_block->x = col_num * col_width;
            rect_block->y = 0;
            rect_block->w = col_width - bd_thickness;
            rect_block->h = speed * (given_release_hit - given_press_hit);
        }
        else
        {
            printf("PressNote initialization error: given press time >= given release time");
        }
    }

    void update_position(Uint32 current_time)
    {
        if (state < miss) // if existing or irrelevent
        {
            position = (current_time - takeoff) * speed;
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

    void render_block(SDL_Renderer *renderer)
    {
        // render block
        if(state !=miss)
        {
            SDL_SetRenderDrawColor(renderer, block_r, block_g, block_b, block_a);
            rect_block->y = position;
            if (SDL_RenderFillRect(renderer, rect_block))
            {
                logSDLError(std::cout, "RenderFillRect block rect");
            }
        }
    }

    void render(SDL_Renderer *renderer) // render a single note
    {   
        // render release note
        if (state == existing)
        {
            SDL_SetRenderDrawColor(renderer, release_r, release_g, release_b, release_a);
            rect->y = position - goal_thickness/2;

            if (SDL_RenderFillRect(renderer, rect))
            {
                logSDLError(std::cout, "RenderFillRect release rect");
            }
        }
    }
};


#endif
