#include <string>
#include <fstream>
#include <iostream>

using namespace std;
int main()
{
    string str = "点";
    string str2 = "按";
    cout<<str.length()<<"\n";
    cout<<str2.length()<<"\n";
    ifstream infile("res\\calorie_copy.txt");
    string line;
    while (std::getline(infile, line))
    {
        size_t found = (line.find("点") == string::npos)? line.find("按") : line.find("点");

        if (found != string::npos)
        {
            cout << line << "\n";
            printf("cn char found at position %d \n", found);
            printf("0 found at position %d \n", line.find("0"));
        }
    }

    return 0;
}