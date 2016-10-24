#ifdef ENABLE_TESTS
#include <criterion/criterion.h>
#include "map.h"
#include "codingtree.h"
#include "log.h"
#include <stdio.h>

Test(demo, simple_success){
	cr_assert(1, "Hello World!");
}

Test(map, map_has){
	Map *map=Map_create();
	int i=42;
	Map_set(map, "test", &i);
	cr_assert(Map_has(map, "test"), "Map did not contain inserted item.");
	Map_destroy(map);
}

Test(map, map_getset_raw){
	Map *map=Map_create();
	int i=42;
	Map_set(map, "test", &i);
	int j=*((int*)Map_get(map, "test"));
	cr_assert(i==j, "Retrieved item != saved item");
	Map_destroy(map);
}

Test(map, map_delete){
	Map *map=Map_create();
	int i=42;
	Map_set(map, "test", &i);
	Map_del(map, "test");
	cr_assert(!Map_has(map, "test"), "Deleting item not destroying it!");
	cr_assert(i==42, "Deleting item modified value of it");
	Map_destroy(map);
}

Test(map, map_getset_string){
	Map *map=Map_create();
	Map_setstr(map, "test", "test value");
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(strcmp(Map_getstr(map, "test"), "test value")==0, "Retrieved!=Inserted");
	Map_destroy(map);
}

Test(map, map_getset_int){
	Map *map=Map_create();
	Map_setint(map, "test", 42);
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(Map_getint(map, "test")==42, "Retrieved!=Inserted");
	Map_destroy(map);
}

Test(map, map_getset_float){
	Map *map=Map_create();
	Map_setfloat(map, "test", 4.2f);
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(Map_getfloat(map, "test")==4.2f, "Retrieved!=Inserted");
	Map_destroy(map);
}

Test(map, map_getset_bool){
	Map *map=Map_create();
	Map_setbool(map, "test", false);
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(Map_getbool(map, "test")==false, "Retrieved!=Inserted");
	Map_destroy(map);
}

typedef int map_getset_func_sig(void);

int map_getset_func_testfunc(){
	return 1234;
}

Test(map, map_getset_func){
	Map *map=Map_create();
	Map_SETFUNC(map, "test", map_getset_func_sig, map_getset_func_testfunc);
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(Map_GETFUNC(map, "test", map_getset_func_sig)()==1234, "Called func did not return correct value");
	Map_destroy(map);
}

Test(tree, tree_create_destroy){
	TreeNode *tree=TreeNode_create(1);
	TreeNode_attach(tree, TreeNode_create_leaf('a'), 0);
	TreeNode_destroy(tree);
}

//Creates a simple tree for testing with
//Looks like this:
//              *
//             / \
//            a   *
//              // \\
//             bc  d*
//                 / \
//                e   f

TreeNode* create_demotree(){
	TreeNode* root=TreeNode_create(2);
	TreeNode* l1=TreeNode_create(4);
	TreeNode* l2=TreeNode_create(2);
	TreeNode_attach(root, TreeNode_create_leaf('a'), 0);
	TreeNode_attach(root, l1, 1);
	TreeNode_attach(l1, TreeNode_create_leaf('b'), 0);
	TreeNode_attach(l1, TreeNode_create_leaf('c'), 1);
	TreeNode_attach(l1, TreeNode_create_leaf('d'), 2);
	TreeNode_attach(l1, l2, 3);
	TreeNode_attach(l2, TreeNode_create_leaf('e'), 0);
	TreeNode_attach(l2, TreeNode_create_leaf('f'), 1);
	return root;
}

Test(tree, tree_create_demotree){
	TreeNode_destroy(create_demotree());
}

Test(tree, tree_leaf){
	TreeNode* dt=create_demotree();
	cr_assert(!TreeNode_leaf(dt), "Wrong return (true) for TreeNode_leaf call on node");
	TreeNode_destroy(dt);
}

Test(tree, tree_access_l0){
	TreeNode* dt=create_demotree();
	TreeNode* res=TreeNode_traverse(dt, 0);
	cr_assert(TreeNode_leaf(res), "Wrong return (false) for TreeNode_leaf call on leaf");
	cr_assert(res->value=='a', "Incorrect value in node after traversing one level");
	cr_assert(!TreeNode_leaf(TreeNode_traverse(dt, 1)),
		"Wrong return (true) for TreeNode_leaf call on traversed node");
	TreeNode_destroy(dt);
}

