// Copyright(c) 1996 Leendert Ammeraal. All rights reserved.
// This program text occurs in Chapter 7 of
//
//    Ammeraal, L. (1996) Algorithms and Data Structures in C++,
//       Chichester: John Wiley.

/* disktree:
   Demonstration program for a B-tree on disk. After
   building the B-tree by entering integers on the
   keyboard or by supplying them as a text file, we can
   Insert and delete items via the keyboard. We can also
   search the B-tree for a given item. Each time, the tree
   or a search path is displayed. In contrast to program
   btree, program disktree writes, reads and updates nodes
   on disk, using a binary file. The name of this file is
   to be entered on the keyboard. If a B-tree with that
   name exists, that B-tree is used; otherwise such a file
   is created.
   Caution:
      Do not confuse the (binary) file for the B-tree with
      the optional textfile for input data. Use different
      file-name extensions, such as .bin and .txt.
*/
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

#define M 5  // Order of B-tree: M link fields in each node

enum Status
{
    INSERT_NOT_COMPLETE,
    SUCCESS,
    DUPLICATE_KEY,
    UNDERFLOW,
    NOT_FOUND
};

typedef int KeyType;

struct Node
{
    int n;        // Number of items stored in a Node (n < M)
    KeyType k[M - 1]; // Data items (only the first n in use)
    long p[M];    // 'Pointers' to other nodes (n+1 in use)
};

// Logical order:
//    p[0], k[0], p[1], k[1], ..., p[n-1], k[n-1], p[n]

class BTree
{
public:
    BTree(const char* TreeFileName);
    ~BTree();
    void Insert(KeyType x);
    void insert(const char* InpFileName);

    void print()
    {
        cout << "Contents:\n";
        pr(root_, 0);
    }

    void Delete(KeyType x);
    void ShowSearch(KeyType x);
private:
    enum
    {
        NIL = -1
    };
    long root_, FreeList;
    Node RootNode;
    fstream file;
    Status ins(long r, KeyType x, KeyType& y, long& u);
    void pr(long r, int nSpace);
    int SearchInNode(KeyType x, const KeyType* k, int n) const;
    Status del(long r, KeyType x);
    void ReadNode(long r, Node& Node);
    void WriteNode(long r, const Node& Node);
    void ReadStart();
    long GetNode();
    void FreeNode(long r);
};

BTree::BTree(const char* TreeFileName)
{
    ifstream test(TreeFileName, ios::in);
    // Remove  "| ios::nocreate" if your compiler does not accept it.
    int NewFile = test.fail();
    test.clear();
    test.close();
    if (NewFile)
    {
        file.open(TreeFileName, ios::out | ios::in |
                                ios::trunc | ios::binary);
        // ios::binary required with MSDOS, but possibly
        // not accepted with other environments.
        root_ = FreeList = NIL;
        long start[2] = { NIL, NIL };
        file.write((char*) start, 2 * sizeof(long));
    } else
    {
        long start[2];
        file.open(TreeFileName, ios::out | ios::in | ios::binary); // See above note.
        file.seekg(-1L, ios::end);
        char ch;
        file.read(&ch, 1); // Read signature.
        file.seekg(0L, ios::beg);
        file.read((char*) start, 2 * sizeof(long));
        if (ch != sizeof(int))
        {
            cout << "Wrong file format.\n";
            exit(1);
        }
        root_ = start[0];
        FreeList = start[1];
        RootNode.n = 0;   // Signal for function ReadNode
        ReadNode(root_, RootNode);
        print();
    }
}

BTree::~BTree()
{
    long start[2];
    file.seekp(0L, ios::beg);
    start[0] = root_;
    start[1] = FreeList;
    file.write((char*) start, 2 * sizeof(long));
    // The remaining code of this destructor is slightly
    // different from that in the first Print of the book.
    // The length of the final binary file, including the
    // signature byte at the end, will now always be an odd
    // number, as it should be. There is a similar change in
    // the function GetNode.I am grateful to Chian Wiz from
    // Singapore, who showed me the possibility of a 'file leak',
    // that is, an unused byte, which sometimes caused problems
    // with the program 'showfile', when this was applied to
    // this binary file. Such problems should no longer occur.
    // L. A.
    char ch = sizeof(int); // Signature
    file.seekg(0L, ios::end);
    if ((file.tellg() & 1) == 0)
        file.write(&ch, 1);
    // If the current file length is an even number, a
    // signature is added; otherwise it is already there.
    file.close();
}

