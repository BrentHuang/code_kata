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

static const int M = 5;  // Order of B-tree: M link fields in each node

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
    KeyType k[M - 1]; // Data items (only the first n in use)  TODO 这里省略了data字段，B-树的结点中应该有data字段
    Node* p[M];   // Pointers to other nodes (n+1 in use)
};

struct SearchResult
{
    Node* node;
    int i;
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
            // 通过后续遍历释放各个结点
        }
    }

    /**
     * @brief 从根结点开始打印整棵B-树中的所有关键字
     */
    void Print() const
    {
        cout << "Contents:" << endl;
        PrintNode(root_, 0);
    }

    /**
     * @brief 从根结点开始打印搜索过程
     * @param x 待查找的关键字
     * @details 从根结点开始，先打印结点内的所有关键字，然后换行；接着在结点内查找，如果找到则打印x的数组下标并结束；
     *          如果找不到则进入子树中查找，循环进行直到遇到叶结点为止，打印not found并结束
     */
    SearchResult ShowSearch(KeyType x) const; // SearchResult是找到的结点，其中的关键字k[i] == x

    /**
     * @brief 向B-树中插入一个关键字
     * @param x 待插入的关键字
     * @details 如果关键字已经存在了则不插入，打印一句duplicate key提示
     * @return =0插入成功，否则失败
     */
    int Insert(KeyType x);

    /**
     * @brief 从B-树中删除一个关键字
     * @param x 待删除的关键字
     * @details 如果关键字不存在则打印一句not found提示
     * @return =0删除成功，否则失败
     */
    int Delete(KeyType x);

private:
    /**
     * @brief 前序遍历打印结点及其所有子树中的关键字
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
     * @return i 要么在结点内找到，则i就是数组下标；找不到则说明x在在p[i]指向的子树中
     */
    int SearchInNode(KeyType x, const KeyType* k, int n) const;

    /**
     * @brief 将关键字插入到结点中
     * @param r 往其中插入的结点
     * @param x 待插入的关键字
     * @prarm y 往上提到父结点中的关键字
     * @param q 分裂新增的结点
     * @return SUCCESS/DUPLICATE_KEY/INSERT_NOT_COMPLETE
     * @details B-树的插入规则：首先在最底层的某个分支结点（通过查找得到）中添加待插入的关键字，若插入前该结点内的关键字个数小于m-1，则直接插入即可；
     *          否则要进行结点的分裂，将该结点分裂成2个结点，左边一个结点包含前ceil(m/2)-1个关键字，右边一个结点包含后m-ceil(m/2)个关键字，
     *          中间的那个关键字插入到父结点中。这样父结点中就多了一个关键字，可能需要继续分裂，分裂过程可能会一直波及到根结点。
     */
    Status Ins(Node* r, KeyType x, KeyType& y, Node*& u);

    /**
     * @details 首先找到待删除的关键字所在的结点，
     *          1，如果待删除的关键字在最下面一层的分支结点中，
     *          1.1，如果待删关键字所在结点的关键字个数大于ceil(m/2)-1，则从该结点中直接删去该关键字即可
     *          1.2，待删关键字所在结点p的关键字个数等于ceil(m/2)-1，则删去该关键字后需要进行结点的合并：
     *             1.2.1，如果与结点p相邻的右兄弟（或左兄弟）结点q中的关键字个数大于ceil(m/2)-1个，则借一个过来。设K[pivot]是p中大于（或小于）x的最小（或最大）关键字，
     *                    则将K[pivot]下移至p中，把q的最小（或最大）关键字上移至r的K[pivot]处。注意对右兄弟和左兄弟都要做。
     *             1.2.2，与结点p相邻的右兄弟和左兄弟（也可能只有一个兄弟）结点中的关键字数目均等于ceil(m/2)-1，则需要把删除关键字x之后的p与其右兄弟（或左兄弟）结点以及
     *                    父结点中分割二者的关键字K[pivot]合并成一个结点。分2种情况：（1）如果没有右兄弟，则将K[pivot]和自己合并到左兄弟中（2）有右兄弟，则将K[pivot]和右兄弟合并到自己中。
     *                    由于合并会减少父结点中的一个关键字，如果因此使得父结点中的关键字个数少于ceil(m/2)-1，则对此父结点继续操作（要么从它的兄弟借，要么与它的兄弟合并），
     *                    合并过程可能会一直波及到根节点。
     *          2，待删除的关键字不在最下面一层的分支结点中，设待删关键字为该结点中第i个关键字key[i]，则用相邻的右子树p[i]（或左子树p[i-1]）
     *             中的最小（或最大）关键字y与x交换，然后在p[i]（或p[i-1]）所指的子树中删去x。这样就将问题转化为第一种情况了
     */
    Status Del(Node* r, KeyType x);

