import subprocess

class SubprocessReturnValue:
	def __init__(self, code, value):
		self.code=code
		self.value=value.decode('ascii', 'ignore')

	def __str__(self): return self.value
	def __eq__(self, other): return other==self.value
	def __contains__(self, k): return k in self.value

def call(*args):
	try:
		return SubprocessReturnValue(0, subprocess.check_output(args))
	except subprocess.CalledProcessError as e:
		return SubprocessReturnValue(e.returncode, e.output)

def callc(*args):
	return call("./compress", *args)

print("Testing... ")

assert "No operation" in callc(), "Accepted no args"
assert "Base Usage" in callc("-h"), "Didn't provide help"
assert ""==callc("-q"), "Didn't shut up with -q"

assert "Compressing" in callc("-c", "samples/hexdata.txt", "-o", "out.yz", "-astatic", "-ptree=hex2"),\
       "Didn't tell me it was compressing"
assert "Decompressing" in callc("-x", "out.yz", "-o", "out.txt"), "Didn't tell me it was decompressing"
assert ""==call("diff", "samples/hexdata.txt", "out.txt"), "Input != output"

assert ""==callc("-c", "samples/hexdata.txt", "-o", "out.yz", "-astatic", "-ptree=hex2", "-q"),\
       "Didn't shut up when compressing for real"

assert "Char 0 was found in file but not in LookupTable, ignoring" in \
       callc("-c", "samples/hexdata.txt", "-o", "out.yz", "-astatic", "-ptree=abc"),\
       "Didn't warn when compressing with bad tree"

assert "Extention deeznuts not found" in \
       callc("-c", "samples/hexdata.txt", "-o", "out.yz", "-adeeznuts"),\
       "Didn't fail with invalid extension"

print("ALL GOOD!")