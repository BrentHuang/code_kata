// Copyright(c) 1996 Leendert Ammeraal. All rights reserved.
// This program text occurs in Chapter 7 of
//
//    Ammeraal, L. (1996) Algorithms and Data Structures in C++,
//       Chichester: John Wiley.

/* disktree:
   Demonstration program for a B-tree on disk. After
   building the B-tree by entering integers on the
   keyboard or by supplying them as a text fs_, we can
   Insert and delete items via the keyboard. We can also
   search the B-tree for a given item. Each time, the tree
   or a search path is displayed. In contrast to program
   btree, program disktree writes, reads and updates nodes
   on disk, using a binary fs_. The name of this fs_ is
   to be entered on the keyboard. If a B-tree with that
   name exists, that B-tree is used; otherwise such a fs_
   is created.
   Caution:
      Do not confuse the (binary) fs_ for the B-tree with
      the optional textfile for input data. Use different
      fs_-name extensions, such as .bin and .txt.
*/
// 将Ｂ-树按node存储在一个二进制文件中，用hexdump -C tree.bin分析
// 最终这个二进制文件的长度为奇数，如果为偶数则在结尾写一个字节（内容为sizeof(int)）
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

const int M = 1000;  // Order of B-tree: M link fields in each Node

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
    KeyType k[M - 1]; // Data items (only the first n in use) k[0]~k[n-1]有效
    long p[M];    // 'Pointers' to other nodes (n+1 in use)　p[0]~p[n]有效
};

// Logical order:
//    p[0], k[0], p[1], k[1], ..., p[n-1], k[n-1], p[n]

class DiskBTree
{
public:
    DiskBTree(const char* tree_file_path);
    ~DiskBTree();

    void Print()
    {
        cout << "Contents:" << endl;
        PrintNode(root_, 0);
    }

    void ShowSearch(KeyType x);

    int Insert(KeyType x);
    int Insert(const char* key_file_path);

    int Delete(KeyType x);

private:
    void PrintNode(long r, int indent_space_count);
    int SearchInNode(KeyType x, const KeyType* k, int n) const;
    Status Ins(long r, KeyType x, KeyType& y, long& u);
    Status Del(long r, KeyType x);
    void ReadNode(long r, Node& node);
    void WriteNode(long r, const Node& node);
    long GetNode();
    void FreeNode(long r);
    void ReadStart();

private:
    enum
    {
        NIL = -1
    };

    long root_, free_list_;
    Node root_node_;
    fstream fs_;
};

DiskBTree::DiskBTree(const char* tree_file_path)
{
    ifstream ifs(tree_file_path, ios::in); // Remove  "| ios::nocreate" if your compiler does not accept it.
    bool new_file = ifs.fail();
    ifs.clear();
    ifs.close();

    if (new_file)
    {
        // 文件不存在，新建一个
        fs_.open(tree_file_path, ios::out | ios::in | ios::trunc | ios::binary);
        // ios::binary required with MSDOS, but possibly
        // not accepted with other environments.
        root_ = free_list_ = NIL;
        long start[2] = { NIL, NIL };
        fs_.write((char*) start, 2 * sizeof(long));
    }
    else
    {
        long start[2];
        fs_.open(tree_file_path, ios::out | ios::in | ios::binary); // See above note.
        fs_.seekg(-1L, ios::end);
        char ch;
        fs_.read(&ch, 1); // Read signature.
        fs_.seekg(0L, ios::beg);
        fs_.read((char*) start, 2 * sizeof(long));
        if (ch != sizeof(int))
        {
            cout << "Wrong file format." << endl;
            exit(1);
        }

        root_ = start[0];
        free_list_ = start[1];
        root_node_.n = 0;   // Signal for function ReadNode
        ReadNode(root_, root_node_);
        Print();
    }
}

