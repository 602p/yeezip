import traceback
print("(Hacky shell running inside yeezip!)")
run=1

def quit():
	global run
	run=0

while run:
	try:
		print(eval(input(">>> ")))
	except BaseException as e:
		traceback.print_exc()