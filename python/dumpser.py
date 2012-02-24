import sys,serial
ser = serial.Serial(sys.argv[1], 57600)
done = False
while not done:
	try:
		sys.stdout.write(ser.read(1))
	except:
		done = True
