# yeezip

##Requirements
 * Nada nada, enchilada!
 * That is, a C compiler (GCC or clang) and libc
 * Also linux. Probably. It compiles under cyguin by sure dosen't work
 * (Optional) If you want to run unittests you'll need to install Criterion on the path somewhere.
 * (Optional) Python 3.4 (headers + lib) somewhere on your path for pyiface

##Building
Run `python3 -m build` to build the core executable. `build` has a couple of options:
 * `rbx` -> Build the extensions (stuff in src/extensions) into SO files in ./extensions
 * `lln` -> Include log zones and filenames in logging
 * `espam` -> Enable log messages at the SPAM and ALLOC levels. Otherwise setting loglevel to 1 or 2 has no effect
 * `dbm` -> Don't build the actual CLI program
 * `nr` -> Don't run the actual CLI program after building (does otherwise)
 * `tests` -> Build for running criterion unittests (requires Criterion installed in the path)

So for your first build you should probably run `python3 -m build rbx lln espam` to get the core and all the modules.

##Using
CLI help can be accessed from `./compress -h`
Example usage
 * `./compress -c foo.txt -o out.yz` Compresses foo.txt to out.yz
 * `./compress -x out.yz - foo_out.txt` Decompresses out.yz to foo.txt
All sorts of other options are available.

##Testing
You can run the integration and unit tests py running `python -m test`. This will run both integration and unit tests.
