B-树是一种平衡的多路查找树，适用于组织文件的索引。

定义：一棵m阶的B-树，或者为空树，或者为满足下列特性的m叉树：

- 每个结点至多有m棵子树；
- 根结点至少有两棵子树；
- 除根结点外，每个分支结点至少有<img src="http://www.forkosh.com/mathtex.cgi? \lceil{m/2}\rceil">棵子树；
- 每个分支结点包含以下信息：
<img src="http://www.forkosh.com/mathtex.cgi? (n, A_0, K_1, A_1, K_2, A_2, ..., K_n, A_n)">，
其中：
  - <img src="http://www.forkosh.com/mathtex.cgi? K_i"> 为关键字, 且<img src="http://www.forkosh.com/mathtex.cgi? K_i < K_{i+1} (i = 1, 2, \ldots, n)">；
  - <img src="http://www.forkosh.com/mathtex.cgi? A_i (i = 0, 1, 2, \ldots, n)"> 为指向子树根结点的指针，且指针<img src="http://www.forkosh.com/mathtex.cgi? A_{i-1}">所指子树中所有结点的关键字均小于<img src="http://www.forkosh.com/mathtex.cgi? K_i (i = 1, 2, \ldots, n)">，指针<img src="http://www.forkosh.com/mathtex.cgi? A_i">所指子树中所有结点的关键字均大于<img src="http://www.forkosh.com/mathtex.cgi? K_i (i = 1, 2, \ldots, n)">；
  - <img src="http://www.forkosh.com/mathtex.cgi? n (\lceil{m/2}\rceil - 1 \le n \le m - 1)"> 为关键字的个数.
- 所有的树叶结点都在最底层，并且不包含任何信息（实际上这些树叶并不存在，指向这些树叶的指针为空）；
- 含有n个关键字的B-树正好有n+1个树叶。

下图为一棵4阶的B-树, 其深度为4.

![](https://github.com/BrentHuang/code_kata/blob/master/data_structrue/b_tree.jpg)
