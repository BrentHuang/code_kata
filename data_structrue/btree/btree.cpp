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

typedef int KeyType;

enum Status
{
    INSERT_NOT_COMPLETE,
    SUCCESS,
    DUPLICATE_KEY,
    UNDERFLOW,
    NOT_FOUND,
};

struct Node
{
    int n;        // Number of items stored in a Node (n < M)
    KeyType k[M - 1]; // Data items (only the first n in use)  这里省略了data字段，B-树的结点中应该有data字段
    Node* p[M];   // Pointers to other nodes (n+1 in use)
};

// Logical order:
//    p[0], k[0], p[1], k[1], ..., p[n-1], k[n-1], p[n]

class BTree
{
public:
    BTree()
    {
        root_ = NULL;
    }

    ~BTree()
    {
        // TODO 释放内存
        if (root_ != NULL)
        {

        }
        
    }

    /**
     * @brief 从根结点开始打印整棵B-数中的所有关键字
     */
    void Print() const
    {
        cout << "Contents:" << endl;
        PrintNode(root_, 0);
    }

    /**
     * @brief 从根结点开始打印搜索过程
     * @param x 待查找的关键字
     * @details 从根结点开始，先打印结点内的所有关键字，然后换行；接着在结点内查找，如果找到则打印关键字的位置并结束；
     *          如果找不到则进入子树中查找，循环进行直到遇到叶结点为止，打印not found并结束
     */
    void ShowSearch(KeyType x) const;

    /**
     * @brief 向B-树中插入一个关键字
     * @param x 待插入的关键字
     * @details 如果关键字已经存在了则不插入，打印一句提示
     */
    void Insert(KeyType x);

    /**
     * @brief 从B-树中删除一个关键字
     * @param x 待删除的关键字
     * @details 如果关键字不存在则打印一句提示
     */
    void Delete(KeyType x);

private:
    /**
     * @brief 打印结点及其所有子树中的关键字
     * @param node 待打印的结点
     * @param indent_space_count 初始缩进值
     * @details 先打印结点内的关键字，然后换行，再缩进8个空格，依次打印各个子树中的关键字，递归进行
     */
    void PrintNode(const Node* node, int indent_space_count) const;

    /**
     * @brief 在一个结点内查找
     * @param x 待查找的关键字
     * @param k 结点内的关键字数组
     * @param n 结点内的关键字个数
     * @return i 要么在结点内找到，则i就是结点内的关键字位置（数组下标）；找不到则在p[i]子树中
     */
    int SearchInNode(KeyType x, const KeyType* k, int n) const;

    /**
     * @brief 将关键字插入到结点中
     * @param r 往其中插入的结点
     * @param x 待插入的关键字
     * @prarm y 往上提到父结点中的关键字
     * @param q 分裂新增的结点
     * @return SUCCESS/DUPLICATE_KEY/INSERT_NOT_COMPLETE 如果返回InsertNotComplete，则y和q需要被
     * @details B-树的插入规则：首先在最底层的某个分支结点（通过查找得到）中添加待插入的关键字，若插入前该结点内的关键字个数不超过m-1则插入完毕；
     *          否则要进行结点的分裂，将该结点分裂成2个结点，左边一个结点包含前ceil(m/2)-1个关键字，右边一个结点包含后m-ceil(m/2)个关键字，
     *          中间的那个关键字提到父结点中。所以分裂过程可能会一直波及到根结点
     */
    Status Ins(Node* r, KeyType x, KeyType& y, Node*& u);

    Status Del(Node* r, KeyType x);

private:
    Node* root_;
};

void BTree::ShowSearch(KeyType x) const
{
    cout << "Search path:" << endl;

    int i, j, n;
    Node* r = root_;

    while (r)
    {
        n = r->n;

        for (j = 0; j < r->n; ++j)
        {
            cout << " " << r->k[j];
        }

        cout << endl;

        i = SearchInNode(x, r->k, n); // 要么在结点内找到，要么在p[i]子树中

        if (i < n && x == r->k[i])
        {
            cout << "Key " << x << " found in position " << i << " of last displayed Node." << endl;
            return;
        }

        r = r->p[i]; // 进入p[i]子树
    }

    cout << "Key " << x << " not found." << endl;
}

