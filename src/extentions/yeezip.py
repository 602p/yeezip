import array

print("Bootstrap python code running")

class TreeNode(object):
	def __init__(self, value=0, children=None):
		self.value=value
		self.children=children if children is not None else []

	@property
	def child_count(self):
		return len(self.children)

	@property
	def rounded_child_count(self):
		for item in [2,4,8,16,32,64,128,256]:
			if self.child_count<=item:
				return item

	def add(self, child):
		self.children.append(child)
		return child

	def export(self):
		if self.child_count==0:
			return [self.value, 0]
		
		ret=[0,self.child_count]
		for item in self.children:
			ret.extend(item.export())
		return ret

yee_extension_exported_serialized_tree=""

def export(tree):
	global yee_extension_exported_serialized_tree
	yee_extension_exported_serialized_tree=array.array('B', tree.export()).tostring()

def get_freq():
	return yee_extension_frequency_table

def get_parameters():
	return yee_extension_parameters

print("Bootstrap python code finishing")