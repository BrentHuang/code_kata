// B+树是应文件系统所需而产生的一种B-树的变形树。一棵m阶的B+树和m阶的B-树的差异在于：
// ⑴有n棵子树的结点中含有n个关键码；
// ⑵所有的叶子结点中包含了全部关键码的信息，及指向含有这些关键码记录的指针，且叶子结点本身依关键码的大小自小而大的顺序链接。
// ⑶所有的非终端结点可以看成是索引部分，结点中仅含有其子树根结点中最大（或最小）关键码。