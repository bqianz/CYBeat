#ifndef SCORE_H
#define SCORE_H

#include "note.h"
#include "constants.h"

#include <SDL.h>

#include <fstream>
#include <iostream>

class Score
{
private:
    Note** notes[col_num];
    // notes = {col1, col2, col3, col4}
    // *col1 = {note1, note2, ...} (not fixed length)
    // col1 is of type Note**
    // note1 is a pointer to a Note object

    int total[col_num] = {0,0,0,0}; // total number of notes for each column

    int head[col_num] = {0,0,0,0}; // current head for each column

    Uint32 points = 0; // numerical score

    int feedback = irrelevent; // _, missed, good, perfect
    Uint32 feedback_start_time = 0;

public:
    //Initialize
    Score()
    {
        for (int i = 0; i < col_num; i++)
        {

            int n_col = col_num - i; // number of notes in column i

            total[i] = n_col;

            notes[i] = new Note*[n_col]();

            for (int j = 0; j < n_col; j++)
            {
                // printf("note %d initializing\n", i);
                notes[i][j] = new Note(i, 2000 + j * 500);
                // don't do notes[i] = &Note(), cannot take address of a temporary object
                // don't do *notes[i] = Note(current_time + 2000 + i*500), complies but crashes.
                // If use "new", then Note object exists until deleted
                // If don't use "new", Note object only exists in current scope

                // printf("note %d initialized\n", i);
            }
            // printf("instance of score created\n");
        }
    }

    Score(std::string filepath)
    {

        Uint32 time;
        int i;
        std::ifstream infile(filepath);
        std::string line;

        // find out how how many notes in each column
        while (std::getline(infile, line))
        {
            std::size_t found = (line.find("点") == std::string::npos)? line.find("按") : line.find("点");
            if (found != std::string::npos)
            {
                std::istringstream iss(line.substr(found+4));
                iss >> time >> i;
                total[i]++;
            }
        }
        // make arrays of given sizes
        for(int i = 0 ; i < col_num; i++)
        {
            notes[i] = new Note*[total[i]]();
        }

        // reset file stream
        infile.clear();
        infile.seekg(0, std::ios::beg);
        // make notes
        int temp[] = {0,0,0,0};
        // std::getline(infile, line);
        while (std::getline(infile, line))
        {
            std::size_t found = (line.find("点") == std::string::npos)? line.find("按") : line.find("点");
            if (found != std::string::npos)
            {
                std::istringstream iss(line.substr(found+4));
                iss >> time >> i;
                // std::cout<<time<<"\n";

                int j = temp[i];
                notes[i][j] = new Note(i, time);

                temp[i]++;
            }
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
                printf("note[%d][%d]: ", i, j);
                notes[i][j]->print();
            }
        }
    }

    void update_score(Uint32 current_time) // update status of notes in all columns
    {
        for (int i = 0; i < col_num; i++)
        {
            int head_of_col = head[i];
            int total_of_col = total[i];
            if (head_of_col < total_of_col) // if we haven't finished going through the score
            {
                // printf("updating score");
                int tolerance = std::min(head_of_col + max_notes, total_of_col);

                for (int j = head_of_col; j < tolerance; j++)
                {
                    // printf("updating note %d", i);
                    notes[i][j]->update_state(current_time);
                    notes[i][j]->update_position(current_time);
                }
                // printf("updated state according to time and position\n");
            }
        }
    }


    void update_head(Uint32 current_time)
    {
        for (int i = 0; i < col_num; i++)
        {
            if(head[i] < total[i] && notes[i][head[i]]->increment_head(current_time))
            {
                head[i]++;
            }
        }
    }

    void update_feedback(Uint32 current_time)
    {
        int temp = perfect + 1;

        for (int i = 0; i < col_num; i++)
        {
            int hs = notes[i][head[i]]->get_state(); // head state
            if (hs > existing)
            {
                head[i]++;
                temp = std::min(temp, hs);
            }
        }

        // temp is the lower limit of head state of all 4 columns
        // if none of the head of the columns had their feedback updated,
        // then temp = perfect+1 (which is meaningless)
        // existing < temp <= perfect

        if (temp <= perfect && temp > existing) 
        // if lower limit is not meaningless
        {
            feedback = temp;
            feedback_start_time = current_time;
        }
    }

    // before entering this, it's been checked that timer is started
    void handle_event(Uint32 current_time, SDL_Event* event)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_d: int i = 0;
            case SDLK_f: int i = 1;
            case SDLK_j: int i = 2;
            case SDLK_k: int i = 3;
        }

        int head_of_col = head[i];
        int total_of_col = total[i];

        if (head_of_col < total_of_col)
        {
            // printf("handling head event in column %d\n", i);
            points += notes[i][head_of_col]->handle_event(current_time, event);
            // printf("%d\n", points);
            }
        }
    }

    int get_feedback()
    {
        return feedback;
    }

    Uint32 get_feedback_start_time()
    {
        return feedback_start_time;
    }

    void render(Uint32 current_time, SDL_Renderer *renderer)
    {
        for (int i = 0; i < col_num; i++)
        {
            int head_col = head[i];
            int total_col = total[i];
            if (head_col < total_col)
            {
                int tolerance = std::min(head_col + max_notes, total_col);
                for (int j = head_col; j < tolerance; j++)
                {
                    notes[i][j]->render(renderer);
                }
            }
        }
    }

    int get_total(int i)
    {
        return total[i];
    }

    // int get_head_state(int i)
    // {
    //     return notes[i][head[i]]->get_state();
    // }

    Uint32 get_points()
    {
        return points;
    }
};

#endif