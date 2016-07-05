// Copyright(c) 1996 Leendert Ammeraal. All rights reserved.
// This program text occurs in Chapter 7 of
//
//    Ammeraal, L. (1996) Algorithms and Data Structures in C++,
//       Chichester: John Wiley.

// showfile: Show contents of B-tree fs_
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

const int M = 1000;  // Order of B-tree: M link fields in each Node

typedef int KeyType;

struct Node
{
    int n;        // Number of items stored in a Node (n < M)
    KeyType k[M - 1]; // Data items (only the first n in use) k[0]~k[n-1]有效
    long p[M];    // 'Pointers' to other nodes (n+1 in use)　p[0]~p[n]有效
};

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
    cout << "root: " << start[0] << " free_list: " << start[1] << endl;
    Node node;

    for (; ;)
    {
        pos = file.tellg();
        file.read((char*) &node, sizeof(node));
        if (file.fail())
        {
            break;
        }

        cout << endl << "Position " << setw(8) << pos << ": ";
        cout << "n = " << node.n << endl << "Data : ";

        for (i = 0; i < node.n; ++i)
        {
            cout << setw(6) << node.k[i] << " ";
        }

        cout << endl << "Links: ";

        for (i = 0; i <= node.n; ++i)
        {
            cout << setw(8) << node.p[i] << " ";
        }

        cout << endl;
    }

    return 0;
}
