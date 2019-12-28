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
    Note **notes[col_num];
    // notes = {col1, col2, col3, col4}
    // *col1 = {note1, note2, ...} (not fixed length)
    // col1 is of type RegularNote**
    // note1 is a pointer to a RegularNote object

    int total[col_num] = {}; // total number of notes for each column

    int head[col_num] = {0, 0, 0, 0}; // current head for each column

    Uint32 points = 0; // numerical score

    int feedback = irrelevent; // _, missed, good, perfect
    Uint32 feedback_start_time = 0;

public:
    //Initialize sample score
    Score()
    {
        for (int i = 0; i < col_num; i++)
        {

            int n_col = col_num - i; // number of notes in column i

            total[i] = n_col;

            notes[i] = new Note *[n_col]();

            for (int j = 0; j < n_col; j++)
            {
                // printf("note %d initializing\n", i);
                notes[i][j] = new Note(i, 2000 + j * 500);
                // don't do notes[i] = &RegularNote(), cannot take address of a temporary object
                // don't do *notes[i] = RegularNote(current_time + 2000 + i*500), complies but crashes.
                // If use "new", then RegularNote object exists until deleted
                // If don't use "new", RegularNote object only exists in current scope

                // printf("note %d initialized\n", i);
            }
            // printf("instance of score created\n");
        }
    }

    // constructor to test simple long note
    Score(bool arg)
    {
        if(arg)
        {
            //make long note in 3rd column
            total[0] = 2;
            notes[0] = new Note*[2]();
            notes[0][0] = new Note(0, 2000);
            notes[0][1] = new ReleaseNote(0,2000,4000);
        }
    }

    Score(std::string filepath)
    {

        Uint32 time;
        int i;
        std::ifstream infile(filepath);
        std::string line;
        int prev_i;
        Uint32 prev_time;
        char prev_type = 'n';
        
        int total_upper_limit[col_num] = {};
        // find out how how many notes in each column
        while (std::getline(infile, line))
        {
            std::size_t found = (line.find("点") == std::string::npos) ? ((line.find("按") == std::string::npos) ? line.find("滑") : line.find("按"))
                                                                       : line.find("点");

            if (found != std::string::npos)
            {
                std::istringstream iss(line.substr(found + 4));
                iss >> time >> i;
                total_upper_limit[i]++;
            }
        }

        // make arrays of given sizes
        for (int i = 0; i < col_num; i++)
        {
            notes[i] = new Note *[total_upper_limit[i]]();
        }

        // reset file stream
        infile.clear();
        infile.seekg(0, std::ios::beg);

        while (std::getline(infile, line))
        {
            std::size_t press_found = (line.find("点") == std::string::npos) ? line.find("按") : line.find("点");

            // if single note or pressing, create regular note
            if (press_found != std::string::npos)
            {
                std::istringstream iss(line.substr(press_found + 4));
                iss >> time >> i;
                // std::cout<<time<<"\n";

                int j = total[i];
                notes[i][j] = new Note(i, time);
                total[i]++;
                prev_i = i;
                prev_time = time;
                prev_type = 'n';
            }


            // ignore 开始 + 滑 notes

            // if possible release note
            else if (line.find("滑") != std::string::npos && line.find("开始") == std::string::npos
            && prev_type == 'n')
            {
                std::istringstream iss(line.substr(line.find("滑") + 4));
                iss >> time >> i;

                if (i == prev_i)
                {
                    int j = total[i]; // current index in column i
                    notes[i][j] = new ReleaseNote(i, prev_time, time);
                    prev_type = 'r';
                    total[i]++;
                    prev_i = i;
                    prev_time = time;
                }
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
                int tolerance = std::min(head_of_col + max_notes, total_of_col);
                for (int j = head_of_col; j < tolerance; j++)
                {
                    // if note is of type ReleaseNote
                    if (notes[i][j]->get_type() == 'r')
                    {
                        int prev = notes[i][j - 1]->get_state();
                        notes[i][j]->update_state_without_event(current_time, prev);
                        notes[i][j]->update_position(current_time,prev);
                    }
                    else
                    {
                        notes[i][j]->update_state_without_event(current_time);
                        notes[i][j]->update_position(current_time);
                    }   
                }
                // printf("updated state according to time and position\n");
            }
        }
    }

    void update_head_and_feedback(Uint32 current_time)
    {
        int temp = perfect + 1;

        for (int i = 0; i < col_num; i++)
        {
            if (head[i] < total[i])
            {
                int hs = notes[i][head[i]]->get_state(); // head state

                if (hs > existing)
                {
                    head[i]++;
                    temp = std::min(temp, hs);
                }
            }
        }
        // temp is the lower limit of head state of all 4 columns
        // if none of the head of the columns are hit, then temp = perfect+1 (which is meaningless)
        // existing < temp <= perfect+1

        if (temp <= perfect) // if lower limit is not meaningless, aka a note has been hit
        {
            feedback = temp;
            feedback_start_time = current_time;
        }
    }

    // before entering this, it's been checked that timer is started
    void handle_event(Uint32 current_time, SDL_Event& event)
    {
        int i;
        // event is guaranteed to be one of these four keys
        switch (event.key.keysym.sym)
        {
            case SDLK_d: i = 0; break;
            case SDLK_f: i = 1; break;
            case SDLK_j: i = 2; break;
            case SDLK_k: i = 3; break;
        }

        if (head[i] < total[i]) // if head index is valid
        {
            if (notes[i][head[i]]->get_type() == 'r') // if note is of type ReleaseNote
            {
                // if prev note not missed and event is key release
                // doesn't check for state of release note,in case hold is really long
                // that is, tail of note is off screen, but still need to handle if you let go too early
                // if state is good/perfect, then head increment to next note, no need to worry (??)
                if (notes[i][head[i] - 1]->get_state() != miss && event.type == SDL_KEYUP)
                {
                    points += notes[i][head[i]]->handle_event(current_time);
                }
            }

            else // regular note
            {
                if (notes[i][head[i]]->get_state() == existing && event.type == SDL_KEYDOWN && event.key.repeat == 0)
                {
                    points += notes[i][head[i]]->handle_event(current_time);
                }
            }
        }
    }

    void render(Uint32 current_time, SDL_Renderer *renderer)
    {
        // render block
        for (int i = 0; i < col_num; i++)
        {
            if (head[i] < total[i])
            {
                int tolerance = std::min(head[i] + max_notes, total[i]);
                for (int j = head[i]; j < tolerance; j++)
                {
                    if (notes[i][j]->get_type() == 'r') // if type ReleaseNote
                    {
                        int prev = notes[i][j - 1]->get_state();
                        notes[i][j]->render_block(renderer,prev);
                    }
                }
            }
        }

        // render notes
        for (int i = 0; i < col_num; i++)
        {
            if (head[i] < total[i])
            {
                int tolerance = std::min(head[i] + max_notes, total[i]);
                for (int j = head[i]; j < tolerance; j++)
                {
                    notes[i][j]->render(renderer);
                }
            }
        }
    }

    Uint32 get_points()
    {
        return points;
    }

    int get_feedback()
    {
        return feedback;
    }

    Uint32 get_feedback_start_time()
    {
        return feedback_start_time;
    }
};

#endif