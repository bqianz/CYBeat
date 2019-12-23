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

            rect.x = col_num * col_width;
            rect.y = 0;
            rect.w = col_width - bd_thickness;
            rect.h = speed * (release_hit - press_hit);

            state = irrelevent;
        }
        else
        {
            printf("LongNote initialization error: given press time >= given release time");
        }
    }

    void update_state(Uint32 current_time)
    {
        if (state == irrelevent && current_time >= press_takeoff)
        {
            state = existing;
        }
        else if (state == existing && current_time > release_disappear)
        {
            state = miss;
        }
    }
    void update_position(Uint32 current_time)
    {
        if (state==existing)
        {
            rect.y = (current_time - release_takeoff) * speed;
        }
    }

    handle_event(Uint32 current_time)
    {
        
    }


    print()
    {
        printf("press hit = %d, release hit = %d, state = %d \n", press_hit, release_hit, state);
    }

    // inherits render function as is

private:
    float speed;

    Uint32 press_hit;
    Uint32 press_takeoff;
    Uint32 press_disappear;

    Uint32 release_hit;
    Uint32 release_takeoff;
    Uint32 release_disappear;

    SDL_Rect rect;

    int state;
}
#endif