DiskBTree::~DiskBTree()
{
    long start[2];
    fs_.seekp(0L, ios::beg);
    start[0] = root_;
    start[1] = free_list_;
    fs_.write((char*) start, 2 * sizeof(long));

    // The remaining code of this destructor is slightly
    // different from that in the first print of the book.
    // The length of the final binary file, including the
    // signature byte at the end, will now always be an odd
    // number（奇数）, as it should be. There is a similar change in
    // the function GetNode. I am grateful to Chian Wiz from
    // Singapore, who showed me the possibility of a 'file leak',
    // that is, an unused byte, which sometimes caused problems
    // with the program 'showfile', when this was applied to
    // this binary file. Such problems should no longer occur.
    // L. A.
    char ch = sizeof(int); // Signature

    fs_.seekg(0L, ios::end);
    if ((fs_.tellg() & 1) == 0)
    {
        fs_.write(&ch, 1);
    }

    // If the current file length is an even number（偶数）, a
    // signature is added; otherwise it is already there.
    fs_.close();
}

void DiskBTree::ShowSearch(KeyType x)
{
    cout << "Search path:" << endl;
    int i, j, n;
    long r = root_;
    Node node;

    while (r != NIL)
    {
        ReadNode(r, node);
        n = node.n;

        for (j = 0; j < node.n; ++j)
        {
            cout << " " << node.k[j];
        }

        cout << endl;

        i = SearchInNode(x, node.k, n);
        if (i < n && x == node.k[i])
        {
            cout << "Key " << x << " found in position " << i << " of last displayed node.";
            return;
        }

        r = node.p[i];
    }

    cout << "Key " << x << " not found." << endl;
}

int DiskBTree::Insert(KeyType x)
{
    int ret = 0;
    KeyType y;
    long q = NIL;

    Status code = Ins(root_, x, y, q);
    switch (code)
    {
        case DUPLICATE_KEY:
        {
            cout << "Duplicate key ignored." << endl;
            ret = -1;
        }
            break;

        case INSERT_NOT_COMPLETE:
        {
            long root = root_;
            root_ = GetNode();
            root_node_.n = 1;
            root_node_.k[0] = y;
            root_node_.p[0] = root;
            root_node_.p[1] = q;
            WriteNode(root_, root_node_);
        }
            break;

        default:
        {
            ret = -1;
        }
            break;
    }

    return ret;
}

int DiskBTree::Insert(const char* key_file_path)
{
    ifstream ifs(key_file_path, ios::in);
    if (ifs.fail())
    {
        cout << "Cannot open input file " << key_file_path << endl;
        return -1;
    }

    KeyType x;

    while (ifs >> x)
    {
        Insert(x);
    }

    ifs.clear();
    ifs.close();

    return 0;
}

int DiskBTree::Delete(KeyType x)
{
    int ret = 0;

    Status code = Del(root_, x);
    switch (code)
    {
        case NOT_FOUND:
        {
            cout << "Key " << x << " not found." << endl;
            ret = -1;
        }
            break;

        case UNDERFLOW:
        {
            long root = root_;
            root_ = root_node_.p[0];
            FreeNode(root);

            if (root_ != NIL)
            {
                ReadNode(root_, root_node_);
            }
        }
            break;
    }

    return ret;
}

void DiskBTree::PrintNode(long r, int indent_space_count)
{
    if (r != NIL)
    {
        int i;
        cout << setw(indent_space_count) << "";

        Node Node;
        ReadNode(r, Node);

        for (i = 0; i < Node.n; ++i)
        {
            cout << Node.k[i] << " ";
        }

        cout << endl;

        for (i = 0; i <= Node.n; ++i)
        {
            PrintNode(Node.p[i], indent_space_count + 8);
        }
    }
}

int DiskBTree::SearchInNode(KeyType x, const KeyType* k, int n) const
{
    int middle, left = 0, right = n - 1;
    if (x <= k[left])
    {
        return 0;
    }

    if (x > k[right])
    {
        return n;
    }

    // 二分查找
    while (right - left > 1)
    {
        middle = (right + left) / 2;
        (x <= k[middle] ? right : left) = middle;
    }

    return right;
}

