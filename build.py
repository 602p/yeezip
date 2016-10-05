import os, sys

files = [
	"src/main.c",
	"src/codingtree.c",
	"src/map.c",
	"src/log.c",
	"src/args.c",
	"src/io.c",
	"src/extension.c",
	"src/compress.c",
	"src/tests/maptest.c",
	"src/tests/treetest.c",
	"src/tests/functionmaptest.c",
	"src/tests/extensiontest.c",
	"src/tests/mmap.c",
	"src/tests/bittest.c",
	"src/tests/dispatcher.c"
]

extention_dir="src/extentions"

output = "compress"

defines={
	"lln":["LOG_LINENO"],
	"espam":["ENABLE_SPAM"],
	"debug":["LOG_LINENO", "ENABLE_SPAM"]
}

###########################################################################################

files_as_string = " ".join(files)

defines_as_string=""
for arg in sys.argv[1:]:
	if arg in defines:
		for param in defines[arg]:
			defines_as_string+=" -D"+param

extentions = os.listdir(extention_dir)
for arg in sys.argv:
	if arg.startswith("-x"):
		extentions.remove(arg[2:])
	if arg.startswith("-o"):
		extentions=[arg[2:]]

if "rbx" in sys.argv:
	for extention in extentions:
		fullname=extention_dir+"/"+extention
		barename=extention[:-2]
		soname="extentions/"+barename+".so"
		cmdline="gcc -ldl -lm -fPIC -shared -I./src "+fullname+" "+files_as_string+" -o "+soname+" "+defines_as_string
		print("Building extention %s: %s"%(barename, cmdline))
		os.system(cmdline)

cmdline="gcc {files_as_string} -ldl -lm -I./src -o {output} {defines_as_string}".format(**locals())

if "dbm" not in sys.argv:
	print("Executing:"+cmdline)

	os.system(cmdline)

if "nr" not in sys.argv:
	os.system("./compress")