Test(tree, tree_access_l1){
	TreeNode* dt=create_demotree();
	TreeNode* next=TreeNode_traverse(dt, 1);
	cr_assert(TreeNode_leaf(TreeNode_traverse(next, 0)),
		"Wrong return (false) for TreeNode_leaf call on leaf (1->0)");
	cr_assert(TreeNode_leaf(TreeNode_traverse(next, 1)),
		"Wrong return (false) for TreeNode_leaf call on leaf (1->1)");
	cr_assert(TreeNode_leaf(TreeNode_traverse(next, 2)),
		"Wrong return (false) for TreeNode_leaf call on leaf (1->2)");
	cr_assert(!TreeNode_leaf(TreeNode_traverse(next, 3)),
		"Wrong return (true) for TreeNode_leaf call on node (1->3)");
	cr_assert(TreeNode_traverse(next, 0)->value=='b', "Wrong value for leaf (1->0)");
	cr_assert(TreeNode_traverse(next, 1)->value=='c', "Wrong value for leaf (1->1)");
	cr_assert(TreeNode_traverse(next, 2)->value=='d', "Wrong value for leaf (1->2)");
	TreeNode_destroy(dt);
}

Test(tree, tree_access_l2){
	TreeNode* dt=create_demotree();
	TreeNode* next=TreeNode_traverse(TreeNode_traverse(dt, 1), 3);
	cr_assert(TreeNode_leaf(TreeNode_traverse(next, 0)),
		"Wrong return (false) for TreeNode_leaf call on node (1->3->0)");
	cr_assert(TreeNode_leaf(TreeNode_traverse(next, 1)),
		"Wrong return (false) for TreeNode_leaf call on node (1->3->1)");
	cr_assert(TreeNode_traverse(next, 0)->value=='e', "Wrong value for leaf (1->3->0)");
	cr_assert(TreeNode_traverse(next, 1)->value=='f', "Wrong value for leaf (1->3->1)");
	TreeNode_destroy(dt);
}

Test(tree, tree_count){
	TreeNode *dt=create_demotree();
	cr_assert(TreeNode_count(dt)==9, "Incorrect TreeNode_count result");
	TreeNode_destroy(dt);
}

Test(tree, tree_serialize){
	TreeNode *dt=create_demotree();
	cr_assert(Tree_savetobuf_size(TreeNode_count(dt))==18, "Incorrect size for saving");
	void *buf=Tree_savetobuf(dt);
	TreeNode *new=Tree_loadfrombuf(buf);
	TreeNode_destroy(dt);
	TreeNode_destroy(new);
	free(buf);
}

Test(tree, tree_serialize_cmp){
	TreeNode *dt=create_demotree();
	cr_assert(Tree_savetobuf_size(TreeNode_count(dt))==18, "Incorrect size for saving");
	void *buf=Tree_savetobuf(dt);
	TreeNode *new=Tree_loadfrombuf(buf);
	
	cr_assert(TreeNode_count(dt)==TreeNode_count(new), "New nodes!=Old nodes");
	cr_assert(TreeNode_traverse(new, 0)->value=='a', "New leaf (0) != Old leaf");
	cr_assert(TreeNode_traverse(TreeNode_traverse(new, 1),0)->value=='b', "New leaf (1->0) != Old leaf");
	cr_assert(TreeNode_traverse(TreeNode_traverse(new, 1),1)->value=='c', "New leaf (1->1) != Old leaf");
	cr_assert(TreeNode_traverse(TreeNode_traverse(new, 1),2)->value=='d', "New leaf (1->2) != Old leaf");
	cr_assert(TreeNode_traverse(TreeNode_traverse(TreeNode_traverse(new, 1),3),0)->value=='e',
		"New leaf (1->3->0) != Old leaf");
	cr_assert(TreeNode_traverse(TreeNode_traverse(TreeNode_traverse(new, 1),3),1)->value=='f',
		"New leaf (1->3->1) != Old leaf");

	TreeNode_destroy(dt);
	TreeNode_destroy(new);
	free(buf);
}

#endif