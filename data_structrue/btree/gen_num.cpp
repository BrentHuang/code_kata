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

class intfile
{  // Creating a file with integers
public:
    intfile()
    {
        char fname[50];
        cout << "Name of output file: ";
        cin >> setw(50) >> fname;
        out.open(fname, ios::out);
        if (out.fail())
        {
            cout << "Cannot open output file.\n";
            exit(1);
        }
    }

    ~intfile()
    { out.close(); }

    void write(int x)
    {
        static int cnt = 0;
        out << x;
        if (++cnt == 10)
        {
            out << '\n';
            cnt = 0;
        }
        else out << ' ';
    }

private:
    ofstream out;
};

int main()
{
    long i, n;
    srand(int(time(NULL)));
    cout << "How many integers are to be generated? ";
    cin >> n;
    intfile output;
    for (i = 0; i < n; i++) output.write(rand());
    return 0;
}

