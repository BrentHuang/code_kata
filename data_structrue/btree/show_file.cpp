// Copyright(c) 1996 Leendert Ammeraal. All rights reserved.
// This program text occurs in Chapter 7 of
//
//    Ammeraal, L. (1996) Algorithms and Data Structures in C++,
//       Chichester: John Wiley.

// showfile: Show contents of B-tree file
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

typedef int KeyType;
#define M 5  // Order of B-tree: M link fields in each node

int main()
{
    char fname[50];
    cout << "B-tree file name: ";
    cin >> setw(50) >> fname;
    ifstream file(fname, ios::in | ios::binary);
    // ios::binary required with MSDOS, but possibly
    // not accepted with other environments.
    if (file.fail())
    {
        cout << "Cannot open file " << fname << endl;
        exit(1);
    }
    int i;
    long start[2], pos;
    struct node
    {
        int n;    // Number of items stored in a Node (n < M)
        KeyType k[M - 1]; // Data items (only the first n in use)
        long p[M];    // 'Pointers' to other nodes (n+1 in use)
    } Node;
    file.seekg(-1L, ios::end);
    char ch;
    file.read(&ch, 1); // Read signature.
    if (ch != sizeof(int))
    {
        cout << "Wrong file format.\n";
        exit(1);
    }
    file.seekg(0L, ios::beg);
    file.read((char*) start, 2 * sizeof(long));
    cout << "root_: " << start[0] <<
    "     Freelist: " << start[1] << endl;
    for (; ;)
    {
        pos = file.tellg();
        file.read((char*) &Node, sizeof(node));
        if (file.fail()) break;
        cout << "\nPosition " << setw(8) << pos << ": ";
        cout << "n = " << Node.n << "\nData : ";
        for (i = 0; i < Node.n; i++)
            cout << setw(6) << Node.k[i];
        cout << "\nLinks: ";
        for (i = 0; i <= Node.n; i++)
            cout << setw(8) << Node.p[i];
        cout << endl;
    }
    return 0;
}

