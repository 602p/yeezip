import struct
import sys

flags={
	"HF_NOTREE" : 1,
	"HF_FLAT"   : 2,
	"HF_PTREE"  : 4
}

for k,v in flags.items(): globals()[k]=v

with open(sys.argv[1], 'rb') as fd:
	s=struct.unpack("!cccBBI", fd.read(9))
	print("Magic String: %s"%"".join([c.decode("ascii") for c in s[:3]]))
	print("Version: %i"%s[3])
	print("Flags: %s"%(", ".join([k for k,v in flags.items() if s[4]&v]) if s[4]!=0 else "HF_NONE"))
	print("Filesize: %i"%s[5])
	if not s[4]&HF_NOTREE:
		print("===Tree Stored===")
		nodes=struct.unpack("!I", fd.read(4))[0]
		print("Nodes: %i"%nodes)
		indent=0
		nodenum=0
		child_stack=[]
		while nodenum<nodes:
			node=struct.unpack("!cB", fd.read(2))
			print(("\t"*indent)+"Value: %s\tChildren: %i"%(repr(node[0].decode('ascii')) if node[0]!=b'\x00' else "<Container>", node[1]))
			if node[1]:
				indent+=1
				child_stack.append(node[1])
			else:
				child_stack[-1]-=1
				if child_stack[-1]==0:
					child_stack.pop()
					indent-=1
			nodenum+=1