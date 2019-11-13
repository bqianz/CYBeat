#include "constants.h"

// goal_height
// SCREEN_HEIGHT
class Note
{
    public:
        //Initializes the variables
        Note()
        {
            x = 0;
            y = 0;

            v = 0; // 

            hitTime = 2000;

        }

        void handleEvent(SDL_Event& e);

        void move();

        void render;

    private:
        int x, y;

        int v;

        Uint32 hitTime;

        SDL_Rect note_rect

}