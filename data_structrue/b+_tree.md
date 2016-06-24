B+树是B-树的一种变形树。

定义：一棵m阶的B+树，或者为空树，或者为满足下列特性的m叉树：
- 每个结点至多有m棵子树；
- 根结点要么没有子树，要么至少有2棵子树；
- 除根结点外，每个分支结点至少有<img src="http://www.forkosh.com/mathtex.cgi? \lceil{m/2}\rceil">棵子树；
- **所有的分支结点可以看成是索引部分，结点中仅含有其子树根结点中最大（或最小）关键字：
<img src="http://www.forkosh.com/mathtex.cgi? (n, A_0, K_1, A_1, K_2, A_2, ..., K_n, A_n)">，
其中：
  - <img src="http://www.forkosh.com/mathtex.cgi? K_i"> 为关键字, 且<img src="http://www.forkosh.com/mathtex.cgi? K_i < K_{i+1} (i = 1, 2, \ldots, n)">；
  - <img src="http://www.forkosh.com/mathtex.cgi? A_i (i = 0, 1, 2, \ldots, n)"> 为指向子树根结点的指针，且指针<img src="http://www.forkosh.com/mathtex.cgi? A_{i-1}">所指子树中所有结点的关键字均小于<img src="http://www.forkosh.com/mathtex.cgi? K_i (i = 1, 2, \ldots, n)">，指针<img src="http://www.forkosh.com/mathtex.cgi? A_i">所指子树中所有结点的关键字均大于<img src="http://www.forkosh.com/mathtex.cgi? K_i (i = 1, 2, \ldots, n)">；
  - <img src="http://www.forkosh.com/mathtex.cgi? n (\lceil{m/2}\rceil - 1 \le n \le m - 1)"> 为关键字的个数。**
- **所有的树叶结点都在最底层，包含了全部关键字以及指向相应记录的指针，且树叶节点按关键字的从小到大顺序链接；**
- **有n棵子树的分支结点中含有n个关键字，而且每个关键字都不小于对应子树中最大的关键字。**

下图为一棵3阶的B+树：

![](https://github.com/BrentHuang/code_kata/blob/master/data_structrue/b++_tree.jpg)

在B+树上通常有两个头指针，一个指向根结点，另一个指向关键字最小的树叶。因此可以对B+树进行两种查找操作：一种是从根节点开始进行树型查找，另一种是从最小关键字开始进行顺序查找。
