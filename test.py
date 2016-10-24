import subprocess, sys

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

def test(val, msg):
	if not val:
		global bueno
		bueno=False
		print("\033[91m***FAILED: "+msg+"!***\033[0m")

if "nointeg" not in sys.argv:
	bueno=True

	print("Building...")
	call("python", "-m", "build", "espam", "lln", "rbx")

	print("Running Integration tests... ")

	test("No operation" in callc(), "Accepted no args")
	test("Base Usage" in callc("-h"), "Didn't provide help")
	test(""==callc("-q"), "Didn't shut up with -q")

	test("Compressing" in callc("-c", "samples/hexdata.txt", "-o", "out.yz", "-astatic", "-ptree=hex2"),\
	       "Didn't tell me it was compressing")
	test("Decompressing" in callc("-x", "out.yz", "-o", "out.txt"), "Didn't tell me it was decompressing")
	test(""==call("diff", "samples/hexdata.txt", "out.txt"), "Input != output")

	test(""==callc("-c", "samples/hexdata.txt", "-o", "out.yz", "-astatic", "-ptree=hex2", "-q"),\
	       "Didn't shut up when compressing for real")

	test("Char 0 was found in file but not in LookupTable, ignoring" in \
	       callc("-c", "samples/hexdata.txt", "-o", "out.yz", "-astatic", "-ptree=abc"),\
	       "Didn't warn when compressing with bad tree")

	test("Extention deeznuts not found" in \
	       callc("-c", "samples/hexdata.txt", "-o", "out.yz", "-adeeznuts"),\
	       "Didn't fail with invalid extension")

	test("Saving tree" in callc("-c", "samples/hexdata_small.txt", "-o", "out.yz", "-O", "-s", "out.yt",
		"-astatic", "-ptree=hex2"), "Didn't save tree outside of file")

	test("HF_NOTREE set" in callc("-x", "out.yz", "-o", "out.txt"), "Didn't fail w/o tree (or tree was saved)")

	test(callc("-x", "out.yz", "-o", "out.txt", "-l", "out.yt").code==0, "Failed to decompress w/ external tree")

	test(""==call("diff", "samples/hexdata_small.txt", "out.txt"), "Input != output when using external tree")

	call("rm", "out.yz", "out.yt", "out.txt")

	if bueno:
		print("\033[92m\t\t\t\t\tALL GOOD!\033[0m")

	if not bueno:
		sys.exit(1)

if "nounit" not in sys.argv:
	print("Running unittests...")

	print(call("python", "-m", "build", "tests").value)