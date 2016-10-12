tree=TreeNode()
[tree.add(TreeNode(i)) for i in range(15)]
n2=TreeNode()
[n2.add(TreeNode(i)) for i in range(4)]
tree.add(n2)
export(tree)