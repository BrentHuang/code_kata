// Copyright(c) 1996 Leendert Ammeraal. All rights reserved.
// This program text occurs in Chapter 7 of
//
//    Ammeraal, L. (1996) Algorithms and Data Structures in C++,
//       Chichester: John Wiley.

// btree: B-tree of order M
//  (with nodes that contain at most M links)
#include <iostream>
#include <iomanip>

using namespace std;

#define M 5  // Order of B-tree: M link fields in each node

typedef int dtype; // 这个就是key type

enum status
{
    InsertNotComplete,
    Success,
    DuplicateKey,
    Underflow,
    NotFound
};

struct node
{
    int n;        // Number of items stored in a node (n < M)
    dtype k[M - 1]; // Data items (only the first n in use)  这里忽略了data字段，B-树的结点中应该有data字段
    node* p[M];   // Pointers to other nodes (n+1 in use)
};

// Logical order:
//    p[0], k[0], p[1], k[1], ..., p[n-1], k[n-1], p[n]

class Btree
{
public:
    Btree() : root(NULL)
    { }

    void insert(dtype x);

    void print() const
    {
        cout << "Contents:\n";
        pr(root, 0);
    }

    void DelNode(dtype x);
    void ShowSearch(dtype x) const;
private:
    node* root;
    status ins(node* r, dtype x, dtype& y, node*& u);
    void pr(const node* r, int nSpace) const;
    int NodeSearch(dtype x, const dtype* a, int n) const;
    status del(node* r, dtype x);
};

void Btree::insert(dtype x)
{
    node* pNew;
    dtype xNew;
    status code = ins(root, x, xNew, pNew);
    if (code == DuplicateKey)
        cout << "Duplicate key ignored.\n";
    if (code == InsertNotComplete)
    {
        node* root0 = root;
        root = new node;
        root->n = 1;
        root->k[0] = xNew;
        root->p[0] = root0;
        root->p[1] = pNew;
    }
}

status Btree::ins(node* r, dtype x, dtype& y, node*& q)
{  // Insert x in *this. If not completely successful, the
    // integer y and the pointer q remain to be inserted.
    // Return value:
    //    Success, DuplicateKey or InsertNotComplete.
    node* pNew, * pFinal;
    int i, j, n;
    dtype xNew, kFinal;
    status code;
    if (r == NULL)
    {
        q = NULL;
        y = x;
        return InsertNotComplete;
    }
    n = r->n;
    i = NodeSearch(x, r->k, n);
    if (i < n && x == r->k[i]) return DuplicateKey;
    code = ins(r->p[i], x, xNew, pNew);
    if (code != InsertNotComplete) return code;
    // Insertion in subtree did not completely succeed;
    // try to insert xNew and pNew in the current node:
    if (n < M - 1)
    {
        i = NodeSearch(xNew, r->k, n);
        for (j = n; j > i; j--)
        {
            r->k[j] = r->k[j - 1];
            r->p[j + 1] = r->p[j];
        }
        r->k[i] = xNew;
        r->p[i + 1] = pNew;
        ++r->n;
        return Success;
    }
    // Current node is full (n == M - 1) and will be split.
    // Pass item k[h] in the middle of the augmented
    // sequence back via parameter y, so that it
    // can move upward in the tree. Also, pass a pointer
    // to the newly created node back via parameter q:
    if (i == M - 1)
    {
        kFinal = xNew;
        pFinal = pNew;
    } else
    {
        kFinal = r->k[M - 2];
        pFinal = r->p[M - 1];
        for (j = M - 2; j > i; j--)
        {
            r->k[j] = r->k[j - 1];
            r->p[j + 1] = r->p[j];
        }
        r->k[i] = xNew;
        r->p[i + 1] = pNew;
    }
    int h = (M - 1) / 2;
    y = r->k[h];             // y and q are passed on to the
    q = new node;            // next higher level in the tree
    // The values p[0],k[0],p[1],...,k[h-1],p[h] belong to
    // the left of k[h] and are kept in *r:
    r->n = h;
    // p[h+1],k[h+1],p[h+2],...,k[M-2],p[M-1],kFinal,pFinal
    // belong to the right of k[h] and are moved to *q:
    q->n = M - 1 - h;
    for (j = 0; j < q->n; j++)
    {
        q->p[j] = r->p[j + h + 1];
        q->k[j] = (j < q->n - 1 ? r->k[j + h + 1] : kFinal);
    }
    q->p[q->n] = pFinal;
    return InsertNotComplete;
}

void Btree::pr(const node* r, int nSpace) const
{
    if (r)
    {
        int i;
        cout << setw(nSpace) << "";
        for (i = 0; i < r->n; i++)
            cout << setw(3) << r->k[i] << " ";
        cout << endl;
        for (i = 0; i <= r->n; i++) pr(r->p[i], nSpace + 8);
    }
}

int Btree::NodeSearch(dtype x, const dtype* a, int n) const
{
    int i = 0;
    while (i < n && x > a[i]) i++; // 可以优化为二分查找
    return i;
}