void BTree::Insert(KeyType x)
{
    long pNew;
    KeyType xNew;
    Status code = ins(root_, x, xNew, pNew);
    if (code == DUPLICATE_KEY)
        cout << "Duplicate key ignored.\n";
    if (code == INSERT_NOT_COMPLETE)
    {
        long root0 = root_;
        root_ = GetNode();
        RootNode.n = 1;
        RootNode.k[0] = xNew;
        RootNode.p[0] = root0;
        RootNode.p[1] = pNew;
        WriteNode(root_, RootNode);
    }
}

void BTree::insert(const char* InpFileName)
{
    ifstream InpFile(InpFileName, ios::in);
    if (InpFile.fail())
    {
        cout << "Cannot open input file " << InpFileName
        << endl;
        return;
    }
    KeyType x;
    while (InpFile >> x) Insert(x);
    InpFile.clear();
    InpFile.close();
}

Status BTree::ins(long r, KeyType x, KeyType& y, long& q)
{  // Insert x in *this. If not completely successful, the
    // integer y and the pointer q remain to be inserted.
    // Return value:
    //    SUCCESS, DUPLICATE_KEY or INSERT_NOT_COMPLETE.
    long pNew, pFinal;
    int i, j, n;
    KeyType xNew, kFinal;
    Status code;
    if (r == NIL)
    {
        q = NIL;
        y = x;
        return INSERT_NOT_COMPLETE;
    }
    Node Node, NewNode;
    ReadNode(r, Node);
    n = Node.n;
    i = SearchInNode(x, Node.k, n);
    if (i < n && x == Node.k[i]) return DUPLICATE_KEY;
    code = ins(Node.p[i], x, xNew, pNew);
    if (code != INSERT_NOT_COMPLETE) return code;
    // Insertion in subtree did not completely succeed;
    // try to Insert xNew and pNew in the current Node:
    if (n < M - 1)
    {
        i = SearchInNode(xNew, Node.k, n);
        for (j = n; j > i; j--)
        {
            Node.k[j] = Node.k[j - 1];
            Node.p[j + 1] = Node.p[j];
        }
        Node.k[i] = xNew;
        Node.p[i + 1] = pNew;
        ++Node.n;
        WriteNode(r, Node);
        return SUCCESS;
    }
    // Current Node is full (n == M - 1) and will be split.
    // Pass item k[h] in the middle of the augmented
    // sequence back via parameter y, so that it
    // can move upward in the tree. Also, pass a pointer
    // to the newly created Node back via parameter q:
    if (i == M - 1)
    {
        kFinal = xNew;
        pFinal = pNew;
    } else
    {
        kFinal = Node.k[M - 2];
        pFinal = Node.p[M - 1];
        for (j = M - 2; j > i; j--)
        {
            Node.k[j] = Node.k[j - 1];
            Node.p[j + 1] = Node.p[j];
        }
        Node.k[i] = xNew;
        Node.p[i + 1] = pNew;
    }
    int h = (M - 1) / 2;
    y = Node.k[h];           // y and q are passed on to the
    q = GetNode();           // next higher level in the tree
    // The values p[0],k[0],p[1],...,k[h-1],p[h] belong to
    // the left of k[h] and are kept in *r:
    Node.n = h;
    // p[h+1],k[h+1],p[h+2],...,k[M-2],p[M-1],kFinal,pFinal
    // belong to the right of k[h] and are moved to *q:
    NewNode.n = M - 1 - h;
    for (j = 0; j < NewNode.n; j++)
    {
        NewNode.p[j] = Node.p[j + h + 1];
        NewNode.k[j] =
                (j < NewNode.n - 1 ? Node.k[j + h + 1] : kFinal);
    }
    NewNode.p[NewNode.n] = pFinal;
    WriteNode(r, Node);
    WriteNode(q, NewNode);
    return INSERT_NOT_COMPLETE;
}

void BTree::pr(long r, int nSpace)
{
    if (r != NIL)
    {
        int i;
        cout << setw(nSpace) << "";
        Node Node;
        ReadNode(r, Node);
        for (i = 0; i < Node.n; i++) cout << Node.k[i] << " ";
        cout << endl;
        for (i = 0; i <= Node.n; i++) pr(Node.p[i], nSpace + 8);
    }
}

int BTree::SearchInNode(KeyType x, const KeyType* a, int n) const
{
    int middle, left = 0, right = n - 1;
    if (x <= a[left]) return 0;
    if (x > a[right]) return n;
    while (right - left > 1)
    {
        middle = (right + left) / 2;
        (x <= a[middle] ? right : left) = middle;
    }
    return right;
}

