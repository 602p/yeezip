"""Build tool for yeezip.

Trying to understand make/cmake/whatever made me sad so I hacked this together.
It's just a dumb command line builder that takes a couple options.

Options are just words on the command line:
	espam 		Sets -DENABLE_SPAM (otherwise log commands below STATUS evaporate at compile-time)
	lln 		Sets -DLOG_LINENO (include filename and line when logging)
	debug 		Same as using `espam lln`
	tests 		Build using the criterion main instead of cli main. Causes linking against libcriterion

run like `python -m build <args...>`
"""

import os, sys

files = [ #List of source code files to include
	"src/main.c",
	"src/codingtree.c",
	"src/map.c",
	"src/log.c",
	"src/args.c",
	"src/io.c",
	"src/extension.c",
	"src/compress.c",
	"src/tests.c"
]

extention_dir="src/extentions" #Directory of extension files
                               #(Those ending in .c are compiled to SOs, .py copied to the extension folder)

output = "compress" #Output executable name

defines={ #Mapping of cli_flag->[preprocessor macro to enable, ...]
	"lln":["LOG_LINENO"],
	"espam":["ENABLE_SPAM"],
	"debug":["LOG_LINENO", "ENABLE_SPAM"],
	"tests":["ENABLE_TESTS"]
}

python_extensions=["testpython","python_interface"] #extension names (stripped of .c)
                                                    #That need to link to libpython3.4m

###########################################################################################
#Horrible crap below

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

basecmd="gcc -ldl -lm -fPIC -I./src"+defines_as_string+" "+files_as_string

if "tests" in sys.argv:
	basecmd+=" -lcriterion "


if "rbx" in sys.argv:
	os.system("rm extentions/*")
	for extention in [x for x in extentions if x.endswith(".c")]:
		fullname=extention_dir+"/"+extention
		barename=extention[:-2]
		soname="extentions/"+barename+".so"
		cmdline=basecmd+" "+fullname+" -shared -o "+soname
		if barename in python_extensions:
			cmdline+=" -lpython3.4m"
		# print("Building extention %s: %s"%(barename, cmdline))

		os.system(cmdline)
	os.system("cp src/extentions/*.py extentions")

cmdline=basecmd+" -o "+output

if "dbm" not in sys.argv:
	# print("Executing:"+cmdline)

	os.system(cmdline)

if "nr" not in sys.argv:
	os.system("./compress")