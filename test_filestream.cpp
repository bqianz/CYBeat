#include <string>
#include <fstream>
#include <iostream>

using namespace std;
int main()
{
    ifstream infile("res\\calorie.txt");
    string line;
    string prev_line;
    bool temp = false;
    

    int line_count = 0;
    while (std::getline(infile, line))
    {
        line_count++;
    }
    printf("line count  = %d \n", line_count);

    // reset file stream
    infile.clear();
    infile.seekg(0, std::ios::beg);

    // while (std::getline(infile, line))
    // {
    //     // size_t found = (line.find("点") == string::npos)? line.find("按") : line.find("点");

    //     if (line.find("滑") != string::npos) // if found
    //     {
    //         if (prev_line.find("按") != string::npos)
    //         {
    //             cout << prev_line << "\n"
    //                  << line << "\n";
    //         }
    //         // cout << line << "\n";
    //         // printf("cn char found at position %d \n", found);
    //         // printf("0 found at position %d \n", line.find("0"));
    //     }

    //     prev_line = line;
    // }

    return 0;
}