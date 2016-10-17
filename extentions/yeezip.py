import array, functools

yee_extension_exported_serialized_tree="" #top-level symbol that will be grabbed by C

def export(tree):
	"""Turn a TreeNode and it's children into a bytearray, and save it to yee_extension_exported_serialized_tree"""
	global yee_extension_exported_serialized_tree
	yee_extension_exported_serialized_tree=array.array('B', tree.export()).tostring()

def get_freq():
	"""Wrapper to return the freqency table as passed by C"""
	return yee_extension_frequency_table

def get_parameters():
	"""Wrapper to return app parameters as passed by C"""
	return yee_extension_parameters

def filtered_print(*a, level=3, start="", **k):
	"""Emulate yeezip log behavior"""
	if get_parameters().get("loglevel", 0)<=level:
		print(start+"<PYTHON>\t", end="")
		print(*a, **k)

#set up logging functions for each level
for idx, name in enumerate(["alloc", "spam", "debug", "status", "info", "warn", "error", "fail"]):
	globals()[name]=functools.partial(filtered_print, level=idx+1)

debug("Bootstrap python code running")

debug("get_parameters() -> "+str(get_parameters()))

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
		"""Export this item and children as a list of integers in the same format used by yeezip"""
		if self.child_count==0:
			return [self.value, 0] #Leaf, no children, so just reurn my value
		
		ret=[0,self.child_count] #Node, no value, so just return my count and children
		for item in self.children:
			ret.extend(item.export()) #Extend (format is a flat array) with children (and their children eventually etc)
		return ret

debug("Bootstrap python code finishing")