void BTree::Insert(KeyType x)
{
    KeyType y;
    Node* q = NULL;

    Status code = Ins(root_, x, y, q);

    switch (code)
    {
        case DUPLICATE_KEY:
        {
            cout << "Duplicate key ignored." << endl;
        }
            break;

        case INSERT_NOT_COMPLETE:
        {
            Node* root = root_;
            root_ = new Node;
            root_->n = 1;
            root_->k[0] = y;
            root_->p[0] = root;
            root_->p[1] = q;
        }
            break;

        default:
            break;
    }
}

void BTree::Delete(KeyType x)
{
    Status code = Del(root_, x);

    switch (code)
    {
        case NOT_FOUND:
        {
            cout << x << " not found." << endl;
        }
            break;

        case UNDERFLOW:
        {
            Node* root = root_;
            root_ = root_->p[0];
            delete root;
        }
            break;

        default:
            break;
    }
}

void BTree::PrintNode(const Node* node, int indent_space_count) const
{
    if (NULL == node)
    {
        return;
    }

    cout << setw(indent_space_count) << "";

    for (int i = 0; i < node->n; ++i)
    {
        cout << setw(3) << node->k[i] << " ";
    }

    cout << endl;

    for (int i = 0; i <= node->n; ++i)
    {
        PrintNode(node->p[i], indent_space_count + 8);
    }
}

int BTree::SearchInNode(KeyType x, const KeyType* k, int n) const
{
    int i = 0;

    while (i < n && x > k[i])
    {
        ++i; // TODO 可以优化为二分查找
    }

    return i;
}

Status BTree::Ins(Node* r, KeyType x, KeyType& y, Node*& q)
{
    if (NULL == r)
    {
        q = NULL;
        y = x;
        return INSERT_NOT_COMPLETE;
    }

    KeyType y1;
    Node* q1 = NULL;

    // 结点中的最后一个关键字和子树指针
    KeyType final_x;
    Node* final_p = NULL;

    int i, j, n;
    Status code;

    n = r->n;
    i = SearchInNode(x, r->k, n);

    if (i < n && x == r->k[i])
    {
        return DUPLICATE_KEY;
    }

    code = Ins(r->p[i], x, y1, q1);
    if (code != INSERT_NOT_COMPLETE)
    {
        return code;
    }

    // Insertion in subtree did not completely succeed;
    // try to Insert x1 and q1 in the current Node:
    if (n < M - 1)
    {
        // 插入到结点内即可
        i = SearchInNode(y1, r->k, n);

        for (j = n; j > i; --j)
        {
            // 往后移保持关键字数组有序
            r->k[j] = r->k[j - 1];
            r->p[j + 1] = r->p[j];
        }

        r->k[i] = y1;
        r->p[i + 1] = q1;
        ++(r->n);

        return SUCCESS;
    }

    // Current Node is full (n == M - 1) and will be split.
    // Pass item k[h] in the middle of the augmented
    // sequence back via parameter y, so that it
    // can move upward in the tree. Also, pass a pointer
    // to the newly created Node back via parameter q:
    // 此时结点中的关键字个数已经为M-1了，关键字数组已经满了
    if (i == M - 1)
    {
        // 待插入的关键字比结点中所有的关键字都大，则final_x为待插入的关键字，final_p为空
        final_x = y1;
        final_p = q1;
    }
    else
    {
        // 在中间位置，final记录结点中的最后一个关键字和子树指针
        final_x = r->k[M - 2];
        final_p = r->p[M - 1];

        for (j = M - 2; j > i; --j)
        {
            r->k[j] = r->k[j - 1];
            r->p[j + 1] = r->p[j];
        }

        r->k[i] = y1;
        r->p[i + 1] = q1;
    }

    const int h = (M - 1) / 2; // 往上提的那个关键字的位置（数组下标）
    y = r->k[h];             // y and q are passed on to the next higher level in the tree

    q = new Node;            // 分裂产生的新结点

    // The values p[0],k[0],p[1],...,k[h-1],p[h] belong to
    // the left of k[h] and are kept in *r:
    r->n = h; // 更新原有结点中的关键字个数
    // p[h+1],k[h+1],p[h+2],...,k[M-2],p[M-1],final_x,final_p
    // belong to the right of k[h] and are moved to *q:
    q->n = M - 1 - h; // 新结点中的关键字个数

    for (j = 0; j < q->n; ++j)
    {
        const int idx = j + h + 1;
        q->k[j] = (j < q->n - 1 ? r->k[idx] : final_x);
        q->p[j] = r->p[idx];
    }

    q->p[q->n] = final_p;

    return INSERT_NOT_COMPLETE;
}

