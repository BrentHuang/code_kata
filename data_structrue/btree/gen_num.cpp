// Copyright(c) 1996 Leendert Ammeraal. All rights reserved.
// This program text occurs in Chapter 7 of
//
//    Ammeraal, L. (1996) Algorithms and Data Structures in C++,
//       Chichester: John Wiley.

// gennum: Generation of a textfile with integers,
//         to be read by program disktree.
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

class IntFile
{  // Creating a fs_ with integers
public:
    IntFile()
    {
        char fname[50];
        cout << "Name of output file: ";
        cin >> setw(50) >> fname;

        ofs_.open(fname, ios::out);
        if (ofs_.fail())
        {
            cout << "Cannot open output file." << endl;
            exit(1);
        }
    }

    ~IntFile()
    {
        ofs_.close();
    }

    void write(int x)
    {
        static int cnt = 0;
        ofs_ << x;

        if (++cnt == 10)
        {
            ofs_ << endl;
            cnt = 0;
        }

        else ofs_ << ' ';
    }

private:
    ofstream ofs_;
};

int main()
{
    long i, n;
    srand(time(NULL));

    cout << "How many integers are to be generated? ";
    cin >> n;

    IntFile output;
    for (i = 0; i < n; i++)
    {
        output.write(rand());
    }

    return 0;
}