void BTree::ShowSearch(KeyType x)
{
    cout << "Search path:\n";
    int i, j, n;
    long r = root_;
    Node Node;
    while (r != NIL)
    {
        ReadNode(r, Node);
        n = Node.n;
        for (j = 0; j < Node.n; j++) cout << " " << Node.k[j];
        cout << endl;
        i = SearchInNode(x, Node.k, n);
        if (i < n && x == Node.k[i])
        {
            cout << "Key " << x << " found in position " << i
            << " of last displayed Node.\n";
            return;
        }
        r = Node.p[i];
    }
    cout << "Key " << x << " not found.\n";
}

void BTree::Delete(KeyType x)
{
    long root0;
    switch (del(root_, x))
    {
        case NOT_FOUND:
            cout << x << " not found.\n";
            break;
        case UNDERFLOW:
            root0 = root_;
            root_ = RootNode.p[0];
            FreeNode(root0);
            if (root_ != NIL) ReadNode(root_, RootNode);
            break;
    }
}

Status BTree::del(long r, KeyType x)
{
    if (r == NIL) return NOT_FOUND;
    Node Node;
    ReadNode(r, Node);
    int i, j, pivot, n = Node.n;
    KeyType* k = Node.k;  // k[i] means Node.k[i]
    const int nMin = (M - 1) / 2;
    Status code;
    long* p = Node.p, pL, pR;       // p[i] means Node.p[i]
    i = SearchInNode(x, k, n);
    if (p[0] == NIL)  // Are we dealing with a leaf?
    {
        if (i == n || x < k[i]) return NOT_FOUND;
        // x == k[i]
        for (j = i + 1; j < n; j++)
        {
            k[j - 1] = k[j];
            p[j] = p[j + 1];
        }
        Node.n--;
        WriteNode(r, Node);
        return Node.n >= (r == root_ ? 1 : nMin) ?
               SUCCESS : UNDERFLOW;
    }
    // *r is an interior Node, not a leaf:
    if (i < n && x == k[i])
    {  // x found in an interior Node. Go to left child
        // and follow a path all the way to a leaf,
        // using rightmost branches:
        long q = p[i], q1;
        int nq;
        Node Node1;
        for (; ;)
        {
            ReadNode(q, Node1);
            nq = Node1.n;
            q1 = Node1.p[nq];
            if (q1 == NIL) break;
            q = q1;
        }
        // Exchange k[i] (= x) with rightmost item in leaf:
        k[i] = Node1.k[nq - 1];
        Node1.k[nq - 1] = x;
        WriteNode(r, Node);
        WriteNode(q, Node1);
    }
    // Delete x in leaf of subtree with root_ p[i]:
    code = del(p[i], x);
    if (code != UNDERFLOW) return code;
    // There is underflow; borrow, and, if necessary, merge:
    // Too few data items in Node *p[i]
    Node NodeL, NodeR;
    if (i > 0)
    {
        pivot = i - 1;
        pL = p[pivot];
        ReadNode(pL, NodeL);
        if (NodeL.n > nMin) // Borrow from left sibling
        {  // k[pivot] between pL and pR:
            pR = p[i];
            // Increase contents of *pR, borrowing from *pL:
            ReadNode(pR, NodeR);
            NodeR.p[NodeR.n + 1] = NodeR.p[NodeR.n];
            for (j = NodeR.n; j > 0; j--)
            {
                NodeR.k[j] = NodeR.k[j - 1];
                NodeR.p[j] = NodeR.p[j - 1];
            }
            NodeR.n++;
            NodeR.k[0] = k[pivot];
            NodeR.p[0] = NodeL.p[NodeL.n];
            k[pivot] = NodeL.k[--NodeL.n];
            WriteNode(pL, NodeL);
            WriteNode(pR, NodeR);
            WriteNode(r, Node);
            return SUCCESS;
        }
    }
    pivot = i;
    if (i < n)
    {
        pR = p[pivot + 1];
        ReadNode(pR, NodeR);
        if (NodeR.n > nMin) // Borrow from right sibling
        {  // k[pivot] between pL and pR:
            pL = p[pivot];
            ReadNode(pL, NodeL);
            // Increase contents of *pL, borrowing from *pR:
            NodeL.k[NodeL.n] = k[pivot];
            NodeL.p[NodeL.n + 1] = NodeR.p[0];
            k[pivot] = NodeR.k[0];
            NodeL.n++;
            NodeR.n--;
            for (j = 0; j < NodeR.n; j++)
            {
                NodeR.k[j] = NodeR.k[j + 1];
                NodeR.p[j] = NodeR.p[j + 1];
            }
            NodeR.p[NodeR.n] = NodeR.p[NodeR.n + 1];
            WriteNode(pL, NodeL);
            WriteNode(pR, NodeR);
            WriteNode(r, Node);
            return SUCCESS;
        }
    }
    // Merge; neither borrow left nor borrow right possible.
    pivot = (i == n ? i - 1 : i);
    pL = p[pivot];
    pR = p[pivot + 1];
    // Add k[pivot] and *pR to *pL:
    ReadNode(pL, NodeL);
    ReadNode(pR, NodeR);
    NodeL.k[NodeL.n] = k[pivot];
    NodeL.p[NodeL.n + 1] = NodeR.p[0];
    for (j = 0; j < NodeR.n; j++)
    {
        NodeL.k[NodeL.n + 1 + j] = NodeR.k[j];
        NodeL.p[NodeL.n + 2 + j] = NodeR.p[j + 1];
    }
    NodeL.n += 1 + NodeR.n;
    FreeNode(pR);
    for (j = i + 1; j < n; j++)
    {
        k[j - 1] = k[j];
        p[j] = p[j + 1];
    }
    Node.n--;
    WriteNode(pL, NodeL);
    WriteNode(r, Node);
    return
            Node.n >= (r == root_ ? 1 : nMin) ? SUCCESS : UNDERFLOW;
}

