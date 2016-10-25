#ifdef ENABLE_TESTS
#include <criterion/criterion.h>
#include "map.h"
#include "codingtree.h"
#include "log.h"
#include "args.h"
#include "extension.h"
#include "io.h"
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

TreeNode* create_demotree(){
	//Creates a simple tree for testing with
	//Looks like this:
	//              *
	//             / \
	//            a   *
	//              // \\
	//             bc  d*
	//                 / \
	//                e   f
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

Test(args, args_no_cmdline){
	loglevel=999;
	char *args[]={"compress"};
	Intent *i=parse_args(1, args);
	cr_assert(i->intent==INTENT_NONE, "Produced an intent when none passed");
	cr_assert(i->invalid, "Produced a valid intent w/o a intent");
	free(i);
}

Test(args, args_help){
	loglevel=999;
	char *args[]={"compress", "-h"};
	Intent *i=parse_args(2, args);
	cr_assert(i->intent==INTENT_HELP, "Returned incorrect intent for -h");
	free(i);
}

Test(args, args_compress_noalg){
	loglevel=999;
	char *args[]={"compress", "-c", "foo", "-o", "bar"};
	Intent *i=parse_args(5, args);
	cr_assert(i->intent==INTENT_COMPRESS, "Returnd incorrect intent for -c");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile");
	cr_assert(!i->skip_save_tree, "Generated suprious skip_save_tree");
	cr_assert_str_empty(i->algorithm, "Generated suprious algorithm");
	free(i);
}

Test(args, args_decompress){
	loglevel=999;
	char *args[]={"compress", "-e", "foo", "-o", "bar"};
	Intent *i=parse_args(5, args);
	cr_assert(i->intent==INTENT_DECOMPRESS, "Returnd incorrect intent for -e");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile");
	free(i);
}

Test(args, args_decompress_x){
	loglevel=999;
	char *args[]={"compress", "-x", "foo", "-o", "bar"};
	Intent *i=parse_args(5, args);
	cr_assert(i->intent==INTENT_DECOMPRESS, "Returnd incorrect intent for -x");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile (with -x)");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile (with -x)");
	free(i);
}

Test(args, args_compress_noalg_exttree){
	loglevel=999;
	char *args[]={"compress", "-c", "foo", "-o", "bar", "-s", "baz"};
	Intent *i=parse_args(7, args);
	cr_assert(i->intent==INTENT_COMPRESS, "Returnd incorrect intent for -c");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile");
	cr_assert_str_eq(i->savefile, "baz", "Incorrect outfile");
	cr_assert(!i->skip_save_tree, "Generated suprious skip_save_tree");
	cr_assert_str_empty(i->algorithm, "Generated suprious algorithm");
	free(i);
}

Test(args, args_compress_noalg_exttree_dontsave){
	loglevel=999;
	char *args[]={"compress", "-c", "foo", "-o", "bar", "-s", "baz", "-O"};
	Intent *i=parse_args(8, args);
	cr_assert(i->intent==INTENT_COMPRESS, "Returnd incorrect intent for -c");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile");
	cr_assert_str_eq(i->savefile, "baz", "Incorrect outfile");
	cr_assert(i->skip_save_tree, "Didn't note -O");
	cr_assert_str_empty(i->algorithm, "Generated suprious algorithm");
	free(i);
}

Test(args, args_compress_alg){
	loglevel=999;
	char *args[]={"compress", "-c", "foo", "-o", "bar", "-afargle"};
	Intent *i=parse_args(6, args);
	cr_assert(i->intent==INTENT_COMPRESS, "Returnd incorrect intent for -c");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile");
	cr_assert_str_eq(i->algorithm, "fargle", "Incorrect algorithm");
	cr_assert(!i->skip_save_tree, "suprious -O");
	free(i);
}

Test(args, args_compress_alg_exttree){
	loglevel=999;
	char *args[]={"compress", "-c", "foo", "-o", "bar", "-afargle", "-s", "baz"};
	Intent *i=parse_args(8, args);
	cr_assert(i->intent==INTENT_COMPRESS, "Returnd incorrect intent for -c");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile");
	cr_assert_str_eq(i->algorithm, "fargle", "Incorrect algorithm");
	cr_assert(!i->skip_save_tree, "suprious -O");
	free(i);
}

Test(args, args_compress_alg_exttree_dontsave){
	loglevel=999;
	char *args[]={"compress", "-c", "foo", "-o", "bar", "-afargle", "-s", "baz", "-O"};
	Intent *i=parse_args(9, args);
	cr_assert(i->intent==INTENT_COMPRESS, "Returnd incorrect intent for -c");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile");
	cr_assert_str_eq(i->algorithm, "fargle", "Incorrect algorithm");
	cr_assert(i->skip_save_tree, "Didn't note -O");
	free(i);
}