Status DiskBTree::Ins(long r, KeyType x, KeyType& y, long& q)
{  // Insert x in *this. If not completely successful, the
    // integer y and the pointer q remain to be inserted.
    // Return value:
    //    SUCCESS, DUPLICATE_KEY or INSERT_NOT_COMPLETE.
    if (NIL == r)
    {
        y = x;
        q = NIL;
        return INSERT_NOT_COMPLETE;
    }

    KeyType y1;
    long q1;

    KeyType final_x;
    long final_q;

    int i, j, n;
    Status code;

    Node node, new_node;

    ReadNode(r, node);
    n = node.n;

    i = SearchInNode(x, node.k, n);
    if (i < n && x == node.k[i])
    {
        return DUPLICATE_KEY;
    }

    code = Ins(node.p[i], x, y1, q1);
    if (code != INSERT_NOT_COMPLETE)
    {
        return code;
    }

    // Insertion in subtree did not completely succeed;
    // try to Insert y1 and q1 in the current node:
    if (n < M - 1)
    {
        i = SearchInNode(y1, node.k, n);

        for (j = n; j > i; --j)
        {
            node.k[j] = node.k[j - 1];
            node.p[j + 1] = node.p[j];
        }

        node.k[i] = y1;
        node.p[i + 1] = q1;
        ++(node.n);
        WriteNode(r, node);

        return SUCCESS;
    }

    // Current node is full (n == M - 1) and will be split.
    // Pass item k[h] in the middle of the augmented
    // sequence back via parameter y, so that it
    // can move upward in the tree. Also, pass a pointer
    // to the newly created node back via parameter q:
    if (i == M - 1)
    {
        final_x = y1;
        final_q = q1;
    }
    else
    {
        final_x = node.k[M - 2];
        final_q = node.p[M - 1];

        for (j = M - 2; j > i; --j)
        {
            node.k[j] = node.k[j - 1];
            node.p[j + 1] = node.p[j];
        }

        node.k[i] = y1;
        node.p[i + 1] = q1;
    }

    int h = (M - 1) / 2;
    y = node.k[h];           // y and q are passed on to the
    q = GetNode();           // next higher level in the tree

    // The values p[0],k[0],p[1],...,k[h-1],p[h] belong to
    // the left of k[h] and are kept in *r:
    node.n = h;

    // p[h+1],k[h+1],p[h+2],...,k[M-2],p[M-1],final_x,final_q
    // belong to the right of k[h] and are moved to *q:
    new_node.n = M - 1 - h;

    for (j = 0; j < new_node.n; ++j)
    {
        new_node.p[j] = node.p[j + h + 1];
        new_node.k[j] = ((j < new_node.n - 1) ? node.k[j + h + 1] : final_x);
    }

    new_node.p[new_node.n] = final_q;
    WriteNode(r, node);
    WriteNode(q, new_node);

    return INSERT_NOT_COMPLETE;
}

