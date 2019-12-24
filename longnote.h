#ifndef LONGNOTE_H
#define LONGNOTE_H

#include "note.h"

class LongNote : public Note
{
public:
    LongNote(){};

    LongNote(int col_num, Uint32 given_press_time, Uint32 given_release_time, float given_speed = 0.5)
    {
        if(given_press_time < given_release_time)
        {
            speed = given_speed; // pixels travelled per millisecond

            press_hit = given_press_time;
            press_takeoff = press_hit - goal_height / speed;
            press_disappear = press_takeoff + SCREEN_HEIGHT / speed;

            release_hit = given_release_time;
            release_takeoff = release_hit - goal_height / speed;
            release_disappear = release_takeoff + SCREEN_HEIGHT / speed;

            rect->x = col_num * col_width;
            rect->y = 0;
            rect->w = col_width - bd_thickness;
            rect->h = speed * (release_hit - press_hit);

            state = irrelevent;
        }
        else
        {
            printf("LongNote initialization error: given press time >= given release time");
        }
    }

    void update_state(Uint32 current_time)
    {
        // first appearing on screen
        if (state == irrelevent && current_time >= press_takeoff)
        {
            state = existing;
        }

        // bottom of rectangle reaches goal line
        else if (state == existing && current_time > press_disappear)
        {
            state = miss;
        }

        // top of rectangle reaches goal line
        else if (state > miss && current_time > release_disappear-1)
        {
            state = miss;
        }
    }

    void update_position(Uint32 current_time)
    {
        if (state==existing)
        {
            rect->y = (current_time - release_takeoff) * speed;
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

    print()
    {
        printf("press hit = %d, release hit = %d, state = %d \n", press_hit, release_hit, state);
    }

    // inherits render function as is

private:
    float speed = 0.0;

    Uint32 press_hit = 0;
    Uint32 press_takeoff = 0;
    Uint32 press_disappear = 0;

    Uint32 release_hit = 0;
    Uint32 release_takeoff = 0;
    Uint32 release_disappear = 0;

    SDL_Rect* rect = NULL;

    int state = irrelevent;
}
#endif