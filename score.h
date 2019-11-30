#ifndef SCORE_H
#define SCORE_H

#include "note.h"
#include "constants.h"

#include <SDL.h>

class Score
{
    public:

    //Initialize
    Score()
    {
        //make four notes that appear two seconds after every press of "start"

        // printf("creating instance of Score using current_time = %d\n", current_time);
        // current_time = current_time % 2**32;
        total = 4;
        head = 0;
        notes = new Note*[total]();
        for (int i = 0; i < total; i++)
        {
            // printf("note %d initializing\n", i);
            notes[i] = new Note(2000 + i*500);
            // don't do notes[i] = &Note(), cannot take address of a temporary object
            // don't do *notes[i] = Note(current_time + 2000 + i*500), complies but crashes.
            // If use "new", then Note object exists until deleted
            // If don't use "new", Note object only exists in current scope

            // printf("note %d initialized\n", i);
        }
        // printf("instance of score created\n");
    }

    ~Score()
    {
	    for (int i = 0; i < total; i++) {
            if (notes[i] != NULL)
            {
                delete notes[i];
            }
        }
        // printf("indivisual notes deleted\n");
        delete[] notes;
        // printf("notes pointer deleted");
	}

    void print()
    {
        for(int i = head; i < total; i++)
        {
            printf("note[%d]: ",i);
            notes[i]->print();
        }
    }

    void update_score(Uint32 current_time)
    {
        if(head < total) // if we haven't finished going through the score
        {
            // printf("updating score");
            int i = head;
            while (i < max_notes && i < total)
            {
                // printf("updating note %d", i);
                notes[i]->update_state(current_time);
                notes[i]->update_position(current_time);
                i++;
            }
            // printf("updated state according to time and position\n");
            if(notes[head]->get_state() == 't') // if first note is terminated
            {
                head++;
            }
        }
    }

    void handle_event(Uint32 current_time, SDL_Event& e)
    {
        if(head < total)
        {
            char head_state = notes[head]->get_state();
            printf("head state is %c\n", head_state);

            if(head_state == 'e')
            {
                // printf("handling head event\n");
                notes[head]->handle_event(e, current_time);
            }
        }

    }

    void render(Uint32 current_time, SDL_Renderer* renderer)
    {
        // printf("rendering score");
        if(head < total)
        {
            int i = head;
            while (i < max_notes && i < total)
            {
                if (notes[i]->get_state() == 'e')
                {
                    notes[i]->render(renderer);
                }
                i++;
            }
        }

    }

    int get_total()
    {
        return total;
    }

    char get_head_state()
    {
        return notes[head]->get_state();
    }

    int get_head()
    {
        return head;
    }

    private:

    // array of Note*
    Note** notes;

    int total; // total number of notes all notes in column

    int head; // current head


};

#endif