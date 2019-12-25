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
protected:
    float speed;

    Uint32 hit;
    Uint32 takeoff;
    Uint32 finish;

    SDL_Rect *rect;

    int state;

public:
    Note(){};

    Note(int col_num, Uint32 given_time, float given_speed = 0.5)
    {
        speed = given_speed;
        hit = given_time;
        takeoff = hit - goal_height / speed;
        finish = takeoff + SCREEN_HEIGHT / speed;

        rect->x = col_num * col_width;
        rect->y = 0;
        rect->w = col_width - bd_thickness;
        rect->h = notes_thickness;

        state = irrelevent;
    }

    void update_state_without_event(Uint32 current_time)
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
            rect->y = (current_time - takeoff) * speed;
        }
    }

    virtual Uint32 handle_event(Uint32 current_time, SDL_Event *event)
    {
        // printf("%lu\n",current_time);
        // printf("%lu\n",perfect_hit_time);

        // if time of press is within perfect_range of perfect_hit_time
        // if perfect_hit_time - perfect_range < current_time < perfect_hit_time + perfect_range
        // # TODO: make sure perfect_hit_time - perfect_range isn't negative??
        // printf("pressing key\n");

        Uint32 points = 0;

        if (state == existing && event->type == SDL_KEYDOWN && event->key.repeat == 0)
        {
            if (hit - perfect_range < current_time &&
                current_time < hit + perfect_range)
            {
                state = perfect;
                points = points_array[state];
            }
            else if (hit - good_range < current_time &&
                     current_time < hit + good_range)
            {
                state = good;
                points = points_array[state];
            }
            else if (hit - bad_range < current_time &&
                     current_time < hit + bad_range)
            {
                state = miss;
                points = points_array[state];
            }
        }

        return points;
    }

    virtual void render(SDL_Renderer *renderer, Uint32 current_time) // render a single note
    {
        if (state == existing)
        {
            SDL_SetRenderDrawColor(renderer, notes_r, notes_g, notes_b, notes_a);
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

    virtual bool increment_head(Uint32 current_time) { return state > existing; }

    int get_state() { return state; }
};

class ReleaseNote : public Note
{
private:
    SDL_Rect* rect_block;
    Uint32 press;
    int state_block;

public:
    PressNote(int col_num, Uint32 given_press_hit, Uint32 given_release_hit, float given_speed = 0.5)
    : Note(col_num, given_release_hit, given_speed)
    {
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
        Note::update_position(current_time);

        if (state > irrelevent)
        {
            rect_block->y = (current_time - takeoff) * speed;
        }
    }


    void handle_event(Uint32 current_time, SDL_Event *event)
    {
        Uint32 points = 0;

        // first press
        if (state == existing && event->type == SDL_KEYDOWN && event->key.repeat == 0)
        {
            if (press_hit - perfect_range < current_time &&
                current_time < press_hit + perfect_range)
            {
                state = perfect;
                points = points_array[state];
            }

            else if (press_hit - good_range < current_time &&
                    current_time < press_hit + good_range)
            {
                state = good;
                points = points_array[state];
            }
            else if (press_hit - bad_range < current_time &&
                    current_time < press_hit + bad_range)
            {
                state = miss;
                points = points_array[state];
            }
        }

        // release
        else if (state > miss && event->type == SDL_KEYUP)
        {
            if (release_hit - perfect_range < current_time &&
                current_time < release_hit + perfect_range)
            {
                state = std::min(perfect,state);
                points = points_array[state];
            }
            else if (release_hit - good_range < current_time &&
                    current_time < release_hit + good_range)
            {
                state = good;
                points = points_array[state];
            }
            printf("key released");
        }

        return points;
    }

    render(SDL_Renderer *renderer, Uint32 current_time) // render a single note
    {
        // state == existing, good, perfect : full opacity
        // state == miss : half opacity
        // top of rectangle reaches finish: don't render (??)

        // if bottom of rectangle reached start line and top of rectangle hasn't reached finish
        if(state > irrelevent && current_time <= release_disappear)
        {
            int alpha;
            if(state == miss) {alpha = notes_a / 2;}
            else {alpha = notes_a;}

            SDL_SetRenderDrawColor(renderer, notes_r, notes_g, notes_b, alpha);
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


    bool increment_head(Uint32 current_time)
    {
        return current_time > release_hit;
    }

    bool update_feedback(Uint32 current_time)
    {
        // keydown, keyup, head of rect missed, tail of rect missed
        
    }

    print()
    {
        printf("press hit = %d, release hit = %d, state = %d \n", press_hit, release_hit, state);
    }

    // inherits render function as is


};


#endif