Test(args, args_decompress_loadtree){
	loglevel=999;
	char *args[]={"compress", "-x", "foo", "-o", "bar", "-l", "fargle"};
	Intent *i=parse_args(7, args);
	cr_assert(i->intent==INTENT_DECOMPRESS, "Returnd incorrect intent for -x");
	cr_assert_str_eq(i->infile, "foo", "Incorrect infile");
	cr_assert_str_eq(i->outfile, "bar", "Incorrect outfile");
	cr_assert_str_eq(i->importfile, "fargle", "Incorrect importfie");
	free(i);
}

Test(args, args_parameter){
	loglevel=999;
	char *args[]={"compress", "-pfoo=bar"};
	Intent *i=parse_args(2, args);
	cr_assert(Map_has(i->options, "foo"), "Didn't get parameter");
	cr_assert_str_eq(Map_getstr(i->options, "foo"), "bar", "Incorrect parameter value");
	free(i);
}

Test(args, args_parameter_P){
	loglevel=999;
	char *args[]={"compress", "-Pfoo"};
	Intent *i=parse_args(2, args);
	cr_assert(Map_has(i->options, "foo"), "Didn't get parameter");
	free(i);
}

Test(ext, ext_load_single){
	ExtensionData *ext=import_lib(EXTENSION_PATH "/testtree.so");
	cr_assert(ext!=0, "Didn't load extension");
	cr_assert_str_eq(ext->name, "static", "Incorrect extension name");
	Map *m=Map_create();
	free(ext->builder(999, m, 0));
	Map_destroy(m);
	free(ext);
}

Test(ext, ext_load_dir){
	Map *m=Map_create();
	load_extensions(EXTENSION_PATH, m);
	cr_assert(Map_has(m, "static"));
	free(m);
}

Test(io, io_byte_set){
	byte b=0;
	Byte_setbit(&b, 0, true);
	cr_assert(b==1, "Byte_setbit didn't set right");
}

Test(io, io_byte_set_multiple){
	byte b=0;
	Byte_setbit(&b, 0, true);
	Byte_setbit(&b, 1, true);
	cr_assert(b==3, "Byte_setbit didn't set right");
}

Test(io, io_byte_get){
	byte b=2;
	cr_assert(Byte_getbit(b, 1), "Byte_getbit didn't get right");
	cr_assert(Byte_getbit(b, 0)==0, "Byte_getbit didn't get right (2)");
}

Test(io, io_min_bits){
	cr_assert(min_bits_to_represent(1)==1, "min_bits_to_represent incorrect for 1");
	cr_assert(min_bits_to_represent(2)==1, "min_bits_to_represent incorrect for 2");
	cr_assert(min_bits_to_represent(3)==2, "min_bits_to_represent incorrect for 3");
	cr_assert(min_bits_to_represent(4)==2, "min_bits_to_represent incorrect for 4");
	cr_assert(min_bits_to_represent(5)==3, "min_bits_to_represent incorrect for 5");
	cr_assert(min_bits_to_represent(8)==3, "min_bits_to_represent incorrect for 8");
	cr_assert(min_bits_to_represent(9)==4, "min_bits_to_represent incorrect for 9");
	cr_assert(min_bits_to_represent(16)==4, "min_bits_to_represent incorrect for 16");
	cr_assert(min_bits_to_represent(17)==5, "min_bits_to_represent incorrect for 17");
	cr_assert(min_bits_to_represent(32)==5, "min_bits_to_represent incorrect for 32");
	cr_assert(min_bits_to_represent(33)==6, "min_bits_to_represent incorrect for 33");
	cr_assert(min_bits_to_represent(64)==6, "min_bits_to_represent incorrect for 64");
	cr_assert(min_bits_to_represent(65)==7, "min_bits_to_represent incorrect for 65");
	cr_assert(min_bits_to_represent(128)==7, "min_bits_to_represent incorrect for 128");
	cr_assert(min_bits_to_represent(129)==8, "min_bits_to_represent incorrect for 129");
	cr_assert(min_bits_to_represent(256)==8, "min_bits_to_represent incorrect for 256");
	cr_assert(min_bits_to_represent(257)==9, "min_bits_to_represent incorrect for 257");
}

Test(io, io_min_bits_nuts){
	cr_assert(min_bits_to_represent(123871)==17, "min_bits_to_represent incorrect for 123871");
	cr_assert(min_bits_to_represent(12344)==14, "min_bits_to_represent incorrect for 12344");
	cr_assert(min_bits_to_represent(128319273)==27, "min_bits_to_represent incorrect for 128319273");
}

//There are no tests for the Memory Mapping and BitFile stuff, as there is no way
// to isolate it from the rest of the file stuff (as it operates on the file as a
// stream to avoid pulling the whole thing into memory)

#endif