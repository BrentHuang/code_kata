# B+树
B+树是B-树的一种变体。

定义：一棵m阶的B+树，其基本定义与B-树相同,除了：
- **非叶子结点的子树个数与关键字个数相同，而且每个关键字都不小于对应子树中最大的关键字,即p[i]指向的子树中的关键字在范围(k[i-1],k[i]]中; (另一种定义为:P[i]指向的子树中的关键字在范围[K[i], K[i+1])中**
- **所有的关键字都在树叶结点中出现，叶子节点中包含了全部关键字以及指向相应记录的指针;**
- **叶子结点中增加了一个指针,将所有树叶节点按关键字从小到大顺序链接起来；**
- 每个叶子结点中的关键字个数的范围为  - <img src="http://www.forkosh.com/mathtex.cgi? n (\lceil{m/2}\rceil - 1 \le n \le m - 1)">；


下图为一棵3阶的B+树：

![](https://github.com/BrentHuang/code_kata/blob/master/data_structrue/b++_tree.jpg)

![](https://github.com/BrentHuang/code_kata/blob/master/data_structrue/b++_tree2.jpg)

在B+树上通常有两个头指针，一个指向根结点，另一个指向关键字最小的树叶。因此可以对B+树进行两种查找操作：一种是从根节点开始进行树型查找，另一种是从最小关键字开始进行顺序查找。