Status DiskBTree::Del(long r, KeyType x)
{
    if (NIL == r)
    {
        return NOT_FOUND;
    }

    Node node;
    ReadNode(r, node);

    KeyType* k = node.k;  // k[i] means node.k[i]
    long* p = node.p;
    long pL = NIL;
    long pR = NIL;       // p[i] means node.p[i]
    int i, j, pivot, n = node.n;
    const int N_MIN = (M - 1) / 2;
    Status code;

    i = SearchInNode(x, k, n);
    if (NIL == p[0])  // Are we dealing with a leaf?
    {
        if (i == n || x < k[i])
        {
            return NOT_FOUND;
        }

        // x == k[i]
        for (j = i + 1; j < n; ++j)
        {
            k[j - 1] = k[j];
            p[j] = p[j + 1];
        }

        --node.n;
        WriteNode(r, node);

        return (node.n >= (r == root_ ? 1 : N_MIN)) ? SUCCESS : UNDERFLOW;
    }

    // *r is an interior node, not a leaf:
    if (i < n && x == k[i])
    {  // x found in an interior node. Go to left child
        // and follow a path all the way to a leaf,
        // using rightmost branches:
        long q = p[i], q1;
        int nq;
        Node node1;

        for (; ;)
        {
            ReadNode(q, node1);

            nq = node1.n;
            q1 = node1.p[nq];

            if (NIL == q1)
            {
                break;
            }

            q = q1;
        }

        // Exchange k[i] (= x) with rightmost item in leaf:
        k[i] = node1.k[nq - 1];
        node1.k[nq - 1] = x;
        WriteNode(r, node);
        WriteNode(q, node1);
    }

    // Delete x in leaf of subtree with root_ p[i]:
    code = Del(p[i], x);
    if (code != UNDERFLOW)
    {
        return code;
    }

    // There is underflow; borrow, and, if necessary, merge:
    // Too few data items in Node *p[i]
    Node nodeL, nodeR;
    if (i > 0)
    {
        pivot = i - 1;
        pL = p[pivot];
        ReadNode(pL, nodeL);

        if (nodeL.n > N_MIN) // Borrow from left sibling
        {  // k[pivot] between pL and pR:
            pR = p[i];
            // Increase contents of *pR, borrowing from *pL:
            ReadNode(pR, nodeR);
            nodeR.p[nodeR.n + 1] = nodeR.p[nodeR.n];

            for (j = nodeR.n; j > 0; --j)
            {
                nodeR.k[j] = nodeR.k[j - 1];
                nodeR.p[j] = nodeR.p[j - 1];
            }

            ++(nodeR.n);
            nodeR.k[0] = k[pivot];
            nodeR.p[0] = nodeL.p[nodeL.n];
            k[pivot] = nodeL.k[--nodeL.n];

            WriteNode(pL, nodeL);
            WriteNode(pR, nodeR);
            WriteNode(r, node);

            return SUCCESS;
        }
    }

    pivot = i;

    if (i < n)
    {
        pR = p[pivot + 1];
        ReadNode(pR, nodeR);

        if (nodeR.n > N_MIN) // Borrow from right sibling
        {  // k[pivot] between pL and pR:
            pL = p[pivot];
            ReadNode(pL, nodeL);

            // Increase contents of *pL, borrowing from *pR:
            nodeL.k[nodeL.n] = k[pivot];
            nodeL.p[nodeL.n + 1] = nodeR.p[0];
            k[pivot] = nodeR.k[0];
            ++(nodeL.n);
            --(nodeR.n);

            for (j = 0; j < nodeR.n; ++j)
            {
                nodeR.k[j] = nodeR.k[j + 1];
                nodeR.p[j] = nodeR.p[j + 1];
            }

            nodeR.p[nodeR.n] = nodeR.p[nodeR.n + 1];
            WriteNode(pL, nodeL);
            WriteNode(pR, nodeR);
            WriteNode(r, node);

            return SUCCESS;
        }
    }

    // Merge; neither borrow left nor borrow right possible.
    pivot = ((i == n) ? i - 1 : i);
    pL = p[pivot];
    pR = p[pivot + 1];

    // Add k[pivot] and *pR to *pL:
    ReadNode(pL, nodeL);
    ReadNode(pR, nodeR);
    nodeL.k[nodeL.n] = k[pivot];
    nodeL.p[nodeL.n + 1] = nodeR.p[0];

    for (j = 0; j < nodeR.n; ++j)
    {
        nodeL.k[nodeL.n + 1 + j] = nodeR.k[j];
        nodeL.p[nodeL.n + 2 + j] = nodeR.p[j + 1];
    }

    nodeL.n += 1 + nodeR.n;
    FreeNode(pR);

    for (j = i + 1; j < n; ++j)
    {
        k[j - 1] = k[j];
        p[j] = p[j + 1];
    }

    --(node.n);
    WriteNode(pL, nodeL);
    WriteNode(r, node);

    return (node.n >= (r == root_ ? 1 : N_MIN)) ? SUCCESS : UNDERFLOW;
}

