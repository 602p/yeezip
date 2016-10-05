import random
import sys

with open("hexdata.txt", 'w') as fd:
	for _ in range(1000):
		for _ in range(5):
			fd.write(format(random.randint(0, (16**3)-1), '#012x')+" ")
		fd.write("\n")

with open("hexdata_small.txt", 'w') as fd:
	for _ in range(10):
		for _ in range(5):
			fd.write(format(random.randint(0, (16**3)-1), '#012x')+" ")
		fd.write("\n")