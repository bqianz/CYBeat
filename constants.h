#ifndef CONSTANTS_H
#define CONSTANTS_H

// window resolution
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 600;

const int col_num = 4;
const std::string keys[col_num] = {"D", "F", "J", "K"};
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

const int block_r = 0;
const int block_g = 0;
const int block_b = 0;
const int block_a = 127;

const int release_r = 255;
const int release_g = 255;
const int release_b = 255;
const int release_a = 255;

const Uint32 perfect_range = 100; // 200 milliseconds
const Uint32 good_range = 200;
const Uint32 bad_range = 300;

const Uint32 points_array[3] = {0,200,500};
// const Uint32 range_array[3] = {300,200,100};


// note states
const int irrelevent = -2;
const int existing = -1;
const int miss = 0;
const int good = 1;
const int perfect = 2;

// font
const std::string font_name = "lazy.ttf";
const int regular_font_size = 22;
const int big_font_size = 50;

#endif