void DiskBTree::ReadNode(long r, Node& node)
{
    if (NIL == r)
    {
        return;
    }

    if (r == root_ && root_node_.n > 0)
    {
        node = root_node_; // 根结点常驻内存
    }
    else
    {
        fs_.seekg(r, ios::beg); // 读文件时使用tellg()；写文件时使用tellp()。g代表get，p代表put
        fs_.read((char*) &node, sizeof(node));
    }
}

void DiskBTree::WriteNode(long r, const Node& node)
{
    if (r == root_)
    {
        root_node_ = node;
    }

    fs_.seekp(r, ios::beg);
    fs_.write((char*) &node, sizeof(node));
}

long DiskBTree::GetNode()  // Modified (see also the destructor DiskBTreeTree)
{
    long r;
    Node node;

    if (NIL == free_list_)
    {
        // 增加一个node
        fs_.seekp(0L, ios::end); // Allocate space on disk; if file length is an odd number, the new node will overwrite signature byte at end of file
        r = fs_.tellp() & ~1; // 如果文件长度为偶数，则r就是文件长度；如果为奇数，则r往前移一个字节
        WriteNode(r, node);
    }
    else
    {
        // 取free_list的第一个元素
        r = free_list_;
        ReadNode(r, node);        // To update free_list:
        free_list_ = node.p[0];     // Reduce the free list by 1
    }

    return r;
}

void DiskBTree::FreeNode(long r)
{
    Node Node;
    ReadNode(r, Node);
    Node.p[0] = free_list_;
    free_list_ = r;
    WriteNode(r, Node);
}

void DiskBTree::ReadStart()
{
    long start[2];
    fs_.seekg(0L, ios::beg);
    fs_.read((char*) start, 2 * sizeof(long));
    root_ = start[0];
    free_list_ = start[1];
    ReadNode(root_, root_node_);
}

int main()
{
    cout << "sizeof(node): " << sizeof(Node) << endl;

    cout << "Demonstration program for a B-tree on disk. The" << endl
        << "structure of the B-tree is shown by indentation." << endl
        << "For each node, the number of links to other nodes" << endl
        << "will not be greater than " << M << ", the order M of the B-tree." << endl
        << "The B-tree representation is similar to the" << endl
        << "table of contents of a book. The items stored in" << endl
        << "each Node are displayed on a single line." << endl << endl;

    char tree_file_path[50];
    cout << "Enter name of (possibly nonexistent) BINARY file path for" << endl
            << "the B-tree: ";
    cin >> setw(50) >> tree_file_path;

    DiskBTree tree(tree_file_path);

    cout << endl << "Enter a (possibly empty) sequence of integers," << endl << "followed by a slash (/):" << endl;
    KeyType x;
    char ch = 0;

    while (cin >> x, !cin.fail())
    {
        tree.Insert(x);
        ch = 1;
    }

    if (ch)
    {
        tree.Print();
    }

    cin.clear();
    cin >> ch; // Skip terminating character
    cout << endl << "Do you want data to be read from a text file? (Y/N): ";
    cin >> ch;

    if (toupper(ch) == 'Y')
    {
        char key_file_path[50];
        cout << "Name of this text file: ";
        cin >> setw(50) >> key_file_path;
        tree.Insert(key_file_path);
//        tree.Print();
    }

    for (; ;)
    {
        cout << endl << "Enter an integer, followed by I, D, or S (for" << endl
            << "Insert, Delete and Search), or enter Q to quit: ";
        cin >> x >> ch;
        if (cin.fail())
        {
            break;
        }

        ch = (char) toupper(ch);
        switch (ch)
        {
            case 'S':
                tree.ShowSearch(x);
                break;
            case 'I':
                tree.Insert(x);
                break;
            case 'D':
                tree.Delete(x);
                break;
            default:
                cout << "Invalid command, use S, I or D" << endl;
                break;
        }

        if (ch == 'I' || ch == 'D')
        {
            tree.Print();
        }
    }

    return 0;
}
