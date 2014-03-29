#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int file_io_test() {
    string line;
    ifstream file ("example.txt");
    int start = clock();
    if (file.is_open())
    {
        while ( getline(file, line) )
            cout << line << endl;

        file.close();
    }
    else 
    {
        cout << "Unable to open file" << endl; 
    }
    printf("%.3lf\n",double(clock()-start)/CLOCKS_PER_SEC);
    return 0;
}
