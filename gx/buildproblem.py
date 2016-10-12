import sys

frequency=[0 for _ in range(256)]

with open(sys.argv[1], 'r') as fd:
	for char in fd.read():
		if ord(char)<256:
			frequency[ord(char)]+=1

print(frequency)
with open("out.list", 'w') as fd:
	fd.write(str(frequency))