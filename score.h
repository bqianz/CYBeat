#ifndef SCORE_H
#define SCORE_H

#include "note.h"
#include "constants.h"

#include <SDL.h>

class Score
{
    private:

    Note** notes[col_num]; 
    // notes = {col1, col2, col3, col4}
    // col1 = {note1, note2, ...} (not fixed length)
    // note1 is a pointer to a Note object

    int total[col_num]; // total number of notes for each column

    int head[col_num]; // current head for each column

    Uint32 points; // numerical score


    public:

    //Initialize
    Score()
    {
        for (int i = 0; i < col_num; i++)
        {
            points = 0;

            int n_col = col_num - i; // number of notes in column i
            
            total[i] = n_col;
            head[i] = 0;

            notes[i] = new Note*[n_col]();

            for (int j = 0; j < n_col; j++)
            {
                // printf("note %d initializing\n", i);
                notes[i][j] = new Note(i, 2000 + j*500);
                // don't do notes[i] = &Note(), cannot take address of a temporary object
                // don't do *notes[i] = Note(current_time + 2000 + i*500), complies but crashes.
                // If use "new", then Note object exists until deleted
                // If don't use "new", Note object only exists in current scope

                // printf("note %d initialized\n", i);
            }
        // printf("instance of score created\n");
        }
    }

    ~Score()
    {
        for (int i = 0; i < col_num; i++)
        {
            for (int j = 0; j < total[i]; j++)
            {
                if (notes[i][j] != NULL)
                {
                    delete notes[i][j];
                }
            }
            delete[] notes[i];
        }
	}

    void print()
    {
        for (int i = 0; i < col_num; i++)
        {
            for (int j = head[i]; j < total[i]; j++)
            {
                printf("note[%d][%d]: ",i,j);
                notes[i][j]->print();
            }
        }
    }

    void update_score(Uint32 current_time) // update status of notes in all columns
    {
        for(int i = 0; i < col_num; i++)
        {
            int head_of_col = head[i];
            int total_of_col = total[i];
            if(head_of_col < total_of_col) // if we haven't finished going through the score
            {
                // printf("updating score");
                int tolerance = std::min(head_of_col+max_notes, total_of_col);

                for(int j = head_of_col; j< tolerance; j++)
                {
                    // printf("updating note %d", i);
                    notes[i][j]->update_state(current_time);
                    notes[i][j]->update_position(current_time);
                }
                // printf("updated state according to time and position\n");
                if(notes[i][head_of_col]->get_state() == 't') // if first note is terminated
                {
                    head[i]++;
                }
            }
        }
    }

    void handle_event(Uint32 current_time, int i)
    // handle event in column i
    {
        int head_of_col = head[i];
        int total_of_col = total[i];

        if(head_of_col < total_of_col)
        {
            char head_state = notes[i][head_of_col]->get_state();
            // printf("head state in col %d is %c\n", i, head_state);

            if(head_state == 'e')
            {
                // printf("handling head event in column %d\n", i);
                points += notes[i][head_of_col]->handle_event(current_time);
                printf("%d\n", points);
            }
        }
    }

    void render(Uint32 current_time, SDL_Renderer* renderer)
    {
        for(int i = 0; i < col_num; i++)
        {
            int head_col = head[i];
            int total_col = total[i];
            if(head_col < total_col)
            {
                int tolerance = std::min(head_col+max_notes, total_col);
                for(int j = head_col; j < tolerance; j++)
                {
                    // printf();
                    if (notes[i][j]->get_state() == 'e')
                    {
                        notes[i][j]->render(renderer);
                        // printf("rendered note[%d][%d]\n", i,j);
                    }
                }
            }
            // printf("rendered column %d\n", i);
        }
    }

    int get_total(int i)
    {
        return total[i];
    }

    char get_head_state(int i)
    {
        return notes[i][head[i]]->get_state();
    }

    Uint32 get_points()
    {
        return points;
    }
};

#endif