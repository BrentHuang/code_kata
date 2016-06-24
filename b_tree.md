# B-树

B-树是一种平衡的多路查找树，适用于组织文件的索引。

定义：一棵m（m>2)阶的B-树，或者为空树，或者为满足下列特性的m叉树：
- 根结点至少有2棵子树，至多有m棵子树；
- 除根结点外，每个分支结点至少有<img src="http://www.forkosh.com/mathtex.cgi? \lceil{m/2}\rceil">棵子树，至多有m棵子树；
- 每个非树叶节点中至少有<img src="http://www.forkosh.com/mathtex.cgi? (\lceil{m/2}\rceil - 1 ">个关键字，至多有<img src="http://www.forkosh.com/mathtex.cgi? m - 1 ">个关键字；
- 每个非树叶结点包含以下信息：
<img src="http://www.forkosh.com/mathtex.cgi? (n, A_0, K_1, A_1, K_2, A_2, ..., K_n, A_n)">，
其中：
  - <img src="http://www.forkosh.com/mathtex.cgi? n (\lceil{m/2}\rceil - 1 \le n \le m - 1)"> 为关键字的个数；
  - <img src="http://www.forkosh.com/mathtex.cgi? K_i"> 为关键字, 且<img src="http://www.forkosh.com/mathtex.cgi? K_i < K_{i+1} (i = 1, 2, \ldots, n)">；
  - <img src="http://www.forkosh.com/mathtex.cgi? A_i (i = 0, 1, 2, \ldots, n)"> 为指向子树根结点的指针，且指针<img src="http://www.forkosh.com/mathtex.cgi? A_{i-1}">所指子树中所有结点的关键字均小于<img src="http://www.forkosh.com/mathtex.cgi? K_i (i = 1, 2, \ldots, n)">，指针<img src="http://www.forkosh.com/mathtex.cgi? A_i">所指子树中所有结点的关键字均大于<img src="http://www.forkosh.com/mathtex.cgi? K_i (i = 1, 2, \ldots, n)">。
- 所有的树叶结点都在最底层，并且不包含任何信息（实际上这些树叶并不存在，指向这些树叶的指针为空）；
- 含有n个关键字的B-树正好有n+1个树叶。

下图为一棵4阶的B-树，其深度为4。

![](https://github.com/BrentHuang/code_kata/blob/master/data_structrue/b_tree.jpg)

B-树的特性：
       1.关键字集合分布在整颗树中；
       2.任何一个关键字出现且只出现在一个结点中；
       3.搜索有可能在非叶子结点结束；
       4.其搜索性能等价于在关键字全集内做一次二分查找；
       5.自动层次控制；

B-树的搜索，从根结点开始，对结点内的关键字（有序）序列进行二分查找，如果命中则结束，否则进入查询关键字所属范围的儿子结点；重复，直到所对应的儿子指针为空，或已经是叶子结点；

$$ \alpha + \beta $$

<img src="http://latex.codecogs.com/gif.latex?\frac{\partial J}{\partial \theta_k^{(j)}}=\sum_{i:r(i,j)=1}{\big((\theta^{(j)})^Tx^{(i)}-y^{(i,j)}\big)x_k^{(i)}}+\lambda \theta_k^{(j)}" />  

![](http://latex.codecogs.com/gif.latex?\\frac{\\partial J}{\\partial \\theta_k^{(j)}}=\\sum_{i:r(i,j)=1}{\\big((\\theta^{(j)})^Tx^{(i)}-y^{(i,j)}\\big)x_k^{(i)}}+\\lambda \\xtheta_k^{(j)})

![](http://latex.codecogs.com/gif.latex?\alpha + \beta = 1)

![](http://www.forkosh.com/mathtex.cgi? \lceil{m/2}\rceil)

![](http://latex.codecogs.com/gif.latex?\lceil{m/2}\rceil)