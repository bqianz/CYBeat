#ifndef SCORE_H
#define SCORE_H

#include "note.h"
#incliude "constants.h"

#include <SDL.h>

class Score
{
    public:
    
    //Initialize
    Score()
    {
        //make four notes that appear at every press of "start"
        num = 4;
        notes = new Note*[4];
        for (int i = 0; i < num; i++)
        {
            notes[i] = &Note(2000 + i*500);
        }
    }

    ~Score()
    {
	    for (int i = 0; i < num; i++) {
            if (notes[i] != NULL)
            {
                delete notes[i];
            }
        }
        delete[] notes;
	}


    void update_score(Uint32 current_time, SDL_Event e)
    {
        int i= 0;
        while (i < max_num_notes_per_col && i < num)
        {
            notes[i]->update_state(current_time);
        }
        i++;

        notes[0]->handleEvent(&e, current_time);

        if(note[0]->get_state == 't') // if first note is terminated
        {
            // cut off first note
            Notes* temp = notes[0];
            notes[0] = notes[1];
            delete temp;
            num--;
        }
    }

    void render(Uint32 current_time, SDL_Renderer* renderer)
    {
        int i = 0;
        while (i < max_num_notes_per_col && i < num)
        {
            if (note[i]->get_state == 'e')
            {
                note[i]->render(renderer);
            }
        }
        i++;
    }




    private:

    // array of Note*
    Note** notes;

    int num; // number of notes remaining in col (just one column so far)

    // TODO: need to think about how many notes to loop through for looping

}

#endif