private:
    Node* root_;
};

SearchResult BTree::ShowSearch(KeyType x) const
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
            cout << "Key " << x << " found in position " << i << " of last displayed node." << endl;
            return {r, i};
        }

        r = r->p[i]; // 进入p[i]子树
    }

    cout << "Key " << x << " not found." << endl;
    return { NULL, -1 };
}

int BTree::Insert(KeyType x)
{
    int ret = 0;
    KeyType y;
    Node* q = NULL;

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
            Node* root = root_;
            root_ = new Node;
            root_->n = 1;
            root_->k[0] = y;
            root_->p[0] = root;
            root_->p[1] = q;
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

int BTree::Delete(KeyType x)
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
            Node* root = root_;
            root_ = root_->p[0];
            delete root;
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

void BTree::PrintNode(const Node* node, int indent_space_count) const
{
    if (NULL == node)
    {
        return;
    }

    cout << setw(indent_space_count) << "";
    int i;

    for (i = 0; i < node->n; ++i)
    {
        cout << setw(3) << node->k[i] << " ";
    }

    cout << endl;

    for (i = 0; i <= node->n; ++i)
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

Status BTree::Del(Node* r, KeyType x)
{
    if (NULL == r)
    {
        return NOT_FOUND;
    }

    KeyType* k = r->k;  // k[i] means r->k[i]
    Node** p = r->p;
    Node* pL = NULL;
    Node* pR = NULL;       // p[i] means r->p[i]
    int i, j, pivot, n = r->n;
    const int N_MIN = (M - 1) / 2;
    Status code;

    i = SearchInNode(x, k, n);
    if (NULL == p[0]) // *r is a leaf 待删除的关键字在最下面一层的分支结点中（或者说就是叶结点）
    {
        if (i == n || x < k[i])
        {
            return NOT_FOUND;
        }

        // x == k[i], and *r is a leaf
        for (j = i + 1; j < n; ++j)
        {
            k[j - 1] = k[j];
            p[j] = p[j + 1];
        }

        return (--(r->n) >= (r == root_ ? 1 : N_MIN)) ? SUCCESS : UNDERFLOW;
    }

    // *r is an interior Node, not a leaf: 待删除的关键字不在最下面一层的分支结点中（或者说不是叶结点）
    if (i < n && x == k[i]) // i==0和1<=i<=n-1表示关键字在本结点中；i==n表示关键字在结点的最后一棵子树p[n]中
    {  // x found in an interior Node. Go to left child
        // *p[i] and follow a path all the way to a leaf,
        // using rightmost branches:
        // 这里是用左子树中的最大关键字与x互换，下面一段循环就是找左子树中的最大关键字
        Node* q = p[i], * q1;
        int nq;

        for (; ;)
        {
            nq = q->n;
            q1 = q->p[nq];

            if (q1 == NULL)
            {
                break;
            }

            q = q1;
        }

        // Exchange k[i] (= x) with rightmost item in leaf: 交换两个关键字
        k[i] = q->k[nq - 1];
        q->k[nq - 1] = x;
    }

    // Delete x in leaf of subtree with root p[i]: 从左子树p[i]中删除x
    code = Del(p[i], x);
    if (code != UNDERFLOW)
    {
        return code;
    }

    // 注意下面均是相对于p[i]操作，p[i-1]是p[i]的左兄弟，p[i+1]是p[i]的右兄弟，且已经从p[i]中删除了x的

    // There is underflow; borrow, and, if necessary, merge:
    // Too few data items in Node *p[i]
    if (i > 0 && p[i - 1]->n > N_MIN) // Borrow from left sibling 如果i==0就没有左兄弟了，这个逻辑就不考虑了
    {
        // 有相邻的左兄弟且其中的关键字个数大于ceil(m/2)-1，则从左兄弟中借一个关键字，怎么借？通过父结点中转，画个图理理思路
        pivot = i - 1; // k[pivot] between pL and pR: K[pivot]是待删结点的父结点中小于x的最大关键字
        pL = p[pivot]; // 左兄弟
        pR = p[i]; // pR就是真被删关键字的那个结点

        // Increase contents of *pR, borrowing from *pL:
        // 将K[pivot]插入到pR中作为第一个关键字，后面的关键字顺次后移
        pR->p[pR->n + 1] = pR->p[pR->n];

        for (j = pR->n; j > 0; --j)
        {
            pR->k[j] = pR->k[j - 1];
            pR->p[j] = pR->p[j - 1];
        }

        ++pR->n;
        pR->k[0] = k[pivot];
        pR->p[0] = pL->p[pL->n]; // pL->k[pL->n - 1]是左兄弟中的最大关键字，下一步是把这个最大关键字放到pivot的位置并从左兄弟中删掉，那么相应的子树pL->p[pL->n]应该放哪里呢？就这里！

        // 将左子树的最大关键字上移到待删结点的pivot位置
        k[pivot] = pL->k[--(pL->n)];

        return SUCCESS;
    }

    if (i < n && p[i + 1]->n > N_MIN) // Borrow from right sibling
    {
        // 有相邻的右兄弟且其中的关键字个数大于ceil(m/2)-1，这个if的逻辑是从右兄弟借
        pivot = i; // k[pivot] between pL and pR:
        pL = p[pivot];
        pR = p[pivot + 1];

        // Increase contents of *pL, borrowing from *pR:
        pL->k[pL->n] = k[pivot];
        pL->p[pL->n + 1] = pR->p[0];
        k[pivot] = pR->k[0];
        ++(pL->n);
        --(pR->n);

        for (j = 0; j < pR->n; ++j)
        {
            pR->k[j] = pR->k[j + 1];
            pR->p[j] = pR->p[j + 1];
        }

        pR->p[pR->n] = pR->p[pR->n + 1];

        return SUCCESS;
    }

    // Merge; neither borrow left nor borrow right possible.
    // 相邻的左兄弟和右兄弟（也可能只有一个）中的关键字数目均等于ceil(m/2)-1，没得借了！这时采用合并的方法，怎么合并？
    pivot = (i == n ? i - 1 : i); // i==n时pR就是自己，pL是左兄弟；i<n时pL就是自己，pR是右兄弟
    pL = p[pivot];
    pR = p[pivot + 1];

    // Add k[pivot] and *pR to *pL: 将K[pivot]和pR合并到pL中去，删除pR
    pL->k[pL->n] = k[pivot];
    pL->p[pL->n + 1] = pR->p[0];

    for (j = 0; j < pR->n; ++j)
    {
        pL->k[pL->n + 1 + j] = pR->k[j];
        pL->p[pL->n + 2 + j] = pR->p[j + 1];
    }

    pL->n += (1 + pR->n);
    delete pR;

    // 父节点中的关键字减1
    for (j = i + 1; j < n; ++j)
    {
        k[j - 1] = k[j];
        p[j] = p[j + 1];
    }

    return (--(r->n) >= (r == root_ ? 1 : N_MIN)) ? SUCCESS : UNDERFLOW;
}

int main()
{
    cout << "B-tree structure shown by indentation. For each" << endl
        << "node, the number of links to other nodes will not" << endl
        << "be greater than " << M
        << ", the order M of the B-tree." << endl << endl
        << "Enter some integers, followed by a slash (/):" << endl;

    BTree tree;
    KeyType x;
    char ch;

    while (cin >> x, !cin.fail())
    {
        tree.Insert(x);
    }

    cout << endl
        << "B-tree representation (indentation similar to the" << endl
        << "table of contents of a book). The items stored in" << endl
        << "each node are displayed on a single line." << endl;

    tree.Print();

    cin.clear();
    cin >> ch; // Skip terminating character

    for (; ;)
    {
        cout << endl
            << "Enter an integer, followed by I, D, or S (for" << endl
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