void BTree::ReadNode(long r, Node& Node)
{
    if (r == NIL) return;
    if (r == root_ && RootNode.n > 0) Node = RootNode; else
    {
        file.seekg(r, ios::beg);
        file.read((char*) &Node, sizeof(Node));
    }
}

void BTree::WriteNode(long r, const Node& Node)
{
    if (r == root_) RootNode = Node;
    file.seekp(r, ios::beg);
    file.write((char*) &Node, sizeof(Node));
}

void BTree::ReadStart()
{
    long start[2];
    file.seekg(0L, ios::beg);
    file.read((char*) start, 2 * sizeof(long));
    root_ = start[0];
    FreeList = start[1];
    ReadNode(root_, RootNode);
}

long BTree::GetNode()  // Modified (see also the destructor BTree)
{
    long r;
    Node Node;
    if (FreeList == NIL)
    {
        file.seekp(0L, ios::end); // Allocate space on disk; if
        r = file.tellp() & ~1;    // file length is an odd number,
        WriteNode(r, Node);       // the new Node will overwrite
    } else                      // signature byte at end of file
    {
        r = FreeList;
        ReadNode(r, Node);        // To update FreeList:
        FreeList = Node.p[0];     // Reduce the free list by 1
    }
    return r;
}


void BTree::FreeNode(long r)
{
    Node Node;
    ReadNode(r, Node);
    Node.p[0] = FreeList;
    FreeList = r;
    WriteNode(r, Node);
}

int main()
{
    cout <<
    "Demonstration program for a B-tree on disk. The\n"
            "structure of the B-tree is shown by indentation.\n"
            "For each Node, the number of links to other nodes\n"
            "will not be greater than " << M <<
    ", the order M of the B-tree.\n" <<
    "The B-tree representation is similar to the\n"
            "table of contents of a book. The items stored in\n"
            "each Node are displayed on a single line.\n\n";
    char TreeFileName[50];
    cout <<
    "Enter name of (possibly nonexistent) BINARY file for\n"
            "the B-tree: ";
    cin >> setw(50) >> TreeFileName;
    BTree t(TreeFileName);
    cout <<
    "\nEnter a (possibly empty) sequence of integers,\n"
            "followed by a slash (/):\n";
    KeyType x;
    char ch = 0;
    while (cin >> x, !cin.fail())
    {
        t.Insert(x);
        ch = 1;
    }
    if (ch) t.print();
    cin.clear();
    cin >> ch; // Skip terminating character
    cout << "\nDo you want data to be read from a text"
            "file? (Y/N): ";
    cin >> ch;
    if (toupper(ch) == 'Y')
    {
        char InpFileName[50];
        cout << "Name of this textfile: ";
        cin >> setw(50) >> InpFileName;
        t.insert(InpFileName);
        t.print();
    }
    for (; ;)
    {
        cout <<
        "\nEnter an integer, followed by I, D, or S (for\n"
                "Insert, Delete and Search), or enter Q to quit: ";
        cin >> x >> ch;
        if (cin.fail()) break;
        ch = toupper(ch);
        switch (ch)
        {
            case 'S':
                t.ShowSearch(x);
                break;
            case 'I':
                t.Insert(x);
                break;
            case 'D':
                t.Delete(x);
                break;
            default:
                cout << "Invalid command, use S, I or D\n";
                break;
        }
        if (ch == 'I' || ch == 'D') t.print();
    }
    return 0;
}