/**
 * @details 如果要删除的关键字不在最下面一层的分支结点中，设待删关键字为该结点中第i个关键字key[i]，则可从p[i]所指的子树中找出最小关键字Y，代替key[i]的位置，然后在子树中删去Y。
 *          如果要删除的关键字在最下面一层的分支结点中，
 *          1，如果待删关键字所在结点的关键字个数大于ceil(m/2)-1，说明删去该关键字后此结点仍满足B-树的定义，此时从该结点中直接删去该关键字即可。
 *          2，如果待删关键字所在结点的关键字个数等于ceil(m/2)-1，删去该关键字后需要进行结点的合并：
 *             （1）如果与该结点相邻的左（右）兄弟结点中的关键字大于ceil(m/2)-1个，则可将左（右）兄弟结点中最大（小）的关键字上移至双亲结点，
 *                 而将双亲结点中大（小）于该上移关键字的关键字下移至被删关键字所在结点中。
 *             （2）如果与该结点相邻的左（右）兄弟结点中的关键字数目均等于ceil(m/2)-1，则将删除了关键字的结点与双亲结点中分割二者（关键字所在结点及其兄弟结点）
 *                 的关键字合并到p所指的兄弟结点中去，p是双亲结点指向该删除关键字结点的右（左）兄弟结点的指针。
 *                 如果因此使双亲结点中关键字个数少于ceil(m/2)-1，则对此双亲结点继续合并，以致于可能直到对根结点做这样的处理而使整个树减少一层。
 */
Status BTree::Del(Node* r, KeyType x)
{
    if (NULL == r)
    {
        return NOT_FOUND;
    }

    int i, j, pivot, n = r->n;
    KeyType* k = r->k;  // k[i] means r->k[i]
    const int nMin = (M - 1) / 2;
    Status code;
    Node** p = r->p, * pL, * pR;       // p[i] means r->p[i]
    i = SearchInNode(x, k, n);
    if (p[0] == NULL) // *r is a leaf
    {
        if (i == n || x < k[i]) return NOT_FOUND;
        // x == k[i], and *r is a leaf
        for (j = i + 1; j < n; j++)
        {
            k[j - 1] = k[j];
            p[j] = p[j + 1];
        }
        return
                --r->n >= (r == root_ ? 1 : nMin) ? SUCCESS : UNDERFLOW;
    }
    // *r is an interior Node, not a leaf:
    if (i < n && x == k[i])
    {  // x found in an interior Node. Go to left child
        // *p[i] and follow a path all the way to a leaf,
        // using rightmost branches:
        Node* q = p[i], * q1;
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
    // Delete x in leaf of subtree with root_ p[i]:
    code = Del(p[i], x);
    if (code != UNDERFLOW) return code;
    // There is underflow; borrow, and, if necessary, merge:
    // Too few data items in Node *p[i]
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
        return SUCCESS;
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
        return SUCCESS;
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
            --r->n >= (r == root_ ? 1 : nMin) ? SUCCESS : UNDERFLOW;
}

int main()
{
    cout <<
    "B-tree structure shown by indentation. For each\n"
            "Node, the number of links to other nodes will not\n"
            "be greater than " << M <<
    ", the order M of the B-tree.\n\n"
            "Enter some integers, followed by a slash (/):\n";
    BTree t;
    KeyType x;
    char ch;
    while (cin >> x, !cin.fail()) t.Insert(x);
    cout <<
    "\nB-tree representation (indentation similar to the\n"
            "table of contents of a book). The items stored in\n"
            "each Node are displayed on a single line.\n";
    t.Print();
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
                t.Insert(x);
                break;
            case 'D':
                t.Delete(x);
                break;
            default:
                cout << "Invalid command, use S, I or D\n";
                break;
        }
        if (ch == 'I' || ch == 'D') t.Print();
    }
    return 0;
}

