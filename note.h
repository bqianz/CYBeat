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
    Note(){};

    //Initializes the variables
    Note(int col_num, Uint32 given_time, float given_speed = 0.5);

    // only modifies state
    virtual void update_state(Uint32 current_time);

    virtual void update_position(Uint32 current_time);

    // called by head note in a column
    virtual Uint32 handle_event(Uint32 current_time, SDL_Event *event); // returns points

    virtual void render(SDL_Renderer *renderer, Uint32 current_time); // render a single note

    virtual void print();

    virtual bool increment_head(Uint32 current_time) {return state > existing;}

protected:
    float speed = 0.0;

    Uint32 perfect_hit_time = 0;
    Uint32 takeoff_time = 0;
    Uint32 disappear_time = 0;

    SDL_Rect *rect = NULL; // position along track is in rect.y

    // bool showing;
    // bool is_hit;

    int state = irrelevent;
};

Note::Note(int col_num, Uint32 given_time, float given_speed = 0.5)
{
    //printf("intializing note\n");
    speed = given_speed;
    perfect_hit_time = given_time;
    takeoff_time = perfect_hit_time - goal_height / speed;
    disappear_time = takeoff_time + SCREEN_HEIGHT / speed;

    rect->x = col_num * col_width;
    rect->y = 0;
    rect->w = col_width - bd_thickness;
    rect->h = notes_thickness;

    state = irrelevent;
}

virtual void Note::update_state(Uint32 current_time)
{
    if (state == irrelevent && current_time >= takeoff_time)
    {
        state = existing;
    }
    else if (state == existing && current_time > disappear_time)
    {
        state = miss;
    }
}

virtual void Note::update_position(Uint32 current_time)
{
    if (state == existing)
    {
        rect->y = (current_time - takeoff_time) * speed;
    }
}

virtual Uint32 Note::handle_event(Uint32 current_time, SDL_Event *event)
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
        if (perfect_hit_time - perfect_range < current_time &&
            current_time < perfect_hit_time + perfect_range)
        {
            state = perfect;
            points = points_array[state];
        }
        else if (perfect_hit_time - good_range < current_time &&
                 current_time < perfect_hit_time + good_range)
        {
            state = good;
            points = points_array[state];
        }
        else if (perfect_hit_time - bad_range < current_time &&
                 current_time < perfect_hit_time + bad_range)
        {
            state = miss;
            points = points_array[state];
        }
    }

    return points;
}

virtual void Note::render(SDL_Renderer *renderer, Uint32 current_time) // render a single note
{
    // distance_left_to_travel = (perfect_hit_time - current_time) * speed
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

virtual void Note::print()
{
    printf("hit time = %d, state = %d \n", perfect_hit_time, state);
}

#endif
