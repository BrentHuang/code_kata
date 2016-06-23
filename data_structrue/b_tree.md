<script type="text/javascript" src="http://cdn.bootcss.com/mathjax/2.6.1/MathJax.js?config=default"></script>

$$x=\frac{-b\pm\sqrt{b^2-4ac}}{2a}$$
\\(x=\frac{-b\pm\sqrt{b^2-4ac}}{2a}\\)

B-树是一种平衡的多路查找树,它在文件系统中很有用.
定义: 一棵m阶的B-树,或者为空树,或为满足下列特性的m叉树:
- 树中每个结点至多有m棵子树;
- 若根结点不是叶子结点，则至少有两棵子树;
- 除根结点之外的所有非终端结点至少有[m/2]棵子树;
- 所有的非终端结点中包含以下信息数据:
<img src="http://www.forkosh.com/mathtex.cgi? (n, A_0, K_1, A_1, K_2, A_2, ..., K_n, A_n)">
$$ x = \dfrac{-b \pm \sqrt{b^2 - 4ac}}{2a} $$
其中:
 1. <img src="http://www.forkosh.com/mathtex.cgi? K_i"> 为关键码, 且<img src="http://www.forkosh.com/mathtex.cgi? K_i < K_{i+1} (n = 1, 2, \ldots, n)">.
 2. <img src="http://www.forkosh.com/mathtex.cgi? A_i (i = 0, 1, 2, \ldots, n)"> 为指向子树根结点的指针, 且指针<img src="http://www.forkosh.com/mathtex.cgi? A_{i-1}">所指子树中所有结点的关键码均小于<img src="http://www.forkosh.com/mathtex.cgi? K_i (i = 1, 2, \ldots, n)">, 指针<img src="http://www.forkosh.com/mathtex.cgi? A_i">所指子树中所有结点的关键码均大于<img src="http://www.forkosh.com/mathtex.cgi? K_i (i = 1, 2, \ldots, n)">.
 3. <img src="http://www.forkosh.com/mathtex.cgi? n  \lceil{m/2}\rceil - 1 \le n \le m - 1"> 为关键码的个数.
- 所有的叶子结点都出现在同一层次上, 并且不带信息(可以看作是外部结点或查找失败的结点, 实际上这些结点不存在, 指向这些结点的指针为空). 所有叶节点具有相同的深度, 等于树高度.

下图为一棵4阶B-树, 其深度为4.

![](https://github.com/BrentHuang/code_kata/blob/master/data_structrue/b_tree.jpg)
