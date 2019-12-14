#ifndef CONSTANTS_H
#define CONSTANTS_H

// window resolution
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 600;

const int col_num = 4;
const int line_num = col_num - 1;
const int col_width = SCREEN_WIDTH / col_num;

// background colours
const int bg_r = 220;
const int bg_g = 204;
const int bg_b = 255;
const int bg_a = 255;

// goal_line
const int goal_r =255;
const int goal_g =255;
const int goal_b =255;
const int goal_a =255;
const int goal_height = SCREEN_HEIGHT - col_width;
const int goal_thickness = 10;

// other lines
const int bd_r = 192;
const int bd_g = 165;
const int bd_b = 192;
const int bd_a = 255;
const int bd_thickness = 4;


// notes
const int max_notes = 6;
const int notes_r = 0;
const int notes_g = 0;
const int notes_b = 0;
const int notes_a = 255;
const int notes_thickness = 10;

const Uint32 perfect_range = 100; // 200 milliseconds
const Uint32 perfect_points = 500; 
const Uint32 good_range = 200;
const Uint32 good_points = 200;


// note states
const int irrelevent = -1;
const int existing = 0;
const int missed = 1;
const int good = 2;
const int perfect = 3;

#endif