void Btree::ShowSearch(dtype x) const
{
    cout << "Search path:\n";
    int i, j, n;
    node* r = root;
    while (r)
    {
        n = r->n;
        for (j = 0; j < r->n; j++) cout << " " << r->k[j];
        cout << endl;
        i = NodeSearch(x, r->k, n); // i要么找到，要么在左子树中
        if (i < n && x == r->k[i])
        {
            cout << "Key " << x << " found in position " << i << " of last displayed node.\n";
            return;
        }
        r = r->p[i]; // 进入左子树
    }
    cout << "Key " << x << " not found.\n";
}

void Btree::DelNode(dtype x)
{
    node* root0;
    switch (del(root, x))
    {
        case NotFound:
            cout << x << " not found.\n";
            break;
        case Underflow:
            root0 = root;
            root = root->p[0];
            delete root0;
            break;
    }
}

status Btree::del(node* r, dtype x)
{
    if (r == NULL) return NotFound;
    int i, j, pivot, n = r->n;
    dtype* k = r->k;  // k[i] means r->k[i]
    const int nMin = (M - 1) / 2;
    status code;
    node** p = r->p, * pL, * pR;       // p[i] means r->p[i]
    i = NodeSearch(x, k, n);
    if (p[0] == NULL) // *r is a leaf
    {
        if (i == n || x < k[i]) return NotFound;
        // x == k[i], and *r is a leaf
        for (j = i + 1; j < n; j++)
        {
            k[j - 1] = k[j];
            p[j] = p[j + 1];
        }
        return
                --r->n >= (r == root ? 1 : nMin) ? Success : Underflow;
    }
    // *r is an interior node, not a leaf:
    if (i < n && x == k[i])
    {  // x found in an interior node. Go to left child
        // *p[i] and follow a path all the way to a leaf,
        // using rightmost branches:
        node* q = p[i], * q1;
        int nq;
        for (; ;)
        {
            nq = q->n;
            q1 = q->p[nq];
            if (q1 == NULL) break;
            q = q1;
        }
        // Exchange k[i] (= x) with rightmost item in leaf:
        k[i] = q->k[nq - 1];
        q->k[nq - 1] = x;
    }
    // Delete x in leaf of subtree with root p[i]:
    code = del(p[i], x);
    if (code != Underflow) return code;
    // There is underflow; borrow, and, if necessary, merge:
    // Too few data items in node *p[i]
    if (i > 0 && p[i - 1]->n > nMin) // Borrow from left sibling
    {
        pivot = i - 1; // k[pivot] between pL and pR:
        pL = p[pivot];
        pR = p[i];
        // Increase contents of *pR, borrowing from *pL:
        pR->p[pR->n + 1] = pR->p[pR->n];
        for (j = pR->n; j > 0; j--)
        {
            pR->k[j] = pR->k[j - 1];
            pR->p[j] = pR->p[j - 1];
        }
        pR->n++;
        pR->k[0] = k[pivot];
        pR->p[0] = pL->p[pL->n];
        k[pivot] = pL->k[--pL->n];
        return Success;
    }
    if (i < n && p[i + 1]->n > nMin) // Borrow from right sibling
    {
        pivot = i; // k[pivot] between pL and pR:
        pL = p[pivot];
        pR = p[pivot + 1];
        // Increase contents of *pL, borrowing from *pR:
        pL->k[pL->n] = k[pivot];
        pL->p[pL->n + 1] = pR->p[0];
        k[pivot] = pR->k[0];
        pL->n++;
        pR->n--;
        for (j = 0; j < pR->n; j++)
        {
            pR->k[j] = pR->k[j + 1];
            pR->p[j] = pR->p[j + 1];
        }
        pR->p[pR->n] = pR->p[pR->n + 1];
        return Success;
    }
    // Merge; neither borrow left nor borrow right possible.
    pivot = (i == n ? i - 1 : i);
    pL = p[pivot];
    pR = p[pivot + 1];
    // Add k[pivot] and *pR to *pL:
    pL->k[pL->n] = k[pivot];
    pL->p[pL->n + 1] = pR->p[0];
    for (j = 0; j < pR->n; j++)
    {
        pL->k[pL->n + 1 + j] = pR->k[j];
        pL->p[pL->n + 2 + j] = pR->p[j + 1];
    }
    pL->n += 1 + pR->n;
    delete pR;
    for (j = i + 1; j < n; j++)
    {
        k[j - 1] = k[j];
        p[j] = p[j + 1];
    }
    return
            --r->n >= (r == root ? 1 : nMin) ? Success : Underflow;
}

int main()
{
    cout <<
    "B-tree structure shown by indentation. For each\n"
            "node, the number of links to other nodes will not\n"
            "be greater than " << M <<
    ", the order M of the B-tree.\n\n"
            "Enter some integers, followed by a slash (/):\n";
    Btree t;
    dtype x;
    char ch;
    while (cin >> x, !cin.fail()) t.insert(x);
    cout <<
    "\nB-tree representation (indentation similar to the\n"
            "table of contents of a book). The items stored in\n"
            "each node are displayed on a single line.\n";
    t.print();
    cin.clear();
    cin >> ch; // Skip terminating character
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
                t.insert(x);
                break;
            case 'D':
                t.DelNode(x);
                break;
            default:
                cout << "Invalid command, use S, I or D\n";
                break;
        }
        if (ch == 'I' || ch == 'D') t.print();
    }
    return 0;
}

